#include <png.h>
#include "png_processing.h"
#include "afl_vector.h"

void png_write_to_afl_vector(png_structp png, png_bytep buf, size_t buf_size) {
    AFLVector *vector = png_get_io_ptr(png);
    write_to_afl_vector(vector, buf, buf_size);

}


int write_png(png_processing_t *png_prc, png_bytep *row_pointers) {
    AFLVector *vector = &(png_prc->png_out);

    png_set_write_fn(png_prc->png, vector, png_write_to_afl_vector, NULL);

    png_write_info(png_prc->png, png_prc->info);
    png_write_image(png_prc->png, row_pointers);
    png_write_end(png_prc->png, NULL);

    return 0;
}
