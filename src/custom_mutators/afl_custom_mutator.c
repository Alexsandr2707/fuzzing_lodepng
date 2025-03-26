#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <png.h>
#include "afl-fuzz.h"
#include "alloc-inl.h"
#include "png_processing.h"
#include "png_random.h"
#include "png_print.h"

png_processing_t *afl_custom_init(void *afl) {
    png_processing_t *png_prc = create_png_processing();
    if (png_prc == NULL)
        return NULL;

    make_randomize();

    return png_prc;
}

void afl_custom_deinit(png_processing_t *png_prc) {
    destroy_png_processing(png_prc);
}

size_t afl_custom_fuzz(png_processing_t *png_prc, uint8_t *buf, size_t buf_size,
                       uint8_t **out_buf, uint8_t *add_buf,
                       size_t add_buf_size,  // add_buf can be NULL
                       size_t max_size) {
    if (buf == NULL) {
        return 0;
    }

    if (reset_png_processing(png_prc) < 0)
        return 0;

    if (make_random_png(png_prc, buf, buf_size) < 0) {
       return 0;
    }

    //print_png_info(png_prc);

    *out_buf = png_prc->png_out.data;

    return png_prc->png_out.len;
}


