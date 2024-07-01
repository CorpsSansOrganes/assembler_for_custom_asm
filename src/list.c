#include "list.h"

struct node {
  void *value; // pointer to symbol
  node_t *next;
};

struct list {
  node_t *head;
  node_t *tail;
};
