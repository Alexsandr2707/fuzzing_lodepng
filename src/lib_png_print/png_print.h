#ifndef _H_PNG_PRINT_CONFIG
#define _H_PNG_PRINT_CONFIG

#include <png.h>
#include "png_processing.h"

enum {
    PRINT_HEADER_LENGTH = 60,
    PRINT_END_LENGTH = PRINT_HEADER_LENGTH,
};

void print_hw(int height, int width);
void print_color_type(int color_type);
void print_bit_depth(int bit_depth);
void print_compress_method(int compress_method);
void print_filter_method(int filter_method);
void print_interlace_method(int interlace_method);

void print_IHDR_info(png_processing_t *png_prc);
void print_bKGD_info(png_processing_t *png_prc);
void print_tRNS_info(png_processing_t *png_prc);
void print_sPLT_info(png_processing_t *png_prc);
void print_gAMA_info(png_processing_t *png_prc);
void print_iCCP_info(png_processing_t *png_prc);
void print_pHYs_info(png_processing_t *png_prc);
void print_sBIT_info(png_processing_t *png_prc);
void print_sRGB_info(png_processing_t *png_prc);
void print_TEXT_info(png_processing_t *png_prc);
void print_tIME_info(png_processing_t *png_prc);
void print_cSTM_info(png_processing_t *png_prc);

void print_png_info(png_processing_t *png_prc);

#endif
