#include "list.h"
#include "stdlib"
struct node {
  void *value; // pointer to symbol
  node_t *next;
};

struct list {
  node_t *head;
  node_t *tail;
};

list_t *CreateList(void) {
    list_t *newList = (list_t *)malloc(sizeof(list_t));
    if (NULL == newList) {
        return NULL;
    }

    newList->head = NULL;
    newList->tail = NULL;

    return newList;
}
node_t *AddNode(list_t *list, void *value){
   node_t *newNode =  (node_t *)malloc(sizeof(node_t));
   if ( NULL == newNode) {
        return NULL;
   }
   newNode->value = value;
   list->tail->next = newNode;
   list->tail = newNode;
   newNode->next = NULL;
   return newNode;
}

node_t* Find(list_t *list, cmp_func func, void *key){
   node_t *pointer = list->head;

   while (NULL != pointer ){
	if (func (pointer->value, key)){
	   return pointer;
	}
	pointer = pointer->next;
   }
   return NULL;
}
void DestroyList(list_t *list) {
    if (NULL == list) {
        return; 
    }

    node_t *current = list->head;
    node_t *next_node;


    while (NULL != current ) {
        next_node = current->next;
        free(current); 
        current = next_node; 
    }

    free(list);
}
node_t *GetNext(node_t *node){
    if (NULL == node){
	return NULL;
    }	    
    return node->next;
}
void *GetValue(node_t *node) {
    if (NULL == node){
	return NULL;
    }	    
    return node->value;
}