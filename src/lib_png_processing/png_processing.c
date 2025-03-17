#include <png.h>
#include <stdlib.h>
#include "png_processing.h"
#include "afl_vector.h"

png_processing_t *create_png_processing(void) {
    png_processing_t *png_prc = calloc(1, sizeof(*png_prc));
    if (png_prc == NULL) 
        return NULL;

    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png) {
        free(png_prc);
        return NULL;
    }

    png_infop info = png_create_info_struct(png);
    if (!info) {
        free(png_prc);
        png_destroy_write_struct(&png, NULL);
            return NULL;
    }

    png_prc->png = png;
    png_prc->info = info;
    init_afl_vector(&(png_prc->png_out), NULL, 0, 0, 0);

    return png_prc;
}

void destroy_png_processing(png_processing_t *png_prc) {
    png_destroy_write_struct(&(png_prc->png), &(png_prc->info));
    deinit_afl_vector(&(png_prc->png_out), STATIC);
    free(png_prc);
}
