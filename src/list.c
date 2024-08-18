/* list.c
 *
 * This module implements a list data structure along with its associated utilities.
 */



#include "list.h"
#include <stdlib.h> /* malloc, free */

struct node {
  void *value; 
  node_t *next;
};

struct list {
  node_t *head;
  node_t *tail;
};

static node_t *CreateDummy(void);
static node_t *CreateNode(node_t *next, void *value);

list_t *CreateList(void) {
  list_t *new_list = (list_t *)malloc(sizeof(list_t));
  node_t *dummy_node = NULL;

  if (NULL == new_list) {
    return NULL;
  }

  dummy_node = CreateDummy();
  if (NULL == dummy_node) {
    free(new_list);
    return NULL;
  }

  new_list->head = dummy_node;
  new_list->tail = dummy_node;

  return new_list;
}

node_t *AddNode(list_t *list, void *value) {
  node_t *new_node = CreateNode(NULL, value);

  if (NULL == new_node) {
    return NULL;
  }

  list->tail->next = new_node;
  list->tail = new_node;
  return new_node;
}

node_t *Find(list_t *list, cmp_func func, void *key) {
  node_t *cur_node = GetHead(list);

  while (NULL != cur_node) {
    if (1 == func(cur_node->value, key)) {
      return cur_node;
    }
    cur_node = cur_node->next;
  }

  return NULL;
}

void DestroyList(list_t *list) {
  node_t *cur_node = list->head;
  node_t *next_node = NULL;

  while (NULL != cur_node) {
    next_node = cur_node->next;    
    free(cur_node);
    cur_node = next_node;
  }

  free(list);
}

node_t *GetHead(list_t *list) {
  return (list->head)->next; /* list->head is dummy */
}

node_t *GetNext(node_t *node) {
  return node->next;
}

void *GetValue(node_t *node) {
  return node->value;
}

static node_t *CreateDummy(void) {
  return CreateNode(NULL, (void *)(0xDEADBEEF));
}

static node_t *CreateNode(node_t *next, void *value) {
  node_t *node = (node_t *)malloc(sizeof(node_t));
  if (NULL == node) {
    return NULL;
  }

  node->next = next;
  node->value = value;
  return node;
}
