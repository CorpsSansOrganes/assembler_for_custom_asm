#ifndef __SH_ED_VECTOR__
#define __SH_ED_VECTOR__

#include <stddef.h> /* size_t */
#include "utils.h" /* result_t */

typedef struct vector vector_t;         

vector_t *VectorCreate(size_t initial_capacity, size_t element_size);
void VectorDestory(vector_t *vector);
result_t VectorAppend(vector_t *vector, const void *value);
void VectorRemoveLast(vector_t *vector);
void *VectorGet(vector_t *vector, size_t index);

bool_t VectorIsEmpty(const vector_t *vector);                    
size_t GetCapacityVector(const vector_t *vector);
size_t GetSizeVector(const vector_t *vector);
result_t ReserveVector(vector_t *vector, size_t size);
result_t ShrinkVector(vector_t *vector);

#endif /* __SH_ED_VECTOR__ */
