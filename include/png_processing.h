#ifndef _H_PNG_PROCESSING
#define _H_PNG_PROCESSING

#include <png.h>


typedef struct {
    int is_valid;
} PNGChunk_t;

typedef struct png_processing_t {
    PNGChunk_t chunks[10];
    png_structp png;
    png_infop info;
} png_processing_t;


#endif
