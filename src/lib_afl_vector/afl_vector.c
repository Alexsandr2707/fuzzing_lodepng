#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "afl_vector.h"
#include "alloc-inl.h"

int is_valid_afl_vector(AFL_Vector *vector);
AFL_Vector *malloc_afl_vector(void);
int init_afl_vector(AFL_Vector *vector, void *data, size_t len, size_t size, size_t max_size);
void deinit_afl_vector(AFL_Vector *vector, int flag);
void print_afl_vector_info(AFL_Vector *vector);
int write_to_afl_vector(AFL_Vector *vector, void *buf, size_t buf_size);

int is_valid_afl_vector(AFL_Vector *vector) {
    if ((vector == NULL) || 
        (vector->len > vector->size) || 
        (vector->size > vector->max_size) ||
        (vector->data == NULL && vector->len > 0) ||
        (afl_alloc_bufsize(vector->data) != vector->size))
        return 0;
    else 
        return 1;
}

int init_afl_buf(void **buf) {
    if (buf == NULL) 
        return -1;

    struct afl_alloc_buf *afl_buf = calloc(1, sizeof(*afl_buf));
    if (afl_buf == NULL) 
        return -1;

    afl_buf->complete_size = sizeof(*afl_buf);
    
    *buf = afl_buf->buf;
    return 0;
}

AFL_Vector *malloc_afl_vector(void) {
    AFL_Vector *vector = calloc(1, sizeof(*vector));
    if (vector == NULL) 
        return NULL;

    if (init_afl_buf(&(vector->data)))
        return NULL;

    return vector;
}

int init_afl_vector(AFL_Vector *vector, void *data, size_t len, size_t size, size_t max_size) {
    if (max_size == 0)
        max_size = DEF_MAX_SIZE;

    if ((vector == NULL) || 
        (len > size) ||
        (data == NULL && len > 0) ||
        (data != NULL && afl_alloc_bufsize(&data) > size) ||
        (size > max_size) || 
        (max_size > HARD_MAX_SIZE)) 
            return -1;

    void *new_data = NULL;;
    if (data == NULL) {
        if (init_afl_buf(&new_data) < 0)
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

void deinit_afl_vector(AFL_Vector *vector, int flag) {
    if (vector == NULL)
        return;

    if (vector->data) {
        afl_free(vector->data);
    }

    if (flag == DYNAMIC) {
        free(vector);
    }
}

void print_afl_vector_info(AFL_Vector *vector) {
    fprintf(stdout, "----- PRINT AFL VECTOR -----\n");
    if (vector == NULL) {
        fprintf(stdout, "VECTOR IS NULL\n");
        return;
    }

    if (vector->data == NULL)
        fprintf(stdout, "data: NULL\n"); 
    else {
        fprintf(stdout, "data: ... - not NULL\n");
    }

    fprintf(stdout, "len: %lu\n", vector->len);
    fprintf(stdout, "size: %lu\n", vector->size);
    fprintf(stdout, "afl_buf_size: %lu\n", afl_alloc_bufsize(vector->data));
    fprintf(stdout, "max_size: %lu\n", vector->max_size);

    fprintf(stdout, "----------------------------\n");
}
    
int write_to_afl_vector(AFL_Vector *vector, void *buf, size_t buf_size) {
    if (!is_valid_afl_vector(vector) || buf == NULL) {
        return -1;
    }

    size_t new_size = vector->len + buf_size;
    if (new_size > vector->max_size) {
        return -1;
    }

    void *new_data = afl_realloc(&(vector->data), new_size);
    if (new_data == NULL)
        return -1;

    vector->data = new_data;
    vector->size = afl_alloc_bufsize(new_data);
    
    memcpy(vector->data + vector->len, buf, buf_size);
    vector->len += buf_size;

    return 0;
}

