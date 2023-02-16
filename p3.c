/* TITLE: SO LAB
 * SUBTITLE: Practical 3
 * AUTHOR 1: Pablo Lopez Martinez LOGIN 1: pablo.lopezm@udc.es
 * AUTHOR 2: Iker Seoane Curras LOGIN 2: iker.seoane@udc.es
 * GROUP:6.1
 * DATE: 23/09/2022
*/


#include <unistd.h>
#include "libraries.h"



void ProcesarEntrada(char *tr[],arguments* arg);
int TrocearCadena(char * cadena, char * trozos[]);


/*This struct will store a string which will be the name of the command
and the fuction related to itself */
struct CMD {
    char *name;
    void (*func)(char**,arguments* arg);
};

//This fuction will display the name and the acounts of both authors of the shell
void Cmd_autores(char * tr [],arguments *arg){
    if (tr[0] == NULL){ //If there is no parameter it will print both things
        printf("Logins:\npablo.lopezm@udc.es\niker.seoane@udc.es\n");
        printf("Names:\nPablo López Martínez\nIker Seoane Currás\n");
    }
    else if(!strcmp(tr[0], "-l")) //If a parameter -l is introduced then only the accounts would be displayed
        printf("Logins:\npablo.lopezm@udc.es\niker.seoane@udc.es\n");
    else if(!strcmp(tr[0], "-n")) //On the other hand if you use -n if will only print the names
        printf("Names:\nPablo López Martínez\nIker Seoane Currás\n");
}

/*This function will show the pid of the process related to the running shell depending on
wheter you enter the -p parameter or not the parent process pid will be displayed instead of the 
pid of the shell*/
void Cmd_pid(char *tr[],arguments* arg){
     if (tr[0] == NULL){
        int pid = getpid();
        printf("Process pid: %d \n", pid);

    }else if(!strcmp(tr[0], "-p")){
        int ppid = getppid();
        printf("Parent process pid: %d \n", ppid);
    }
}

/*This function informs you about the current directory you are in or it let you 
change the directory*/
void Cmd_carpeta(char * tr[],arguments* arg){
    char dir[MAX];
    if(tr[0]==NULL){
        printf("%s", getcwd(dir, MAX));
        printf("\n");
    }
    else if(chdir(tr[0]) == -1){
        printf("Cannot change directory.\n"); //If it's not possible to change to the desire directory a message will be displayed
        printf(" ");
    }

}

/*A function to return the date and hour in this particular moment, through parameters
-h and -d you decide wheter you only want the hour or the date*/
void Cmd_fecha(char *tr[],arguments* arg) {
    time_t now=time(NULL); 
    struct tm* current_time=localtime(&now); 
    if (tr[0]==NULL) {
        printf("%02d:%02d:%02d\n",current_time->tm_hour,current_time->tm_min,current_time->tm_sec);
        printf("%02d/%02d/%d\n",current_time->tm_mday,current_time->tm_mon+1,(current_time->tm_year)+1900);
    }
    else if (!strcmp(tr[0],"-h")) {
        printf("%02d:%02d:%02d\n",current_time->tm_hour,current_time->tm_min,current_time->tm_sec);
    }
    else if (!strcmp(tr[0],"-d")) {
        printf("%02d/%02d/%d\n",current_time->tm_mday,current_time->tm_mon+1,(current_time->tm_year)+1900);
    }
}

/*This function will display the most relevant information about your machine as your user name or 
the version and name of your operative system*/
void Cmd_infosis(char * tr[],arguments* arg) {
    struct utsname uts;
    if (uname(&uts)!=0) {
        printf("Error");
    }
    else {
        printf("%s (%s), OS: %s-%s-%s\n",uts.nodename,uts.machine,uts.sysname,uts.release,uts.version);
    }
}

/*This function would display all the commands you previously entered and that were stored
in a head node linked list*/
void Cmd_hist(char *tr[],arguments* arg) {
    tPosL p;
    CmdData *info;
    int i = 0;
    if (tr[0] == NULL) { //If there is no parameter passed we just print the complete list
        p = first(arg->historial);
        while (p != last(arg->historial)->next) {
            info = getItem(p, arg->historial);
            printf("%d->%s", i, info->cmd);
            p = next(p, arg->historial);
            i++;
        }
    } else if (!strcmp(tr[0], "-c")) { //If a -c parameter is passed then the list would be cleared
        clearList(&arg->historial);
    } else if (abs(atoi(tr[0])) >0) { //If you pass an integer as a parameter then the function will print until the n command
        int base = abs(atoi(tr[0]));
        i = 0;
        p = first(arg->historial);
        while (i != base) {
            p = next(p, arg->historial);
            i++;
        }
        while (p != last(arg->historial)->next) {
            info = getItem(p, arg->historial);
            printf("%d->%s", i, info->cmd);
            p = next(p, arg->historial);
        }
    }
}

void Cmd_comando(char * tr [], arguments* arg) {
    char command[MAX];
    arg->countCommand++;
    if (arg->countCommand>10) {
        printf("Error recursive loop.\n");
        return;
    }
    char *trozos[MAX/2];
    CmdData* info=NULL;
    tPosL p=first(arg->historial);
    int i=0;
    if (validIndex(tr[0],arg->historial)==-1) {
        return;
    }
    int index=atoi(tr[0]);
    while (i!=index) {
        p=next(p,arg->historial);
        i++;
    }
    info=getItem(p,arg->historial);
    printf("Executing command (%d) in the historic: %s",i,info->cmd);
    strcpy(command,info->cmd);
    TrocearCadena(command,trozos);
    ProcesarEntrada(trozos,arg);
}

//This function displays all the commands information and their multiple parameters
void Cmd_ayuda(char *tr[],arguments* arg) {
    if (tr[0]==NULL) {
        printf("'ayuda cmd' donde cmd es uno de los siguientes comandos:\nfin salir bye fecha pid autores hist comando carpeta infosis ayuda\n");
    }
    else {
        if (!strcmp(tr[0],"fin")) {
            printf("fin 	Ends the execution of the shell\n");
        }   
        else if (!strcmp(tr[0],"salir")) {
            printf("salir  	 Ends the execution of the shell\n");
        }
        else if (!strcmp(tr[0],"bye")) {
            printf("bye    Ends the execution of the shell\n");
        }
        else if (!strcmp(tr[0],"fecha")) {
            printf("fecha [-d| [-h]	Shows the date and current hour\n");
        }
        else if (!strcmp(tr[0],"pid")) {
            printf("pid [-p]	Shows the pid of the shell or its parent process\n");
        }
        else if (!strcmp(tr[0],"autores")) {
            printf("autores [-n] [-l]	Shows the name and logins of the authors\n");
        }
        else if (!strcmp(tr[0],"hist")) {
            printf("hist [-c] [-N]	Shows the historic of the commands\n-N Show the N first elements \n-c :the historic is cleared\n");
        }
        else if (!strcmp(tr[0],"comando")) {
            printf("comando [-N]	Repite the command N (of the historic\n");
        }
        else if (!strcmp(tr[0],"carpeta")) {
            printf("carpeta [dir]	Changes (or shows) the current directory of the shell\n");
        }
        else if (!strcmp(tr[0],"infosis")) {
            printf("infosis 	Displays the information of the machine where the shell is run\n");
        }
        else if (!strcmp(tr[0],"ayuda")) {
            printf("ayuda [cmd]	   Displays information about the commands\n");
        }
        else if (!strcmp(tr[0],"create")) {
            printf("create [f] [name]     Creates files or directories\n-f: Creates files\n");
        }
        else if (!strcmp(tr[0],"stat")) {
            printf("stat [-long][-link][-acc] name1 name2 ..	Information about the files passed as parameters\n-long: long listing\n-acc: accesstime\n-link: path contained in the symbolic link\n");
        }
        else if (!strcmp(tr[0],"list")) {
            printf("list [-reca] [-recb] [-hid][-long][-link][-acc] name1 name2 ..	  Displays information about the commands\n");
            printf("-hid: includes hidden files\n-reca: recursives (before)\n-recb: recursive (before)\nThe rest of parameters same as stat\n");
        }
        else if (!strcmp(tr[0],"delete")) {
            printf("delete name1 name2 ..	 Delete the files\n");
        }
        else if (!strcmp(tr[0],"deltree")) {
            printf("deltree name1 name2 ..	 Delete recursively\n");
        }
        else if (!strcmp(tr[0],"allocate")) {
            printf("allocate [-malloc|-shared|-createshared|-mmap] ...\tAsign a memory block\n"
                   "\t\t-malloc size : asign a malloc block of size tam\n"
                   "\t\t-createshared cl tam: asign (creating) the memory shared block of key cl and size tam\n"
                   "\t\t-shared cl: asign a shared memory block with key cl\n"
                   "\t\t-mmap fich perm: maps the file fich, perm are the permissions");
        }
        else if (!strcmp(tr[0],"deallocate")) {
            printf("deallocate [-malloc|-shared|-mmap]..\tUnasign a memory block\n"
                   "\t\t-malloc size: frees the memory block of size tam\n"
                   "\t\t-shared cl: frees the shared memory block of key cl\n"
                   "\t\t-mmap fich: unmaps the mapped file fich\n"
                   "\t\taddr: frees the memory block in address addr");
        }
        else if (!strcmp(tr[0],"i-o")) {
            printf("i-o [read|write] [-o] fiche addr cont \t Reads or writes cont bytes from (a) fiche to (from) addr\n");
        }
        else if (!strcmp(tr[0],"memdump")) {
            printf("memdump addr cont \t Shows the content of 'cont' size from memory address addr");
        }
        else if (!strcmp(tr[0],"memfill")) {
            printf("memfill addr cont byte \tFills the memory from addr with byte\n");
        }
        else if (!strcmp(tr[0],"memory")) {
            printf("memory [-blocks|-funcs|-vars|-all|-pmap] ..\tShows details of the process memory\n"
                   "\t\t-blocks: the assigned memory blocks\n"
                   "\t\t-funcs: the addresses of the functions\n"
                   "\t\t-vars: the addresses of the variables\n"
                   "\t\t:-all: everything\n"
                   "\t\t-pmap: shows the output of the command pmap(or similar)\n");
        }
        else if (!strcmp(tr[0],"recurse")) {
            printf("recurse [n]\t Invokes the function recursive n times\n");
        }
        else if (!strcmp(tr[0],"priority")){
            printf("priority [pid] [value] \tShows or changes the priority of the process pid to value\n");
        }
        else if (!strcmp(tr[0],"showvar")) {
            printf("showvar var1\tShows the value and the directions of the environment variable var\n");
        }
        else if (!strcmp(tr[0],"changevar")) {
            printf("changevar [-a|-e|-p] var valor\t Changes the value of one environment variable\n"
                   "\t-a: access by the third arg of main\n"
                   "\t-e: access by environ\n"
                   "\t-p: access by putenv\n");
        }
        else if (!strcmp(tr[0],"showenv")) {
            printf("showenv [-environ|-addr] \t Shows the environment of the process\n");
        }
        else if (!strcmp(tr[0],"fork")) {
            printf("fork \tMakes a call fork to create a process\n");
        }
        else if (!strcmp(tr[0],"execute")) {
            printf("execute prog args....\tExecutes, without creating a process,prog with arguments\n");
        }
        else if (!strcmp(tr[0],"listjobs")) {
            printf("listjobs \tList the processes in the background\n");
        }
        else if (!strcmp(tr[0],"deljobs")) {
            printf("deljobs [-term][-sig]\tDeletes the processes of the list of processes in sp\n"
                   "\t-term: the terminated ones\n"
                   "\t\t-sig: the terminated by signal\n");
        }
        else if (!strcmp(tr[0],"job")) {
            printf("job [-fg] pid\tShows information of the process pid.\n"
                   "\t\t-fg: to put the process in the foreground\n");
        }
        else {
            printf("Command not found\n");
        }
    }
}

/*A function to create files or directories*/
void Cmd_create(char *tr[],arguments* arg) {
    char dir[MAX];
    if (tr[0]==NULL) {
        printf("%s", getcwd(dir, MAX));
        printf("\n");
    }
    else if (!strcmp(tr[0],"-f")) {
        createFile(tr);
    }
    else {
        createDirectory(tr);
    }
}

/*This function will display information about the file you pass as parameter*/
void Cmd_stat(char *tr[],arguments* arg) {
    struct parameters param={0,0,0,0,0,0};
    int i=0;
    int size=0;

    if (tr[0]==NULL) {
        Cmd_carpeta(&tr[0],arg);
        return;
    }
    while (tr[i]!=NULL) { //We search through the input to find the parameteres you have entered
        if (!strcmp(tr[i],"-long")) param.lng=1;
        if (!strcmp(tr[i],"-acc")) param.acc=1;
        if (!strcmp(tr[i],"-link")) param.link=1;
        i++;
    }
    size=i;
    i=param.lng+param.link+param.acc;
    if (i==size) Cmd_carpeta(&tr[i+1],arg); //If the input are only parameters
    else {
        while (i<size) {
            if (printStats(tr[i],param)==-1) printf("Error trying to access the file info\n");
            i++;
        }
    }
}


/*List the contents of the directories*/ 
void Cmd_list(char *tr[],arguments* arg) {
    struct parameters param={0,0,0,0,0,0};
    int size=0;
    int i=0;
    while (tr[i]!=NULL) {
        if (!strcmp(tr[i],"-hid")) param.hid=1; 
        if (!strcmp(tr[i],"-reca")) param.reca=1; 
        if (!strcmp(tr[i],"-recb")) param.recb=1; 
        if (!strcmp(tr[i],"-long")) param.lng=1; 
        if (!strcmp(tr[i],"-acc")) param.acc=1; 
        if (!strcmp(tr[i],"-link")) param.link=1; 
        i++;
    }
    size=i;
    i=param.hid+param.reca+param.recb+param.lng+param.acc+param.link;
    if (i==size) {
        if (tr[0]==NULL)  {
            Cmd_carpeta(&tr[0],arg);
            return;
        }
        Cmd_carpeta(&tr[i+1],arg); //If the input are only parameters
        return;
    }
    while (tr[i]!=NULL) {
        displayList(tr[i],param);
        i++;
    }
}


//Delete the selected file
void Cmd_delete(char *tr[],arguments* arg) {
    int i=0;
    int flag=0;
    struct dirent* currentd;
    DIR * dir=opendir(".");
    if(dir==NULL)  {
        printf("Error to open current directory\n");
        return;
    }
    if (tr[0] == NULL){
        printf("Need to specify an empty file or a directory in the current path.\n");
        return;
    }
    closedir(dir);
    while (tr[i]!=NULL) {
        flag=0; 
        dir = opendir(".");
        currentd = readdir(dir);
        while((currentd=readdir(dir))){ 
            if (!strcmp(currentd->d_name,".") || !strcmp(currentd->d_name,"..")) continue;
            if (!strcmp(currentd->d_name, tr[i])){ //Checks for a file with the same name as the one especified
                flag=1;
                if (remove(tr[i]))  printf("Error deleting the element\n");
            }
        }
        if (!flag) printf("File '%s' does not exist.\n",tr[i]);
        i++;
        closedir(dir);
    }
}


//Delete recursively
void Cmd_deltree(char *tr[],arguments* arg) {
    int i = 0;
    if (tr[0] == NULL){
        printf("Need to specify an empty file or a directory in the current path.\n");
        return;
    }
    while (tr[i]!=NULL) { //Checks for every parameter
        if (deleteRec(tr[i])) printf("Error trying to delete\n"); //If any directory could not be removed, the function is called recursively in order to delete all the files/directories
        i++;
    }
}

//Allocates memory
void Cmd_allocate(char* tr[],arguments* arg) {

    if (tr[0] == NULL ) {
        printAllLists(arg);
        return;
    }
    if(!strcmp(tr[0], "-malloc")) {
        if (tr[1] == NULL) {
            printf("List of blocks assigned to the process: %d\n", getpid());
            printMallocList(*arg);
            return;
        } else {//to function
            size_t blockSize;
            blockSize = strtoul(tr[1],NULL,10);
            if (blockSize==0){
                printf("Cannot assign blocks of 0 bytes.\n");
                return;
            }
            void *address=malloc(blockSize);
            if (address == NULL) { //Memory control line
                printf("Cannot allocate: not enough memory\n");
                return;
            }
            printf("Assigned %ld bytes in %p\n", blockSize, address);
            insertMallocNode(&(arg->malloc), address, blockSize);
            }
        }
    if(!strcmp(tr[0], "-createshared")) {
        do_AllocateCreateshared(&tr[1], arg);
        return;
    }
    if(!strcmp(tr[0], "-shared")) {
        key_t key=strtoul(tr[1],NULL,10);
        void* p= ObtainMemoryShmget(key,0,arg);
        if (p==NULL) {
            printf("Impossible to assign shared memory key %d\n",key);
            return;
        }
        printf("Shared memory of key %d in %p\n",key,p);
    }
    if(!strcmp(tr[0], "-mmap")) {
        do_AllocateMmap(&tr[1],arg);
        return;
    }
}


//Deallocates memory
void Cmd_deallocate(char* tr[],arguments* arg) {
    if (tr[0] == NULL) {
        printAllLists(arg);
        return;
    }
    if(!strcmp(tr[0], "-malloc")) {
        if (tr[1] == NULL) return;
        size_t size;
        size = strtoul(tr[1], NULL, 10);
        tPosL pos = findSize(size, arg);
        if (size==0 || pos ==NULL) {
            printf("There are no blocks of such size\n");
            return;
        }
        deleteItem2(&arg->malloc,pos,freeMalloc);
    }

    if(!strcmp(tr[0], "-shared")) {
        if(tr[1] != NULL) {
            key_t key = strtoul(tr[1], NULL, 10);
            tPosL pos = findKey(key, arg);
            if (pos == NULL) {
                printf("There is no block for that key in the process.\n");
                return;
            }
            deleteItem2(&arg->shared,pos,free);
            return;
            }
        else printf("There are not enough parameters.\n");
    }

    if(!strcmp(tr[0], "-delkey"))
        do_DeallocateDelkey(&tr[1]);

    if(!strcmp(tr[0], "-mmap")) {
        if (tr[1]==NULL)  {
            printf("List of blocks assigned to the process: %d\n", getpid());
            printMallocList(*arg);
            return;
        }
        void* p=findFile(tr[1],arg);
        if (p==NULL) printf("%s not mapped.\n",tr[1]);
        deleteItem2(&arg->mmap,p,freeMmap);
    }
     if (!strcmp(tr[0],"-addr")) {
          do_deallocateAddr(&tr[1],arg);
     }
}

//Reads or writes (in) a file
void Cmd_i_o(char* tr[],arguments* arg) {
    if (tr[0]==NULL) {
        printf("Not enough parameters.\n");
        return;
    }
    if (!strcmp(tr[0],"-read")) {
        do_I_O_read(&tr[1]);
    }
    if (!strcmp(tr[0],"-write")) {
        do_I_O_write(&tr[1]);
    }
}

//Repeats the function recursively
void Cmd_recursive(char* tr[],arguments* arg) {
    int n;
    if (tr[0]==NULL) return;
    n=atoi(tr[0]);
    Recursive(n);
}

//Displays memory in the screen
void Cmd_memdump(char* tr[],arguments* arg) {
    size_t cont=MAXSIZE;
    void *p;
    if (tr[0]==NULL)
        return;
    if (tr[1]!=NULL)
        cont=(size_t) strtoul(tr[1],NULL,10);
    sscanf(tr[0],"%p",&p);
    printf("Dumping %zu bytes from address %p\n",cont,p);
    showMem(p,cont);
    return;
}

//Fills memory with indicated char
void Cmd_memfill(char* tr[],arguments* arg) {
    int i=0;
    size_t c=128;
    unsigned char byte='A';
    void* address;

    while (tr[i]!=NULL) i++;
    if (i==0) return;
    if (i>=2) c=atoi(tr[1]);
    if (i==3) byte=(*tr[2]);
    sscanf(tr[0],"%p",&address);

    printf("Filling %ld bytes from memory with byte (%c) from address %p.\n",c,byte,address);
    FillMemory(address,c,byte);
}



//Prints information about the memory
int gl1=0,gl2=0,gl3=0;
void Cmd_memory(char * tr[], arguments* arg){ //No se que es context
auto int l1=0,l2=0,l3=0;
static int s1=0,s2=0,s3=0;
    if (tr[0]==NULL || !strcmp(tr[0], "-all")) {
        printf("Local variables:\t%10p, %10p, %10p\n", &l1, &l2, &l3);
        printf("Global variables:\t%10p, %10p, %10p\n", &gl1, &gl2, &gl3);
        printf("Static variables:\t%10p, %10p, %10p\n", &s1, &s2, &s3);
        printf("Program functions:\t%10p, %10p, %10p\n", printSharedList , printMallocList , printMmapList);
        printf("Library functions:\t%10p, %10p, %10p\n", printf, scanf, atoi);
        printMallocList(*arg);
        printMmapList(*arg);
        printSharedList(*arg);
        return;
    }
    else if(strcmp(tr[0], "-vars")== 0) {
        printf("Local variables:\t%10p, %10p, %10p\n", &l1, &l2, &l3);
        printf("Global variables:\t%10p, %10p, %10p\n", &gl1, &gl2, &gl3);
        printf("Static variables:\t%10p, %10p, %10p\n", &s1, &s2, &s3);
    }
    else if(strcmp(tr[0], "-funcs")== 0){

        printf("Program functions:\t%10p, %10p, %10p\n", printSharedList , printMallocList , printMmapList);
        printf("Library functions:\t%10p, %10p, %10p\n", printf, scanf, atoi);
    }
    else if(strcmp(tr[0], "-blocks")== 0){
        Cmd_allocate(NULL, arg);
    }
    else if(strcmp(tr[0], "-pmap")== 0){
         Do_pmap();
    }
    else {
        printf("Invalid parameter");
    }
}

//Gets or change the priority
void Cmd_priority(char *tr[],arguments *arg) {
    int priority,pid;
    if (tr[0]==NULL) {
        pid=getpid();
        priority=getpriority(PRIO_PROCESS,pid);
    }
    else {
        pid=atoi(tr[0]);
        if (tr[1]!=NULL) {
            priority=atoi(tr[1]);
            setpriority(PRIO_PROCESS, pid, priority);
        }
        else {
            priority= getpriority(PRIO_PROCESS,pid);
        }
    }
    printf("The process %d has priority %d\n",pid,priority);
}

//Prints the environment
void Cmd_showenv(char *tr[], arguments *arg) {
    if (tr[0] == NULL) {
        ShowEnv(*arg->main_env, "main arg3");
        return;
    }
    if (!strcmp(tr[0], "-environ")) ShowEnv(environ,"environ");
    else if (!strcmp(tr[0],"-addr")) {
        printf("environ: %p (stored in %p)\n",&environ[0],&environ);
        printf("main arg3: %p (stored in %p)\n",&environ[0],&arg->main_env);
    }
}

//Shows the content of the variable
void Cmd_showvar(char *tr[], arguments *arg) {
    if (tr[0] == NULL){
        ShowEnv(*arg->main_env,"main arg3");
        return;
    }
    char *envNum;
    int mainEnv, env;
    mainEnv = BuscarVariable(tr[0],*arg->main_env);
    env = BuscarVariable(tr[0], environ);
    envNum = getenv(tr[0]);

    if(mainEnv == -1) {
        if (env != -1) {
            if (envNum != NULL) {
                printf("With environ %s = %s(%p) @%p\n", tr[0], envNum, environ[env], &environ[env]);
                printf("  With getenv %s(%p)\n", envNum, &envNum);
                return;
            }
            printf("There is no such variable");
            return;
            }
        }
    else{
            printf("\tWith arg3 main %s=:%s(%p) @%p\n", tr[0], envNum, (*arg->main_env)[mainEnv], &(*arg->main_env)[mainEnv]);
            printf("\tWith environ %s=:%s(%p) @%p\n", tr[0], envNum, environ[env], &environ[env]);
            printf("\tWith getenv %s(%p)\n", envNum, &envNum);
        }
    return;
}


//change the value of the environment variable
void Cmd_changevar(char* tr[], arguments *arg){
    int pos;
    char* change;

    if (tr[0] == NULL || tr[1] == NULL || tr[2] == NULL) {
        printf("Use: changevar [-a | -e | -p] var value\n ");
        return;
    }
    int size = strlen(tr[1]) + strlen(tr[2]);
    if(!strcmp(tr[0],"-a")){
        if ((pos = BuscarVariable(tr[1],*arg->main_env) != -1))
            if (((arg->main_env)[pos] = (char **) malloc(size) ) != NULL){
                sprintf((*arg->main_env)[pos], "%s=%s",tr[1],tr[2]);
                return;
            }
    }
    if(!strcmp(tr[0],"-e")){
        if ((pos = BuscarVariable(tr[1],environ)) != -1)
            if ((environ[pos] = (char *) malloc(size) ) != NULL){
                sprintf((environ)[pos], "%s=%s",tr[1],tr[2]);
                return;
            }
    }
    if(!strcmp(tr[0],"-p"))
        if ((change = (char *) malloc(size)) != NULL){
            sprintf(change,"%s=%s",tr[1],tr[2]);
            putenv(change);
            return;
        }

    printf("Use: changevar [-a | -e | -p] var value\n ");
    return;
}

void Cmd_fork (char *tr[], arguments *arg){
    pid_t pid;
    if ((pid=fork())==0){
	 //EmptyProcessList(&LP); Depends on each implementation
        printf ("Executing process: %d\n", getpid());
    }
    else if (pid!=-1)
        waitpid (pid,NULL,0);
}

void Cmd_execute(char *tr[], arguments *arg) {
    char* priority=NULL;
    char* arguments[MAX];
    int i=0;
    if(tr[0] == NULL){
        printf("Missing arguments\n");
        return;
    }
    while (tr[i]!=NULL) {
        if (tr[i][0]=='@') priority=&tr[i][1];
        else arguments[i]=tr[i];
        i++;
    }
    arguments[i]=NULL;
    if (priority!=NULL) {
        setpriority(PRIO_PROCESS, getpid(), atoi(priority));
        arguments[i-1]=NULL;
    }
    OurExecvpe(tr[0],arguments,*arg->main_env);
    return;
}

void Cmd_listjobs(char *tr[], arguments *arg) {
    if (isEmptyList(arg->jobs)) return;
    tPosL p = first(arg->jobs);
    jobData * node=NULL;
    while (p != NULL){
        node = getItem(p,arg->jobs);
        printJobNode(node);
        p = next(p,arg->jobs);
    }
}

void Cmd_deljobs(char *tr[], arguments *arg) {
    int value;
    bool term = false, sig = false;
    tPosL p = first(arg->jobs);
    if(tr[0] == NULL){
        Cmd_listjobs(tr,arg);
        return;
    }

    if (!strcmp(tr[0],"-term")) term=true;
    else if (!strcmp(tr[0],"-sig")) sig=true;
    else {
        Cmd_listjobs(tr,arg);
        return;
    }

    while (p != NULL) {
        jobData* node=getItem(p,arg->jobs);
        value=node->value;
        updateNode(node);
        if (node->value!=value) {
            printJobNode(node);
            p=next(p,arg->jobs);
            continue;
        }
        if (term && WIFEXITED(node->value)) {
            tPosL temp = p;
            p=next(p,arg->jobs);
            deleteItem2(&arg->jobs, temp, free);
            continue;
        }
        if (sig && WIFSIGNALED(node->value)) {
            tPosL temp = p;
            p=next(p,arg->jobs);
            deleteItem2(&arg->jobs, temp, free);
            continue;
        }
        p=next(p,arg->jobs);
    }
}

void Cmd_job(char *tr[], arguments *arg) {
    tPosL p;
    if(tr[0] == NULL) {
        Cmd_listjobs(tr, arg);
        return;
    }
    if(strcmp(tr[0], "-fg") == 0){
        if((p = getPid(arg, (pid_t)atoi(tr[1]))) == NULL){
            Cmd_listjobs(tr,arg);
        }else{
            struct jobData *node = getItem(p,arg->jobs);
            //Changing from background to foreground
            waitpid(node->pid, NULL, SIGCONT);
            deleteItem2(&arg->jobs,p,free);
        }
        return;
    }
    if((p = getPid(arg, (pid_t)atoi(tr[0]))) == NULL){
        Cmd_listjobs(tr,arg);
        return;
    }
    jobData *node = getItem(p,arg->jobs);
    printJobNode(node);
    return;
}

//Used only if you wanted to exit the shell
void Cmd_fin(char * tr [],arguments* arg){
    exit(0);
}

void Cmd_program(char* tr[],arguments* arg) {
    int i=0,a=0,j=0;
    char file[MAX],aux[MAX]="";;
    char *environment[MAX],*arguments[MAX];
    environment[0]=NULL;
    arguments[0]=NULL;
    char* priority=NULL;
    pid_t pid=fork();

    while(tr[i]!=NULL && strcmp(tr[i],"&")) {
        if(getenv(tr[i]) != NULL){
            environment[i]=environ[BuscarVariable(tr[i],environ)];
            a++;
        }
        strcat(aux,tr[i]);
        strcat(aux," ");
        if (tr[i][0]=='@') {
            priority=&tr[i][1];
            i++;
            continue;
        }
        if (i>=a) {
            arguments[j]=tr[i];
            j++;
        }
        i++;
    }
    if (priority!=NULL) setpriority(PRIO_PROCESS,pid,atoi(priority));
    arguments[j]=NULL;
    environment[a]=NULL;
    strcpy(file,tr[a]);

    if ((int) pid == 0){
        if (environment[0]!=NULL) {
            if (OurExecvpe(file, arguments, environment) == -1) {
                Cmd_fin(NULL,NULL);
                printf("No executed: No such file or directory.\n");
                return;
            }
        }
        else {
            if (OurExecvpe(file, arguments, *arg->main_env) == -1) {
                printf("No executed: No such file or directory.\n");
                Cmd_fin(NULL,NULL);
                return;
            }
        }
    }
    if (tr[i]!=NULL && !strcmp(tr[i],"&")){
        uid_t uid = getuid();
        insertJobNode(&arg->jobs, pid,aux, getpriority(PRIO_PROCESS,pid),uid);
        return;
    }
    waitpid(pid,NULL,0);
    return;
}

//This struct will store all the commands with their respective functions
struct CMD c[]= {
        {"autores",Cmd_autores},
        {"carpeta",Cmd_carpeta},
        {"pid",Cmd_pid},
        {"infosis",Cmd_infosis},
        {"ayuda",Cmd_ayuda},
        {"hist",Cmd_hist},
        {"comando",Cmd_comando},
        {"fin",Cmd_fin},
        {"salir",Cmd_fin},
        {"bye",Cmd_fin},
        {"fecha",Cmd_fecha},
        {"create",Cmd_create},
        {"stat",Cmd_stat},
        {"list",Cmd_list},
        {"delete",Cmd_delete},
        {"deltree",Cmd_deltree},
        {"recursive",Cmd_recursive},
        {"allocate",Cmd_allocate},
        {"deallocate",Cmd_deallocate},
        {"i-o",Cmd_i_o},
        {"memfill",Cmd_memfill},
        {"memdump",Cmd_memdump},
        {"memory",Cmd_memory},
        {"priority",Cmd_priority},
        {"showenv", Cmd_showenv},
        {"changevar",Cmd_changevar},
        {"showvar",Cmd_showvar},
        {"fork",Cmd_fork},
        {"execute",Cmd_execute},
        {"job",Cmd_job},
        {"listjobs",Cmd_listjobs},
        {"deljobs",Cmd_deljobs},
        {NULL,NULL}
};

/*A function to prepare the string in order to distinguish parameters and commmand, so
it can be processed through the ProcesarCadena() function*/
int TrocearCadena(char * cadena, char * trozos[]){
    int i=1;
    if ((trozos[0]=strtok(cadena," \n\t"))==NULL)
        return 0;
    while ((trozos[i]=strtok(NULL," \n\t"))!=NULL)
        i++;
    return i;
}

//This will process the pieces left by TrocearCadena() and would run the corresponding function
void ProcesarEntrada (char *tr[],arguments* arg) {
    int i;
    if (tr[0] == NULL)
        return;
    for (i = 0; c[i].name != NULL; i++) {
        if (!strcmp(c[i].name, tr[0])) {
            (*c[i].func)(tr + 1, arg);
            arg->countCommand = 0;
            return;
        }
    }
    Cmd_program(tr,arg);
}


int main( int argc, char * argv [] ,char *env[]){ //

    char entrada[MAX];
    char *trozos[MAX/2];
    arguments arg;
    createEmptyList(&arg.historial);
    createEmptyList(&arg.malloc);
    createEmptyList(&arg.mmap);
    createEmptyList(&arg.shared);
    createEmptyList(&arg.jobs);
    arg.countCommand=0;
    arg.main_env = &env;
    while(1){
        CmdData* info=malloc(sizeof(CmdData));
        printf("->");
        fgets(entrada, MAX, stdin);
        strcpy(info->cmd,entrada);
        insertItem(info,&arg.historial);
        if(TrocearCadena(entrada, trozos) == 0) {
            continue;
        }
        if (end(trozos)) break;
        ProcesarEntrada(trozos,&arg);
    }
    deallocateList(&arg.historial,free);
    deallocateList(&arg.malloc,freeMalloc);
    deallocateList(&arg.shared,free);
    deallocateList(&arg.mmap,freeMmap);
    deallocateList(&arg.jobs,free);
}


