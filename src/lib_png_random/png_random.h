#ifndef _H_PNG_RANDOM_CONFIG
#define _H_PNG_RANDOM_CONFIG

#include <png.h>
#include "png_processing.h"

enum {
    MIN_WIDTH = 1,
    MIN_HEIGHT = 1,
    MAX_WIDTH = 1000,
    MAX_HEIGHT = 1000,

    MIN_COMPRESSION_LEVEL = 0,
    MAX_COMPRESSION_LEVEL = 9,

    MIN_COLOR = 0,
    MAX_COLOR = 255,

    PALETTE_SIZE = 256,
};


int channels_count(int color_type);
int calculate_bytes_per_pixel(int color_type, int bit_depth);


void print_hw(int height, int width);
void print_color_type(int color_type);
void print_bit_depth(int bit_depth);
void print_compress_method(int compress_method);
void print_filter_method(int filter_method);
void print_interlace_method(int interlace_method);

void print_png_info(png_processing_t *png_prc);

int get_random_hw(size_t pic_size, size_t *height, size_t *width);

int png_config_IHDR(png_processing_t *png_prc, size_t pic_size);

int png_set_random_chunks(png_processing_t *png_prc);

void print_IHDR_info(png_processing_t *png_prc);

int png_config_bKGD(png_processing_t *png_prc);
int png_config_sPLT(png_processing_t *png_prc);

int png_config_chunks(png_processing_t *png_prc, size_t pic_size);

#endif
