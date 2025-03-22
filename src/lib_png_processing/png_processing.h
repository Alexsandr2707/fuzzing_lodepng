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

    PNG_CHUNK_bKGD = BASE_CHUNK_COUNT,  // Background color
    PNG_CHUNK_cHRM,  // Primary chromaticities and white point
    PNG_CHUNK_gAMA,  // Image gamma
    PNG_CHUNK_hIST,  // Image histogram
    PNG_CHUNK_iCCP,  // Embedded ICC profile
    PNG_CHUNK_iTXt,  // International textual data
    PNG_CHUNK_pHYs,  // Physical pixel dimensions
    PNG_CHUNK_sBIT,  // Significant bits
    PNG_CHUNK_sPLT,  // Suggested palette
    PNG_CHUNK_sRGB,  // Standard RGB color space
    PNG_CHUNK_sTER,  // Stereo image indicator
    PNG_CHUNK_tEXt,  // Textual data
    PNG_CHUNK_tIME,  // Image last-modification time
    PNG_CHUNK_tRNS,  // Transparency
    PNG_CHUNK_zTXt,   // Compressed textual data
    
    CHUNK_COUNT,
};

enum {
    NOT_REQUIRED,
    IS_REQUIRED,
};

enum {
    NOT_VALID,
    IS_VALID,
};

typedef struct {
    int required;
    int valid;
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

#endif
