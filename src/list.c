#include "list.h"
#include <stdlib.h> /* malloc, free */

struct node {
  void *value; // pointer to symbol
  node_t *next;
};

struct list {
  node_t *head;
  node_t *tail;
};

list_t *CreateList(void) {
    list_t *new_list = (list_t *)malloc(sizeof(list_t));
    if (NULL == new_list) {
        return NULL;
    }

    new_list->head = NULL;
    new_list->tail = NULL;

    return new_list;
}

node_t *AddNode(list_t *list, void *value){
   node_t *new_node =  (node_t *)malloc(sizeof(node_t));

   if (NULL == new_node) {
        return NULL;
   }

   new_node->value = value;
   list->tail->next = new_node;
   list->tail = new_node;
   new_node->next = NULL;
   return new_node;
}

node_t* Find(list_t *list, cmp_func func, void *key){
   node_t *pointer = list->head;

   while (NULL != pointer ) {
    if (func (pointer->value, key)) {
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

node_t *GetNext(node_t *node) {
  if (NULL == node) {
    return NULL;
  }	    
  return node->next;
}

void *GetValue(node_t *node) {
  if (NULL == node) {
    return NULL;
  }	    
  return node->value;
}
