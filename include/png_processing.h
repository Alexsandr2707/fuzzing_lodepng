#ifndef _H_PNG_PROCESSING
#define _H_PNG_PROCESSING

#include <png.h>
#include "vector.h"
#include "afl_vector.h"

// chunks
enum {
    PNG_CHUNK_IHDR, // Png header
    PNG_CHUNK_IDAT, // Data 
    PNG_CHUNK_IEND, // End chunk
    BASE_CHUNK_COUNT,

    PNG_CHUNK_NOT_CONST_INFO = BASE_CHUNK_COUNT,

    PNG_CHUNK_bKGD = PNG_CHUNK_NOT_CONST_INFO, // Background color - IMPLEMENTED
    PNG_CHUNK_cHRM,  // Primary chromaticities and white point - IMPLEMENTED
    PNG_CHUNK_gAMA,  // Image gamma - IMPLEMENTED
    PNG_CHUNK_hIST,  // Image histogram - NOT IMPLEMENTED
    PNG_CHUNK_iTXt,  // International textual data - IMPLEMENTED
    PNG_CHUNK_pHYs,  // Physical pixel dimensions - IMPLEMENTED
    PNG_CHUNK_sBIT,  // Significant bits - IMPLEMENTED
    PNG_CHUNK_sPLT,  // Suggested palette - IMPLEMENTED
    PNG_CHUNK_PLTE,  // Suggested palette - IMPLEMENTED
    PNG_CHUNK_sRGB,  // Standard RGB color space - IMPLEMENTED
    PNG_CHUNK_sTER,  // Stereo image indicator - NOT IMPLEMENTED
    PNG_CHUNK_tEXt,  // Textual data - IMPLEMENTED
    PNG_CHUNK_tIME,  // Image last-modification time - IMPLEMENTED
    PNG_CHUNK_tRNS,  // Transparency - IMPLEMENTED
    PNG_CHUNK_zTXt,   // Compressed textual data - IMPLEMENTED

    PNG_CHUNK_cSTM,   // Custom chunk - IMPLEMENTED
    

    PNG_CHUNK_CONST_INFO = PNG_CHUNK_cSTM,

    PNG_CHUNK_iCCP,  // Embedded ICC profile - IMPLEMENTED

    CHUNK_COUNT,
};

enum {
    UNDEFINED,
    NOT_REQUIRED,
    IS_REQUIRED,
};

enum {
    NOT_VALID,
    IS_VALID,
};

enum {
    NOT_CLONED,
    IS_CLONED,
};


typedef struct {
    int required;
    int valid;
    int cloned;

    uint8_t *file_pointer; // pointer to place of chunk in memory
    size_t file_size; // full size of chunk in memory

    Vector info; 
} PNGChunk_t;

typedef struct png_processing_t {
    PNGChunk_t chunks[CHUNK_COUNT];
    png_structp png;
    png_infop info;
    AFLVector png_out;
} png_processing_t;

png_processing_t *create_png_processing(void);
void destroy_png_processing(png_processing_t *png_prc);
int reset_png_processing(png_processing_t *png_prc);
int reset_chunk(PNGChunk_t *chunk, int type);
int png_get_chunk_type(const uint8_t *name);
const char *png_get_chunk_name(int type);

#endif
