#include <png.h>
#include <stdlib.h>
#include "png_write.h"
#include "png_processing.h"
#include "png_random.h"
#include "afl_vector.h"

void png_write_to_afl_vector(png_structp png, png_bytep buf, size_t buf_size) {
    AFLVector *vector = png_get_io_ptr(png);
    write_to_afl_vector(vector, buf, buf_size);
}

png_bytep *make_row_pointers(png_processing_t *png_prc, png_bytep pic) {
    size_t height = png_get_image_height(png_prc->png, png_prc->info);
    size_t width = png_get_image_width(png_prc->png, png_prc->info);

    size_t pixel_bytes = calculate_bytes_per_pixel(png_get_color_type(png_prc->png, png_prc->info), 
                                                   png_get_bit_depth(png_prc->png, png_prc->info));

    png_bytep *row_pointers = calloc(height, sizeof(png_bytep));
    if (row_pointers == NULL)
        return NULL;
    
    for (size_t i = 0; i < height; ++i) {
        row_pointers[i] = pic + i * width * pixel_bytes;
    }

    return row_pointers;
}


int png_write(png_processing_t *png_prc, png_bytep pic) {
    AFLVector *vector = &(png_prc->png_out);
    vector->len = 0;

    png_bytep *row_pointers = make_row_pointers(png_prc, pic);
    if (row_pointers == NULL)
        return -1;

    png_set_write_fn(png_prc->png, vector, png_write_to_afl_vector, NULL);

    png_write_info(png_prc->png, png_prc->info);
    png_write_image(png_prc->png, row_pointers);
    png_write_end(png_prc->png, NULL);

    free(row_pointers);
    return 0;
}
