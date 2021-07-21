#include <stdlib.h>
#include <string.h>

#include "Vector.h"

Vector *Vector_init(int element_size, int capacity){

	Vector *v;

	v = (Vector *)malloc(sizeof(Vector));

	v->element_size = element_size;
	v->capacity = (capacity > 0) ? capacity : 1;
	v->size = 0;
	v->data = malloc(v->element_size * v->capacity);

	return v;

}

void Vector_resize(Vector *v){
	v->capacity *= 2;
	v->data = realloc(v->data, v->element_size * v->capacity);
}

void Vector_push(Vector *v, void *element){
	if(v->size == v->capacity) Vector_resize(v);
	memcpy(v->data + (v->size * v->element_size), element, v->element_size);
	v->size += 1;
}

void Vector_insert(Vector *v, void *element, int pos){
	if(pos >= v->size) return;
	memcpy(v->data + (pos * v->element_size), element, v->element_size);
}

void *Vector_get(Vector *v, int pos){
	if(pos >= v->size) return NULL;
	return v->data + (pos * v->element_size);
}

void Vector_append(Vector *dst, Vector *src, int start, int end){
	int i;
	if(dst == NULL || src == NULL) return;
	
	for(i = start; i <= end; ++i){
		Vector_push(dst, src->data + (i * src->element_size));
	}
}

void Vector_destroy(Vector *v){
	free(v->data);
	free(v);
}
