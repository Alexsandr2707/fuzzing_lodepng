#include <math.h>
#include <stdlib.h>
#include "png_random_config.h"
#include "png_processing.h"

size_t rand_in_range(size_t min, size_t max) {
    return min + (rand() % (max - min + 1));
}

size_t min(size_t a, size_t b) {
    if (a <= b) 
        return a;
    else 
        return b;
}

size_t max(size_t a, size_t b) {
    if (a >= b) 
        return a;
    else 
        return b;
}

// pixel_count == height * width (not real size of picture)
int get_random_hw(size_t pixel_count, size_t *height, size_t *width) {
    if (pixel_count < MIN_WIDTH * MIN_HEIGHT || height == NULL || width == NULL)
        return -1;

    if (MIN_HEIGHT <= MIN_WIDTH) {
        *height = rand_in_range(max(sqrt(pixel_count), MIN_HEIGHT), min(sqrt(pixel_count), MAX_HEIGHT));
        *width = pixel_count / *height;
    } else {
        *width = rand_in_range(max(sqrt(pixel_count), MIN_WIDTH), min(sqrt(pixel_count), MAX_WIDTH));
        *height = pixel_count / *width;
    }

    return 0;
}

int png_set_random_IHDR(png_processing_t *png_prc, size_t pic_size) {
    if (png_prc == NULL)
        return -1;

    size_t height, width, pixel_count;
    int bit_depth, color_type, compress_method, filter_method, interplace_method, pixel_bytes;

    // color type
    color_type = COLOR_TYPE[rand() % sizeof(COLOR_TYPE)];

    // bit_depth
    if (color_type == PNG_COLOR_TYPE_GRAY) {
        bit_depth = GRAY_DEPTH[rand() % sizeof(GRAY_DEPTH)];
    } else if (color_type == PNG_COLOR_TYPE_PALETTE) {
        bit_depth = PALETTE_DEPTH[rand() % sizeof(PALETTE_DEPTH)];
    } else {
        bit_depth = BASE_DEPTH[rand() % sizeof(BASE_DEPTH)];
    }

    
    // pixel count
    if (bit_depth < 8) {
        pixel_count = (pic_size * (8 / bit_depth)) / channels_count(color_type);
    } else {
        pixel_bytes = calculate_bytes_per_pixel(color_type, bit_depth);
        if (pixel_count < 0)
            return -1;
        pixel_count = pic_size / pixel_bytes;
    }

    if (get_random_hw(pixel_bytes, &height, &width) < 0)
        return -1;

    compress_method = COMPRESS_METHOD[rand() % sizeof(COMPRESS_METHOD)];
    filter_method = FILTER_METHOD[rand() % sizeof(COMPRESS_METHOD)];
    interplace_method = INTERPLACE_METHOD[rand() % sizeof(INTERPLACE_METHOD)];

    png_set_IHDR(png_prc->png, png_prc->info, 
                 width, height, 
                 bit_depth, 
                 color_type, 
                 interplace_method, 
                 compress_method, 
                 filter_method);

    return 0;       
}

