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

typedef struct AFL_Vector {
    void *data;
    size_t len;
    size_t size;
    size_t max_size;
} AFL_Vector;

int is_valid_afl_vector(AFL_Vector *vector);
AFL_Vector *malloc_afl_vector(void);
int init_afl_vector(AFL_Vector *vector, void *data, size_t len, size_t size, size_t max_size);
void deinit_afl_vector(AFL_Vector *vector, int flag);
void print_afl_vector_info(AFL_Vector *vector);
int write_to_afl_vector(AFL_Vector *vector, void *buf, size_t buf_size);

#endif
