#include "libraries.h"

bool createNode(tPosL* p){
    *p= malloc(sizeof(struct tNode)); 
    return *p!=(NULL);}

void createEmptyList (tList* L){ //
    createNode(L);
    (*L)->next=NULL;
}

tPosL last(tList L){
    tPosL p;
    for (p=L;p->next!=NULL;p=p->next);
    return p;
}

tPosL first(tList L){
    return L->next;}

bool isEmptyList(tList L){
    return(L->next==NULL);}

tPosL next (tPosL p, tList L){
    return(p->next);}

bool insertItem(void* d,tList *L) {
    tPosL p, j;
    if (!createNode(&p)) return false;
    p->data = d;
    p->next = NULL;
    j = *L;
    while (j->next != NULL) {
        j = j->next;
    }
    j->next = p;
    return true;
}

void deleteItem(tList* L,tPosL pos) {
    tPosL q;
    if (pos==(*L)->next) {
        (*L)->next = pos->next;
    }
    else if (pos->next==NULL) {
        for (q=(*L)->next;q->next != pos;q=q->next);
        q->next=NULL;
    }
    else {
        for (q=(*L)->next;q->next!=pos;q=q->next);
        q->next=pos->next;
    }
}

void deleteItem2(tList* L,tPosL p,void (*freeData)(void *)) {
    tPosL q=(*L)->next;
    if(q == p){
        (*L)->next = (*L)->next->next;
        freeData(p->data);
        free(p);
    }else {
        while (q->next != p) { //aux pasa a ser el nodo anterior al que queremos eliminar
            q = q->next;
        }
        q->next = p->next;
        freeData(p->data);
        free(p);
    }
}



void* getItem (tPosL p, tList L){
    return (p->data);}

int listLength(tList L) {  
    tPosL p;
    int i=0;
    for (p=first(L);(p!=last(L));p=next(p,L)) i++; 
    return i;
}

void deallocateList(tList *L, void (*freeData)(void *)) {
    struct tNode *node,*tmp;
    node=(*L)->next;

    while (node !=NULL) {
        tmp=node;
        freeData(node->data);
        node=node->next;
        free(tmp);
    }
    free(*L);
}

void clearList(tList* L) {
    struct tNode *node,*tmp;
    node=(*L)->next;
    while (node!=NULL) {
        tmp=node;
        free(node->data);
        node=node->next;
        free(tmp);
    }
    (*L)->next=NULL;
}