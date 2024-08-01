#ifndef __LIST__
#define __LIST__

/*
 * @brief An impelementation of a singly-linked list.
 */

typedef struct node node_t;
typedef struct list list_t;
typedef int (*cmp_func)(void *, void *);

/*
 * @brief Creates a new empty list
 * @return Upon success, return a pointer to the newly created list.
 *         Upon failure, return NULL.
 */
list_t *CreateList(void);


/*
 * @brief Creates a new node, and adds it to the end of the list.
 * @param list - the list into which we wish to insert the new node.
 *        value - the value of the newly created node.
 * @return Upon success, returns a pointer to the newly created node.
 *         Upon failure, returns NULL.
 */
node_t *AddNode(list_t *list, void *value); 


/*
 * @brief Searches the list for a specific node.
 * @param list - the list in which we search.
 *        func - a function which return 1 upon a match to key and 0 upon a mismatch key.
 *        key - the key used for the search.
 * @return Upon success, returns a pointer to the node which matches the key.
 *         Upon failure, returns NULL.
 */
node_t* Find(list_t *list, cmp_func func, void *key);

/*
 * @brief Deallocates the memory of a list.
 * @param list - the list we wish to deallocate
 */
void DestoryList(list_t *list);

/*
 * @brief Get the node which comes after a given node.
 * @param node - the node from which we get the following node.
 * @return If a following node exists, a pointer to it is returned.
 *         Otherwise, NULL is returned.
 */
node_t *GetNext(node_t *node);

/* @brief Get the value stored in a node.
 * @param node - the node whose value we wish to aquire
 * @return The value stored in the node.
 */
void *GetValue(node_t *node);

#endif /* __LIST__ */
