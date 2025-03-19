#ifndef _H_VECTOR
#define _H_VECTOR

#include <stdint.h>

enum {
    AFL_VECTOR_DEF_MAX_SIZE = 1000000,
    AFL_VECTOR_HARD_MAX_SIZE = 100000000,
};

enum {
    DYNAMIC,
    STATIC,
};

typedef struct AFLVector {
    void *data;
    size_t len;
    size_t size;
    size_t max_size;
} AFLVector;

int is_valid_afl_vector(AFLVector *vector);
AFLVector *malloc_afl_vector(void);
int init_afl_vector(AFLVector *vector, void *data, size_t len, size_t size, size_t max_size);
void deinit_afl_vector(AFLVector *vector, int flag);
void print_afl_vector_info(AFLVector *vector);
AFLVector *write_to_afl_vector(AFLVector *vector, void *buf, size_t buf_size);

#endif
