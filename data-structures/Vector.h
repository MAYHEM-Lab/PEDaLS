#ifndef VECTOR_H
#define VECTOR_H

struct Vector{
	void *data;
	int element_size;
	int size;
	int capacity;
};

typedef struct Vector Vector;

/**
 * initializes vector
 *
 * @param element_size size of each element
 * @param capacity capacity of the vector
 */
Vector *Vector_init(int element_size, int capacity);

/**
 * doubles the capacity of the vector
 */
void Vector_resize(Vector *v);

/**
 * pushes element at the end of the vector
 */
void Vector_push(Vector *v, void *element);

/**
 * inserts element at the given position
 */
void Vector_insert(Vector *v, void *element, int pos);

/**
 * retrieves element from the given position
 */
void *Vector_get(Vector *v, int pos);

/**
 * appends content of src[start:end] to dst
 */
void Vector_append(Vector *dst, Vector *src, int start, int end);

/**
 * destructor
 */
void Vector_destroy(Vector *v);

#endif
