#ifndef _H_PNG_WRITE
#define _H_PNG_WRITE

#include <png.h>
#include "png_processing.h"

int png_get_offset_info(png_processing_t *png_prc, png_bytep file, size_t file_size);

png_bytep *make_row_pointers(png_processing_t *png_prc, png_bytep pic);
int png_write(png_processing_t *png_prc, png_bytep pic);

int png_read(png_processing_t *png_prc, const uint8_t *file, size_t file_size);

#endif
