#ifndef PRODUCT_LIST_H
#define PRODUCT_LIST_H

/* Write your code here... */
#include <stdbool.h>
/* Definition of variables*/
typedef struct tNode* tPosL;
typedef struct tNode {
    void* data;
    tPosL next;
}tNode;
typedef tPosL tList;
/*Functions*/

//It creates an empty list with a head node
void createEmptyList(tList* L); 
//A function to insert the items at the end of the list since we will be using them for the historic function
bool insertItem(void* d, tList *L);
//This would delete the element in position pos.
void deleteItem(tList* L,tPosL pos);
//A function that would return the length of the list to ease the way in the rest of the functions
int listLength(tList L);
//This would print all the elements of the list until a desired index
void printList(tList L, long int index);
//This function will return a bool telling us wheter is empty or not
bool isEmptyList(tList L);
//This would return the item of a determined position
void* getItem(tPosL p, tList L);
//This function will return the first element
tPosL first(tList L) ;
//This one the last
tPosL last(tList L);
//And this one the next element after a position p
tPosL next(tPosL p, tList L);
void clearList(tList* L);
void deallocateList(tList *L, void (*freeData)(void *));
void deleteItem2(tList* L,tPosL p,void (*freeData)(void *));
#endif