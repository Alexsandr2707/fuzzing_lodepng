#ifndef _H_VECTOR
#define _H_VECTOR

#include <stdint.h>

enum {
    DEF_MAX_SIZE = 1000000,
    HARD_MAX_SIZE = 100000000,
};

enum {
    DYNAMIC,
    STATIC,
};

typedef struct Vector {
    void *data;
    size_t len;
    size_t size;
    size_t max_size;
} Vector;

int is_valid_vector(Vector *vector);
Vector *malloc_vector(void);
int init_vector(Vector *vector, void *data, size_t len, size_t size, size_t max_size);
void deinit_vector(Vector *vector, int flag);
void print_vector_info(Vector *vector);
int write_to_vector(Vector *vector, void *buf, size_t buf_size);


#endif
