#ifndef _H_PNG_RANDOM_CONFIG
#define _H_PNG_RANDOM_CONFIG

// chunks
enum {
    IHDR,
    IDAT,
    IEND,
    BASE_CHUNKS_COUNT,


};

enum {
    MIN_WIDTH = 1,
    MIN_HEIGHT = 1,
    MAX_WIDTH = 1000,
    MAX_HEIGHT = 1000,
};

#endif
