#ifndef _H_PNG_PROCESSING
#define _H_PNG_PROCESSING

#include <png.h>

typedef struct png_processing_t {
    chank_t chunks[CHUNK_COUNT];
    png_structp png;
    png_infop info;
} png_processing_t;


#endif
