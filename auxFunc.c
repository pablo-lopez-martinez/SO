#include "libraries.h"
 
bool end(char* tr[]){
    if (!strcmp(tr[0],"fin")) return true;
    if (!strcmp(tr[0],"salir")) return true;
    if (!strcmp(tr[0],"bye")) return true;
    return false;
}

void actual_time(char act_time[]) {
    time_t t = time(NULL);
    strftime(act_time, 100, "%b %d %H:%M", localtime(&t));}


//Cmd_create

void createFile(char* tr[]) {
    if (fopen(tr[1],"r")!=NULL) //We try to read the file to check if it already exists
        printf("Failed to create. File already exists.\n");
    else {
        if (fopen(tr[1],"a")==NULL)  //We create the file
            printf("Failed to create the file.\n");
    }
}

void createDirectory(char* tr[]) {
    if (opendir(tr[0])!=NULL)  //We try to open the directory to check if it already exists
        printf("Failed to create. File already exists\n");
    else {
        if (mkdir(tr[0],0700)!=0)  //We create the directory
             printf("Failed to create\n");
    }
}

// Cmd_stat

//Function to print the permissions of a file
char LetraTF (mode_t m) { 
        if (S_ISSOCK(m)) return 's'; /*socket */
        if (S_ISLNK(m)) return 'l'; /*symbolic link*/
        if (S_ISBLK(m)) return 'b'; /*block device*/
        if (S_ISDIR(m)) return 'd'; /*directorio */ 
        if (S_ISCHR(m)) return 'c'; /*char device*/
        if (S_ISFIFO(m)) return 'p'; /*pipe*/
        return '-'; /* fichero normal*/
     } 
int printPermissions(struct stat info) { 
        printf("%c",LetraTF(info.st_mode));
        printf((info.st_mode & S_IRUSR) ? "r" : "-");
        printf((info.st_mode & S_IWUSR) ? "w" : "-");
        printf((info.st_mode & S_IXUSR) ? "x" : "-");
        printf((info.st_mode & S_IRGRP) ? "r" : "-");
        printf((info.st_mode & S_IWGRP) ? "w" : "-");
        printf((info.st_mode & S_IXGRP) ? "x" : "-");
        printf((info.st_mode & S_IROTH) ? "r" : "-");
        printf((info.st_mode & S_IWOTH) ? "w" : "-");
        printf((info.st_mode & S_IXOTH) ? "x" : "-");
        return 0;
}

int printLng(struct stat info,int checkAcc) {
     // Last time modified
        if (!checkAcc) {
            char date[MAX];
            strftime(date, MAX, "%Y-%m-%d-%H:%M", localtime(&(info.st_ctime))); 
            printf("%s  ",date);
        }
        // Number of hard links
        printf("%2ld ", info.st_nlink);
        // Inode number
        printf(" %10ld   ", info.st_ino);
        // User and group
        struct passwd *pw = getpwuid(info.st_uid);
        struct group *gr = getgrgid(info.st_gid);
        printf("%3s  ", pw->pw_name==NULL?"UNKNOWN":pw->pw_name);
        printf("%3s  ", gr->gr_name==NULL?"UNKNOWN":gr->gr_name);
        // Permissions
        printPermissions(info);
        return 0;
}

int printAcc(struct stat info) {
    char accTime[MAX];
        strftime(accTime, MAX, "%Y-%m-%d-%H:%M", localtime(&(info.st_atime))); //Access time
        printf("  Access time: %s",accTime);
        return 0;
}

int printLink(char* file) {
    char path[MAX];
    int n=readlink(file,path,MAX);
        if (n ==-1)
            printf("Cannot access link\n");
        else {
            path[n]='\0';
            printf(" -> %s ",path);
        }
        return 0;
}

int printStats(char* file, struct parameters param)
{
    struct stat info;
    if (lstat(file, &info) == -1) {
        return -1;
    }
    if (param.lng == 1)
        printLng(info,param.acc);
    if (param.acc==1) 
        printAcc(info);
    printf(" %10ld %10s", info.st_size, basename(file));
    if (param.link==1 && S_ISLNK(info.st_mode)) 
        printLink(file);
    printf("\n");


    return 0;
}

// Cmd_list

bool isHidden(char* name){
    return (name[0] == '.' || !strcmp(name,".") || !strcmp(name,".."));
}

int listRec(char* tr,struct parameters param) {
    struct dirent *currentd;
    struct stat info;
    DIR * dir=opendir(tr);
    char path[MAX];
    if (dir==NULL) {
        printf("Error trying to access the directory\n");
        return -1; //If we cannot open the directory throw an error
    }
    while ((currentd=readdir(dir))) {
        if (currentd->d_type==DT_LNK) continue;
        sprintf(path, "%s/%s",tr,currentd->d_name); //Concat the actual directory with the previous path
        stat(path,&info);
        if (S_ISDIR(info.st_mode) && !isHidden(currentd->d_name) && !S_ISLNK(info.st_mode)) { //If it's a directory and is not hidden then print its contents
            displayList(path,param);
        }
    }
    closedir(dir);
    return 0;
}

int displayList(char* tr, struct parameters param) {
    DIR* dir;
    struct dirent* currentd;
    char path[MAX];
    if (param.recb) {
        if (listRec(tr,param)) return -1; //If -recb then we call the recursive function and then print the contents of the dir
    }
    dir=opendir(tr);
    if (dir==NULL) {
        printf("Could not access the directory.\n");
        return -1;
    }
    printf("**********%s:\n",tr);
    while ((currentd=readdir(dir))) {
        sprintf(path, "%s/%s",tr,currentd->d_name);
        if (isHidden(currentd->d_name)&& !param.hid) continue;
        if (printStats(path,param)) printf("Error trying to access '%s' info.\n",path);
    }
    closedir(dir);
    if (param.reca && !param.recb) { //If -reca then we print the contents of the dir and then call the recursive function
        if (listRec(tr,param)) return -1;
    }
    return 0;
}

//Cmd_deltree

int deleteRec(char* entry){
    struct stat info;
    if(lstat(entry, &info) == -1) {
        printf("Could not access %s\n", entry);
        return -1;
    }
    if (S_ISDIR(info.st_mode)) { //To check if the input is a directory
        DIR* dir;
        struct dirent* currentd;
        dir= opendir(entry);
        if(dir == NULL) {
            printf("Could not open %s\n", entry);
            return -1;
        }
        while ((currentd=readdir(dir))) { //We iterate through the directory
            if(!strcmp(currentd->d_name,".") || !strcmp(currentd->d_name,"..")) continue;
            char path[MAX];
            sprintf(path, "%s/%s", entry, currentd->d_name); //We create the new path by       
            deleteRec(path); //If any directory could not be removed, the function is called recursively in order to delete all the files/directories 
        }closedir(dir);
     } 
     if (remove(entry) == -1)  printf("Error deleting %s\n",entry);
     return 0;
}

//Cmd_comando

int validIndex(char* index,tList L){
    int numDigits=0;
    while (isdigit(index[numDigits])) {
        numDigits++;
    }
    if (atoi(index)<0) {
        printf("The index must be positive.\n");
        return -1;
    }
    int i=atoi(index);
    if (i>listLength(L)) {
        printf("The index %d is not in the hist\n",i);
        return -1;
    }
    return 0;
}

//Cmd_allocate


    //-malloc

void insertMallocNode(tList *mallocList, void* address, ssize_t blockSize){
    char act_time[MAX];
    struct mallocData *node=malloc(sizeof(mallocData));
    if (node == NULL) {
        printf("Cannot allocate memory with malloc\n");
        return;
    }
    node->address = address;
    actual_time(act_time);
    strcpy(node->date,act_time);
    node->size = blockSize;
    strcpy(node->type,"malloc");
    insertItem(node,mallocList);
    return;
}

void printMallocList(arguments arg){
    tPosL p=first(arg.malloc);
    while (p!=next(last(arg.malloc),arg.malloc)) {
        struct mallocData* node=getItem(p,arg.malloc);
        printf("%p %10ld %s %s\n", node->address, node->size,node->date,node->type);
        p=next(p, arg.malloc);
    }
}

    //-createshared and -shared

void insertSharedNode(tList *sharedList, void* address, ssize_t blockSize, key_t key){
    char act_time[MAX];
    struct sharedData *node=malloc(sizeof(sharedData));
    if (node == NULL){
        printf("Cannot allocate memory with malloc\n");
        return;
    }
    node->address = address;
    actual_time(act_time);
    strcpy(node->date,act_time);
    node->size = blockSize;
    node->key = key;
    strcpy(node->type,"shared");
    insertItem(node,sharedList);
    return;
}

void * ObtainMemoryShmget (key_t clave, size_t tam,arguments *arg){
    void * p;
    int aux,id,flags=0777;
    struct shmid_ds s;

    if (tam)     //tam distito de 0 indica crear
        flags=flags | IPC_CREAT | IPC_EXCL;
    if (clave==IPC_PRIVATE)  //no nos vale
    {errno=EINVAL; return NULL;}
    if ((id=shmget(clave, tam, flags))==-1)
        return (NULL);
    if ((p=shmat(id,NULL,0))==(void*) -1){
        aux=errno;
        if (tam)
            shmctl(id,IPC_RMID,NULL);
        errno=aux;
        return (NULL);
    }
    shmctl (id,IPC_STAT,&s);
    insertSharedNode(&arg->shared, p, s.shm_segsz, clave);
    return (p);
}

void do_AllocateCreateshared (char *tr[], arguments* arg){
    key_t cl;
    size_t tam;
    void *p;
    if (tr[0]==NULL || tr[1]==NULL) {
        printf("List of blocks assigned to the process: %d\n",getpid());
        printSharedList(*arg);
        return;
    }
    cl=(key_t)  strtoul(tr[0],NULL,10);
    tam=(size_t) strtoul(tr[1],NULL,10);
    if (tam==0) {
        printf ("Cannot assign 0-bytes blocks \n");
        return;
    }

    if ((p=ObtainMemoryShmget(cl,tam,arg))!=NULL)
        printf ("Assigned %lu bytes in %p\n",(unsigned long) tam, p);
    else
        printf ("Impossible to assign shared key %lu:%s\n",(unsigned long) cl,strerror(errno));
}

void printSharedList(arguments arg) {
    tPosL p=first(arg.shared);
    while (p!=next(last(arg.shared),arg.shared))
    {
        struct sharedData *node;
        node = getItem(p,arg.shared);
        printf("%p %10ld %s %s (key %d)\n", node->address, node->size,node->date,node->type, node->key);
        p=next(p,arg.shared);
    }
}

    //-mmap

void insertMapNode(tList *mapList, void* address,ssize_t blockSize,char filename[], int descriptor){
    char act_time[MAX];
    struct mmapData *node;
    if ((node = malloc(sizeof(struct mmapData))) == NULL){
        printf("Cannot allocate: not enough memory\n");
        return;
    }
    node->address = address;
    actual_time(act_time);
    strcpy(node->date,act_time);
    node->size = blockSize;
    strcpy(node->fname, filename);
    node->descriptor = descriptor;
    strcpy(node->type,"mmap");

    insertItem(node,mapList);
    return;
}

void * MmapFile (char * fichero, int protection, arguments *agr){
    int df, map=MAP_PRIVATE,modo=O_RDONLY;
    struct stat s;
    void *p;

    if (protection&PROT_WRITE)
        modo=O_RDWR;
    if (stat(fichero,&s)==-1 || (df=open(fichero, modo))==-1)
        return NULL;
    if ((p=mmap (NULL,s.st_size, protection,map,df,0))==MAP_FAILED)
        return NULL;
    insertMapNode(&agr->mmap,p, s.st_size,fichero,df);
    return p;
}

void do_AllocateMmap(char *tr[], arguments *arg){
    char *perm;
    void *p;
    int protection=0;
    if (tr[0]==NULL){
        printf("List of blocks assigned to the process: %d\n",getpid());
        printMmapList(*arg);
        return;
    }
    if ((perm=tr[1])!=NULL && strlen(perm)<4) {
        if (strchr(perm,'r')!=NULL) protection|=PROT_READ;
        if (strchr(perm,'w')!=NULL) protection|=PROT_WRITE;
        if (strchr(perm,'x')!=NULL) protection|=PROT_EXEC;
    }
    if ((p=MmapFile(tr[0],protection,arg))==NULL)
        perror ("Impossible to mmap file\n");
    else {
        printf("File %s mapped in %p\n", tr[0], p);
    }
}

void printMmapList(arguments arg){
    tPosL p=first(arg.mmap);
    while (p!=next(last(arg.mmap),arg.mmap)) {
        struct mmapData *node = getItem(p,arg.mmap);
        printf("%p %10ld %s %s (descriptor %d).\n", node->address, node->size,node->date,node->fname, node->descriptor);
        p=next(p,arg.mmap);
    }
}


void printAllLists(arguments *arg){
    printf("List of blocks assigned to the process: %d\n", getpid());
    printMallocList(*arg);
    printSharedList(*arg);
    printMmapList(*arg);
    return;
}

//Cmd_deallocate
tPosL findKey(key_t key, arguments *arg){
    tPosL p=first(arg->shared);
    while (p!=next(last(arg->shared),arg->shared)) {
        sharedData *node = getItem(p,arg->shared);
        if(node->key == key) return p;
        p=next(p,arg->shared);
    }
    return NULL;
}
tPosL findSize(size_t size, arguments *arg){
    tPosL p=first(arg->malloc);
    while (p!=next(last(arg->malloc),arg->malloc)) {
        sharedData *node = getItem(p,arg->malloc);
        if(node->size == size) return p;
        p=next(p,arg->malloc);
    }
    return NULL;
}
tPosL findFile(char filename[], arguments *arg){
    tPosL  p=first(arg->mmap);
    while (p!=next(last(arg->mmap),arg->mmap)) {
        mmapData *node = getItem(p,arg->mmap);
        if(!strcmp(node->fname, filename)) return p;
        p=next(p,arg->mmap);
    }
    return NULL;
}

tPosL findAddress(void* address, tList list){
    tPosL p =first(list);
    while (p!=next(last(list),list)) {
        sharedData *node = getItem(p,list);
        if(node->address == address){
            return p;
        }
        p=next(p,list);
    }
    return NULL;
}


void freeMalloc(void* p) {
    struct mallocData* node=p;
    free(node->address);
    free(node);
}

void freeMmap(void *p) {
    struct mmapData *node=p;
    munmap(node->address,node->size);
    free(node);
}

void do_DeallocateDelkey (char *args[])
{
    key_t clave;
    int id;
    char *key=args[0];
    if (key==NULL || (clave=(key_t) strtoul(key,NULL,10))==IPC_PRIVATE){
        printf ("delkey needs clave_valida\n");
        return;
    }
    if ((id=shmget(clave,0,0666))==-1){
        perror ("shmget: impossible to obtain shared memory\n");
        return;
    }
    if (shmctl(id,IPC_RMID,NULL)==-1)
        perror ("shmctl: impossible to erase shared memory\n");
    printf("Erased shared memory with key %d\n", clave);
}

void do_deallocateAddr(char *tr[],arguments *arg){
    void* address;
    if(tr[0]!=NULL) {
        sscanf(tr[0], "%p", &address);
        tPosL pos;
        if ((pos = findAddress(address, arg->malloc)) != NULL) {
            deleteItem2(&arg->malloc, pos, freeMalloc);
            return;
        }
        if ((pos = findAddress(address, arg->shared)) != NULL) {
            deleteItem2(&arg->shared, pos, free);
            return;
        }
        if ((pos = findAddress(address, arg->mmap)) != NULL) {
            deleteItem2(&arg->mmap, pos, freeMmap);
            return;
        }
    }printf("There is no address %p allocated with malloc,shared or mmap\n", address);
    return;
}

//Cmd_i_o

ssize_t ReadFile (char *f, void *p, size_t cont){
    struct stat s;
    ssize_t  n;
    int df,aux;
    if (stat (f,&s)==-1 || (df=open(f,O_RDONLY))==-1)
        return -1;
    if (cont==-1)   /* si pasamos -1 como bytes a leer lo leemos entero*/
        cont=s.st_size;
    if ((n=read(df,p,cont))==-1){
        aux=errno;
        close(df);
        errno=aux;
        return -1;
    }
    close (df);
    return n;
}

ssize_t WriteFile (char *f, void *p, size_t cont,int overwrite){
    ssize_t  n;
    int df,aux, flags=O_CREAT | O_EXCL | O_WRONLY;
    if (overwrite)
        flags=O_CREAT | O_WRONLY | O_TRUNC;
    if ((df=open(f,flags,0777))==-1) {
        printf("Cannot open the file\n");
        return -1;
    }
    if ((n=write(df,p,cont))==-1){
        aux=errno;
        close(df);
        errno=aux;
        return -1;
    }
    close (df);
    return n;
}

void do_I_O_read (char *tr[]) {
    void *p;
    size_t cont=-1;
    ssize_t n;
    if (tr[0]==NULL || tr[1]==NULL){
        printf ("Parameters are missing\n");
        return;
    }
    sscanf(tr[1],"%p",&p);
    if (tr[2]!=NULL)
        cont=(size_t) atoll(tr[2]);
    if ((n=ReadFile(tr[0],p,cont))==-1)
        perror ("Impossible to read the file\n");
    else
        printf ("Read %lld bytes of %s in %p\n",(long long) n,tr[0],p);
}

void do_I_O_write(char *tr[]){
    void *p ;
    size_t cont = -1;
    ssize_t n;
    int index=0;
    int overwrite=0;
    if (!strcmp(tr[0],"-o")) {
        overwrite=1;
        index=1;
    }
    if (tr[index]==NULL || tr[index+1]==NULL ){
        printf ("Not enough parameters\n");
        return;
    }
    sscanf(tr[index+1],"%p",&p);
    if (tr[index+2]!=NULL)
        cont = (size_t) atoll(tr[index+2]);
    if ((n=WriteFile(tr[index],p,cont,overwrite))==-1) {
        printf("Impossible to write file\n");
        return;
    }
    printf ("Written %lld bytes of  %s in %p\n",(long long) n,tr[index],p);
}

//Cmd_memory
void Do_pmap (void) {
    pid_t pid; /*hace el pmap (o equivalente) del proceso actual*/
    char elpid[32];
    char *argv[4]={"pmap",elpid,NULL};
    sprintf (elpid,"%d", (int) getpid());
    if ((pid=fork())==-1){
        perror ("Impossible to create process\n");
        return;
    }
    if (pid==0){
        if (execvp(argv[0],argv)==-1)
            perror("cannot execute pmap (linux, solaris)");

        argv[0]="procstat"; argv[1]="vm"; argv[2]=elpid; argv[3]=NULL;
        if (execvp(argv[0],argv)==-1)/*No hay pmap, probamos procstat FreeBSD */
            perror("cannot execute procstat (FreeBSD)");

        argv[0]="procmap",argv[1]=elpid;argv[2]=NULL;
        if (execvp(argv[0],argv)==-1)  /*probamos procmap OpenBSD*/
            perror("cannot execute procmap (OpenBSD)");

        argv[0]="vmmap"; argv[1]="-interleave"; argv[2]=elpid;argv[3]=NULL;
        if (execvp(argv[0],argv)==-1) /*probamos vmmap Mac-OS*/
            perror("cannot execute vmmap (Mac-OS)");
        exit(1);
    }
    waitpid (pid,NULL,0);
}


//Cmd_recursive

void Recursive (int n) {
    char autom[MAXSIZE];
    static char stat[MAXSIZE];
    printf ("parameter:%3d(%p) array %p, arr static %p\n",n,&n,autom,stat);
    if (n>0)
        Recursive(n-1);
}


//Cmd_memdump

void showMem(char address[], size_t size){
    int i=0,j;
    if(size == MAXSIZE) size = 25 ;//if size is not a parameter
    int spaces;
    if(size < 25) spaces = size;
    else spaces = 25; //25 characters per line
    while(i<size){
        for (j = 0; j < spaces; j++) {
            if (isprint(address[j + i])) {
                printf("   %c", (address[j + i])); // This first time we print the chars
            } else printf("   ");
        }
        printf("\n");
        for (j = 0; j < spaces; j++) {
            if (isprint(address[j + i])) {
                printf("%3X", (address[j + i])); // This time we printed in hexadecimal base
            } else printf(" 0");
        }
        printf("\n");
        i+=25;
    }
    printf("\n");
    return;
}


//Cmd_memfill

void FillMemory (void *p, size_t cont, unsigned char byte){
    unsigned char *arr=(unsigned char *) p;
    size_t i;
    for (i=0; i<cont;i++)
        arr[i]=byte;
}

//Environment
void ShowEnv (char **enviroment, char * enviromentName)
{
    int i=0;
    while (enviroment[i]!=NULL) {
        printf ("%p->%s[%d]=(%p) %s\n", &enviroment[i],
                enviromentName, i, enviroment[i], enviroment[i]);
        i++;
    }
}

int BuscarVariable (char * var, char *e[])  /*busca una variable en el entorno que se le pasa como parámetro*/
{
    int pos=0;
    char aux[MAXSIZE];

    strcpy (aux,var);
    strcat (aux,"=");

    while (e[pos]!=NULL)
        if (!strncmp(e[pos],aux,strlen(aux)))
            return (pos);
        else
            pos++;
    errno=ENOENT;   /*no hay tal variable*/
    return(-1);
}


int CambiarVariable(char * var, char * valor, char *e[]) /*cambia una variable en el entorno que se le pasa como parámetro*/
{                                                        /*lo hace directamente, no usa putenv*/
    int pos;
    char *aux;

    if ((pos=BuscarVariable(var,e))==-1)
        return(-1);

    if ((aux=(char *)malloc(strlen(var)+strlen(valor)+2))==NULL)
        return -1;
    strcpy(aux,var);
    strcat(aux,"=");
    strcat(aux,valor);
    e[pos]=aux;
    return (pos);
}


char* getUid(uid_t uid) {
    struct passwd *pw= getpwuid(uid);
    char *string;
    string=pw->pw_name==NULL?"UNKNOWN":pw->pw_name;
    return string;
}

void insertJobNode(tList *jobList, pid_t pid, char executable[], int priority,uid_t uid){
    char act_time[MAX];
    struct jobData *node=malloc(sizeof(jobData));
    if (node == NULL){
        printf("Cannot allocate memory with malloc\n");
        return;
    }
    time_t date;
    time(&date);
    strcpy(node->command,executable);
    strftime(act_time,MAX,"%Y/%m/%d %H:%M:%S",localtime(&date));
    strcpy(node->date,act_time);
    node->pid = pid;
    node->priority= priority;
    node->uid = uid;
    insertItem(node,jobList);
    return;
}

/*las siguientes funciones nos permiten obtener el nombre de una senal a partir
del número y viceversa */
static SEN sigstrnum[]={
        {"HUP", SIGHUP},
        {"INT", SIGINT},
        {"QUIT", SIGQUIT},
        {"ILL", SIGILL},
        {"TRAP", SIGTRAP},
        {"ABRT", SIGABRT},
        {"IOT", SIGIOT},
        {"BUS", SIGBUS},
        {"FPE", SIGFPE},
        {"KILL", SIGKILL},
        {"USR1", SIGUSR1},
        {"SEGV", SIGSEGV},
        {"USR2", SIGUSR2},
        {"PIPE", SIGPIPE},
        {"ALRM", SIGALRM},
        {"TERM", SIGTERM},
        {"CHLD", SIGCHLD},
        {"CONT", SIGCONT},
        {"STOP", SIGSTOP},
        {"TSTP", SIGTSTP},
        {"TTIN", SIGTTIN},
        {"TTOU", SIGTTOU},
        {"URG", SIGURG},
        {"XCPU", SIGXCPU},
        {"XFSZ", SIGXFSZ},
        {"VTALRM", SIGVTALRM},
        {"PROF", SIGPROF},
        {"WINCH", SIGWINCH},
        {"IO", SIGIO},
        {"SYS", SIGSYS},
/*senales que no hay en todas partes*/
#ifdef SIGPOLL
        {"POLL", SIGPOLL},
#endif
#ifdef SIGPWR
        {"PWR", SIGPWR},
#endif
#ifdef SIGEMT
        {"EMT", SIGEMT},
#endif
#ifdef SIGINFO
        {"INFO", SIGINFO},
#endif
#ifdef SIGSTKFLT
        {"STKFLT", SIGSTKFLT},
#endif
#ifdef SIGCLD
        {"CLD", SIGCLD},
#endif
#ifdef SIGLOST
        {"LOST", SIGLOST},
#endif
#ifdef SIGCANCEL
        {"CANCEL", SIGCANCEL},
#endif
#ifdef SIGTHAW
        {"THAW", SIGTHAW},
#endif
#ifdef SIGFREEZE
        {"FREEZE", SIGFREEZE},
#endif
#ifdef SIGLWP
        {"LWP", SIGLWP},
#endif
#ifdef SIGWAITING
        {"WAITING", SIGWAITING},
#endif
        {NULL,-1},
};

int ValorSenal(char * sen)  /*devuelve el numero de senial a partir del nombre*/
{
    int i;
    for (i=0; sigstrnum[i].name!=NULL; i++)
        if (!strcmp(sen, sigstrnum[i].name))
            return sigstrnum[i].signal;
    return -1;
}


char *NombreSenal(int sen)  /*devuelve el nombre senal a partir de la senal*/
{			/* para sitios donde no hay sig2str*/
    int i;
    for (i=0; sigstrnum[i].name!=NULL; i++)
        if (sen==sigstrnum[i].signal)
            return sigstrnum[i].name;
    return ("SIGUNKNOWN");
}

void updateNode(jobData* node) {
    int value;
    pid_t pid;
    //Status
    if((pid=waitpid(node->pid, &value, WNOHANG |WUNTRACED|WCONTINUED)) == node->pid){
        if(WIFEXITED(value)!=0){  // if ended normal
            node->value = WEXITSTATUS(value);
            strcpy(node->status, "FINISHED");

        }else if(WIFSIGNALED(value)){ // if ended by signal
            node->value = WTERMSIG(value);
            strcpy(node->status, "SIGNALED");

        }else if(WIFSTOPPED(value)){ // if stopped by signal
            node->value = WSTOPSIG(value);
            strcpy(node->status, "STOPPED");

        }
    }
    if(pid==0) { //if is still active
        node->value=0;
        strcpy(node->status, "ACTIVE");
    }
    //Priority
    node->priority= getpriority(PRIO_PROCESS,node->pid);
}

void printJobNode(jobData *node){
    updateNode(node);

    if (strcmp(node->status,"SIGNALED")) {
        printf("%6d %s p=%d   %s %10s (%03d)  %s\n", node->pid, getUid(node->uid), node->priority, node->date,
               node->status,
               node->value, node->command);
    }
    else {
        printf("%6d %s p=%d   %s %10s (%s)  %s\n", node->pid, getUid(node->uid), node->priority, node->date,
               node->status,
               NombreSenal(node->value), node->command);
    }
}

tPosL getPid(arguments *arg, pid_t pid){
    tPosL p = first(arg->jobs);
    while(p != NULL){
        jobData *node = getItem(p,arg->jobs);
        if(node->pid == pid) return p;
        p = p->next;
    }
    return NULL;
}


/*para sistemas donde no hay (o no queremos usar) execvpe*/
char * Ejecutable (char *s)
{
    char path[MAX];
    static char aux2[MAX];
    struct stat st;
    char *p;
    if (s==NULL || (p=getenv("PATH"))==NULL)
        return s;
    if (s[0]=='/' || !strncmp (s,"./",2) || !strncmp (s,"../",3))
        return s;       /*is an absolute pathname*/
    strncpy (path, p, MAX);
    for (p=strtok(path,":"); p!=NULL; p=strtok(NULL,":")){
        sprintf (aux2,"%s/%s",p,s);
        if (lstat(aux2,&st)!=-1)
            return aux2;
    }
    return s;
}

int OurExecvpe(char *file, char *const argv[], char *const envp[])
{
    return (execve(Ejecutable(file),argv, envp));
}


