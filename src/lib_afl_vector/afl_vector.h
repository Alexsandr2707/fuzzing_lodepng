#ifndef _H_AFL_VECTOR
#define _H_AFL_VECTOR

#include <stdint.h>

enum {
    AFL_VECTOR_DEF_MAX_SIZE = 1000000,
    AFL_VECTOR_HARD_MAX_SIZE = 1000000000,
};

enum {
    AFL_VECTOR_DYNAMIC,
    AFL_VECTOR_STATIC,
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
void clean_afl_vector(AFLVector *afl_vector);
void print_afl_vector_info(AFLVector *vector);
AFLVector *write_to_afl_vector(AFLVector *vector, const void *buf, size_t buf_size);

#endif
