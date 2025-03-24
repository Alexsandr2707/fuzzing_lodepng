#ifndef _H_PNG_RANDOM_CONFIG
#define _H_PNG_RANDOM_CONFIG

#include <png.h>
#include "png_processing.h"

enum {
    MIN_WIDTH = 1,
    MIN_HEIGHT = 1,
    MAX_WIDTH = 1000,
    MAX_HEIGHT = 1000,

    MIN_PIC_SIZE = 8 * MIN_WIDTH * MIN_HEIGHT,
    MAX_PIC_SIZE = 8 * MAX_WIDTH * MAX_HEIGHT,

    MIN_COMPRESSION_LEVEL = 0,
    MAX_COMPRESSION_LEVEL = 9,

    BIT8_MIN_COLOR = 0,
    BIT8_MAX_COLOR = 255,

    BIT16_MIN_COLOR = 0,
    BIT16_MAX_COLOR = 65535,

    PALETTE_SIZE = 256,

    ICC_HEADER_SIZE = 128,
    MIN_ICC_DATA_SIZE = 8,
    MAX_ICC_DATA_SIZE = 128,

    MIN_PHYS_METER = 1, 
    MAX_PHYS_METER = 1000,

    MIN_PHYS_UNKNOWN = 1, 
    MAX_PHYS_UNKNOWN = 100,

};

#define ICC_PROFILE_NAME "RandomICCProfile"

#define MIN_GAMA 0.1
#define MAX_GAMA 10.0

#define EXTREAM_MIN_GAMA (1.0 / 1000000)
#define EXTREAM_MAX_GAMA 100.0

#define MIN_CHRM_VAL 0.0001
#define MAX_CHRM_VAL 0.7999

#define RAND_FILE "/dev/urandom"

int channels_count(int color_type);
int calculate_bytes_per_pixel(int color_type, int bit_depth);


void make_randomize(); // !!! important
int get_random_hw(size_t pic_size, size_t *height, size_t *width);
int png_set_random_chunks(png_processing_t *png_prc);

int png_config_IHDR(png_processing_t *png_prc, size_t pic_size);

int png_config_bKGD(png_processing_t *png_prc);
int png_config_tRNS(png_processing_t *png_prc);
int png_config_sPLT(png_processing_t *png_prc);
int png_config_cHRM(png_processing_t *png_prc);
int png_config_iCCP(png_processing_t *png_prc);
int png_config_pHYs(png_processing_t *png_prc);
int png_config_sBIT(png_processing_t *png_prc);
int png_config_sRGB(png_processing_t *png_prc);
int png_config_tEXt(png_processing_t *png_prc);
int png_config_iTXt(png_processing_t *png_prc);
int png_config_tIME(png_processing_t *png_prc);
int png_config_cSTM(png_processing_t *png_prc);

int png_config_chunks(png_processing_t *png_prc, size_t pic_size);
int make_random_png(png_processing_t *png_prc, uint8_t *pic, size_t pic_size);

#endif
