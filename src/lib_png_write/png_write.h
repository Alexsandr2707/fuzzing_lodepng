#ifndef _H_PNG_WRITE
#define _H_PNG_WRITE

#include <png.h>
#include "png_processing.h"

png_bytep *make_row_pointers(png_processing_t *png_prc, png_bytep pic);
int write_png(png_processing_t *png_prc, png_bytep pic);


#endif
