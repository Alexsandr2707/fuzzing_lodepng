#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "vector.h"

int is_valid_vector(Vector *vector);
Vector *malloc_vector(void);
int init_vector(Vector *vector, void *data, size_t len, size_t size, size_t max_size);
void deinit_vector(Vector *vector, int flag);
void print_vector_info(Vector *vector);
int write_to_vector(Vector *vector, void *buf, size_t buf_size);

int is_valid_vector(Vector *vector) {
    if ((vector == NULL) || 
        (vector->len > vector->size) || 
        (vector->size > vector->max_size) ||
        (vector->data == NULL && vector->len > 0))
        return 0;
    else 
        return 1;
}

Vector *malloc_vector(void) {
    Vector *vector = calloc(1, sizeof(*vector));
    return vector;
}

int init_vector(Vector *vector, void *data, size_t len, size_t size, size_t max_size) {
    if (max_size == 0)
        max_size = VECTOR_DEF_MAX_SIZE;

    if ((vector == NULL) || 
        (len > size) ||
        (data == NULL && len > 0) ||
        (size > max_size) || 
        (max_size > VECTOR_HARD_MAX_SIZE)) 
            return -1;

    void *new_data = NULL;;
    if (data == NULL) {
        new_data = calloc(size, sizeof(*new_data));
        if (new_data == NULL)
            return -1;
    } else {
        new_data = data;
    }

    vector->data = new_data;
    vector->len = len;
    vector->size = size;
    vector->max_size = max_size;
    
    return 0;
}

void deinit_vector(Vector *vector, int flag) {
    if (vector == NULL)
        return;

    if (vector->data) {
        free(vector->data);
    }

    if (flag == VECTOR_DYNAMIC) {
        free(vector);
    }
}

void print_vector_info(Vector *vector) {
    fprintf(stdout, "----- PRINT VECTOR -----\n");
    if (vector == NULL) {
        fprintf(stdout, "VECTOR IS NULL\n");
        return;
    }

    if (vector->data == NULL)
        fprintf(stdout, "  data: NULL\n"); 
    else {
        fprintf(stdout, "  data: ... - not NULL\n");
    }

    fprintf(stdout, "  len: %lu\n", vector->len);
    fprintf(stdout, "  size: %lu\n", vector->size);
    fprintf(stdout, "  max_size: %lu\n", vector->max_size);

    fprintf(stdout, "------------------------\n");
}

void clean_vector(Vector *vector) {
    if (vector) {
        vector->len = 0;
    }
}

    
int write_to_vector(Vector *vector, void *buf, size_t buf_size) {
    if (!is_valid_vector(vector) || buf == NULL) {
        return -1;
    }

    if (vector->len + buf_size > vector->max_size) {
        return -1;
    }

    size_t new_size = vector->size * 2 + buf_size;
    if (new_size > vector->max_size) {
        new_size = vector->max_size;
    }

    void *new_data = realloc(vector->data, new_size);
    if (new_data == NULL)
        return -1;

    vector->data = new_data;
    vector->size = new_size;
    
    memcpy(vector->data + vector->len, buf, buf_size);
    vector->len += buf_size;

    return 0;
}

