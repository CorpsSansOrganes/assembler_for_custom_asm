#ifndef __SH_ED_VECTOR__
#define __SH_ED_VECTOR__

/*
 * @brief An impelementation of a dynamic array.
 *
 *      Vector is a continuous block of uniform-sized elements, which grows on demand.
 */

#include <stddef.h> /* size_t */
#include "utils.h" /* result_t */

typedef struct vector vector_t;         

/*
 * @brief Creates a new empty vector.
 *
 * @param initial_capcity - How many elements the vector can initial have.
 *        element_size - The size (in bytes) of each element.
 *
 * @return Upon success, returns a pointer to the newly created vector.
 *         Upon failure, return NULL.
 */

vector_t *CreateVector(size_t initial_capacity, size_t element_size);

/*
 * @brief Deallocates the memory of a vector.
 * 
 * @param vector - The vector we wish to deallocate.
 */

void DestroyVector(vector_t *vector);

/*
 * @brief Appends a given element to the end of the vector.
 *
 * @param vector - The vector into which we wish to append an element.
 *
 *        value - Pointer to the element we wish to append. 
 *        NOTE: size of value must be the same as the vector's element size!
 *
 * @return SUCCESS if the element was added correctly, or MEM_ALLOCATION_ERROR
 *         upon a failure.
 */

result_t AppendVector(vector_t *vector, const void *value);

/*
 * @brief Removes the last element in the vector.
 *
 * @param vector - The vector from which we wish to remove an element.
 *        NOTE: If vector is empty, behaviour is undefined!
 */

void RemoveLastVector(vector_t *vector);

/*
 * @brief Returns element index from vector (like vector[index]).
 *
 * @param vector - The vector whose element we wish to get.
 *        NOTE: If index > vector's size, behaviour is undefined!
 *
 * @return The element in vector at index.
 */

void *GetElementVector(vector_t *vector, size_t index);

/*
 * @brief Checks if a vector is empty.
 *
 * @param vector - The vector which we wish to check.
 *
 * @return TRUE if vector is empty, FALSE otherwise.
 */

bool_t IsEmptyVector(const vector_t *vector);                    

/*
 * @brief Return the current capacity of a vector.
 *
 * @param vector - The vector which we wish to check.
 *
 * @return Current capacity of a vector.
 */

size_t GetCapacityVector(const vector_t *vector);

/*
 * @brief Return the current number of element in a vector.
 *
 * @param vector - The vector which we wish to check.
 *
 * @return Current size of a vector.
 */

size_t GetSizeVector(const vector_t *vector);

/*
 * @brief Increase the capacity of a vector explicitly.
 *
 * @param vector - The vector whose capacity we wish to increase.
 *        new_capacity - The new capacity of the vector.
 *
 *        NOTE: if the new capacity is smaller than the current capacity
 *        nothing happens.
 *
 * @return SUCCESS if all goes well, MEM_ALLOCATION_ERROR otherwise.
 */

result_t ReserveVector(vector_t *vector, size_t new_capacity);

/*
 * @brief Set capacity to the current size of the vector.
 *
 * @param vector - The vector whose capacity we wish to set to the current 
 *        size.
 *
 * @return SUCCESS if all goes well, MEM_ALLOCATION_ERROR otherwise.
 */

result_t ShrinkVector(vector_t *vector);

#endif /* __SH_ED_VECTOR__ */
