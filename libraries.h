#define MAX 1024
#define MAXSIZE 2048
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/utsname.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "list.h"
#include <dirent.h>
#include <errno.h>
#include <pwd.h>
#include <grp.h>
#include <dirent.h>
#include <ctype.h>
#include <libgen.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <ctype.h>
#include <sys/resource.h>

extern char **environ;

//Auxiliary functions
//Cmd_create

void createDirectory(char* tr[]);
void createFile(char* tr[]);


//Cmd_stat
typedef struct arguments {
    tList historial;
    tList malloc;
    tList mmap;
    tList shared;
    tList jobs;
    int countCommand;
    char ***main_env;

}arguments;

struct parameters {
    int hid;
    int reca;
    int recb;
    int lng;
    int acc;
    int link;
};

typedef struct CmdData {
    char cmd[MAX];
}CmdData;

typedef struct mallocData {
    void* address;
    ssize_t size;
    char date[MAX];
    char type[10];
}mallocData;

typedef struct mmapData {
    void * address;
    ssize_t size;
    char date[MAX];
    char fname[MAX];
    int descriptor;
    char type[10];
}mmapData;

typedef struct sharedData{
    void* address;
    ssize_t size;
    char date[MAX];
    key_t key;
    char type[10];
}sharedData;

//Jobs data
typedef struct jobData {
    pid_t pid;
    char date[MAX];
    char status[MAX];
    char command[MAX];
    uid_t uid;
    int priority;
    int value;
}jobData;

typedef struct SEN {
    char* name;
    int signal;
} SEN;

// end
bool end(char* tr[]);
//time
void actual_time(char act_time[]);
//char memBlock[MAX];

char LetraTF (mode_t m);
int printStats(char* file,struct parameters param);

//Cmd_list
bool isHidden(char* name);
void printDir(DIR * dir);
void printHid(DIR * dir);
int listRec(char* tr,struct parameters param);
int displayList(char* tr,struct parameters param);

//Cmd_deltree
int deleteRec(char* entry);
void Recursive(int n);
void ShowMem();

//Cmd_comando
int validIndex(char* index,tList L);

//Cmd_mem
void FillMemory (void *p, size_t cont, unsigned char byte);

//Cmd_allocate
void printMallocList(arguments arg);
void printSharedList(arguments arg);
void printMmapList(arguments arg);
void insertMallocNode(tList *mallocList, void* address, ssize_t blockSize);
void insertSharedNode(tList *sharedList, void* address, ssize_t blockSize, key_t key);
void insertMapNode(tList *mapList, void* address,ssize_t blockSize,char filename[], int descriptor);
void * ObtainMemoryShmget (key_t clave, size_t tam,arguments* arg);
void do_AllocateCreateshared (char *tr[], arguments* arg);
void * MapFile (char * fichero, int protection);
void do_AllocateMmap(char *tr[], arguments *arg);
void printAllLists(arguments *arg);

//Cmd_deallocate
void do_DeallocateDelkey (char *args[]);
void do_deallocateAddr(char *tr[],arguments *arg);
tPosL findKey(key_t key, arguments *arg);
tPosL findSize(size_t size, arguments *arg);
tPosL findFile(char filename[], arguments *arg);
tPosL findAddress(void* address, tList list);

//Cmd_I_O
void do_I_O_write(char *tr[]);
void do_I_O_read(char *tr[]);

//Cmd_memory
void Do_pmap (void);

//Cmd_memdump
void showMem(char address[], size_t size);

void freeMalloc(void* p);
void freeMmap(void *p);

//Cmd_showvar
int BuscarVariable (char * var, char *e[]);
int CambiarVariable(char * var, char * valor, char *e[]);

//Enviroment
void ShowEnv (char **enviroment, char * enviromentName);

//Ejecutable
char * Ejecutable (char *s);
int OurExecvpe(char *file, char *const argv[], char *const envp[]);

//Job
void updateNode(jobData* node);
void printJobNode(jobData *node);
void insertJobNode(tList *jobList, pid_t pid, char executable[], int priority,uid_t uid);
tPosL getPid(arguments *arg, pid_t pid);
char* getUid(uid_t uid);

//Signals
int ValorSenal(char * sen);
char *NombreSenal(int sen);