#include <math.h>
#include <stdlib.h>
#include "png_random.h"
#include "png_processing.h"

const int COLOR_TYPE[] = {PNG_COLOR_TYPE_GRAY, 
                          PNG_COLOR_TYPE_RGB, 
                          PNG_COLOR_TYPE_PALETTE, 
                          PNG_COLOR_TYPE_GRAY_ALPHA, 
                          PNG_COLOR_TYPE_RGBA};

const int GRAY_DEPTH[] = {8, 16}; // also can be = 1, 2, 4
const int PALETTE_DEPTH[] = {8}; // also can be = 1, 2, 4
const int BASE_DEPTH[] = {8, 16};

const int COMPRESS_METHOD[] = {PNG_COMPRESSION_TYPE_DEFAULT};
const int FILTER_METHOD[] = {PNG_FILTER_TYPE_DEFAULT};
const int INTERLACE_METHOD[] = {PNG_INTERLACE_NONE, PNG_INTERLACE_ADAM7};

//arrays sizes
enum {
    COLOR_TYPE_SIZE = 5,

    GRAY_DEPTH_SIZE = 2,
    PALETTE_DEPTH_SIZE = 1,
    BASE_DEPTH_SIZE = 2,

    COMPRESS_METHOD_SIZE = 1,
    FILTER_METHOD_SIZE = 1,
    INTERLACE_METHOD_SIZE = 2,
};


int channels_count(int color_type) {
    int channels = 0;

    switch (color_type) {
        case PNG_COLOR_TYPE_GRAY:
            channels = 1;
            break;
        case PNG_COLOR_TYPE_RGB:
            channels = 3;
            break;
        case PNG_COLOR_TYPE_PALETTE:
            channels = 1;
            break;
        case PNG_COLOR_TYPE_GRAY_ALPHA:
            channels = 2;
            break;
        case PNG_COLOR_TYPE_RGBA:
            channels = 4;
            break;
        default:
            return -1; 
    }

    return channels;
}

int calculate_bytes_per_pixel(int color_type, int bit_depth) {
    int channels = channels_count(color_type);
    if (channels_count < 0) 
        return -1;

    return (bit_depth * channels + 7) / 8; 
}


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

void print_hw(int height, int width) {
    printf("height: %d\n", height);
    printf("width: %d\n", width);
}

void print_color_type(int color_type) {
    printf("color_type: ");
    switch (color_type) {
        case PNG_COLOR_TYPE_GRAY: printf("PNG_COLOR_TYPE_GRAY"); break;
        case PNG_COLOR_TYPE_RGB: printf("PNG_COLOR_TYPE_RGB"); break;
        case PNG_COLOR_TYPE_PALETTE: printf("PNG_COLOR_TYPE_PALETTE"); break;
        case PNG_COLOR_TYPE_GRAY_ALPHA: printf("PNG_COLOR_TYPE_GRAY_ALPHA"); break;
        case PNG_COLOR_TYPE_RGBA: printf("PNG_COLOR_TYPE_RGBA"); break;
        default: printf("undefined color");
    }
    
    printf("\n");
}

void print_bit_depth(int bit_depth) {
    printf("bit_depth: %d\n", bit_depth);
}

void print_compress_method(int compress_method) {
    printf("compress_method: ");
    switch (compress_method) {
        case PNG_COMPRESSION_TYPE_DEFAULT: printf("PNG_COMPRESSION_TYPE_DEFAULT"); break;
        default: printf("undifined compression type");
    }

    printf("\n");
}

void print_filter_method(int filter_method) {
    printf("filter_method; ");
    switch (filter_method) {
        case PNG_FILTER_TYPE_DEFAULT: printf("PNG_FILTER_TYPE_DEFAULT"); break;
        default: printf("undefinded filter method");
    }
    printf("\n");
}

void print_interlace_method(int interlace_method) {
    printf("interlace_method: ");
    switch (interlace_method) {
        case PNG_INTERLACE_NONE: printf("PNG_INTERLACE_NONE"); break;
        case PNG_INTERLACE_ADAM7: printf("PNG_INTERLACE_ADAM7"); break;
        default: printf("undefined interlace method");
    }

    printf("\n");
}


void print_IHDR_info(png_processing_t *png_prc) {
    if (png_prc == NULL)
        return;

    printf("----- print IHDR -----\n");

    int height = png_get_image_height(png_prc->png, png_prc->info);
    int width = png_get_image_width(png_prc->png, png_prc->info);
    print_hw(height, width);

    int color_type = png_get_color_type(png_prc->png, png_prc->info);
    print_color_type(color_type);

    int bit_depth = png_get_bit_depth(png_prc->png, png_prc->info);
    print_bit_depth(bit_depth);

    int compress_method = png_get_compression_type(png_prc->png, png_prc->info);
    print_compress_method(compress_method);

    int filter_method = png_get_filter_type(png_prc->png, png_prc->info);
    print_filter_method(filter_method);

    int interlace_method = png_get_interlace_type(png_prc->png, png_prc->info);
    print_interlace_method(interlace_method);

    printf("----- print IHDR DONE -----\n");
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


// also set compression level
int png_set_random_IHDR(png_processing_t *png_prc, size_t pic_size) {
    if (png_prc == NULL)
        return -1;

    size_t height, width, pixel_count;
    int bit_depth, color_type, pixel_bytes, compression_level;
    int compress_method, filter_method, interlace_method; 

    // color type
    color_type = COLOR_TYPE[rand() % COLOR_TYPE_SIZE];
    if (color_type == PNG_COLOR_TYPE_PALETTE) {
        png_prc->chunks[PNG_CHUNK_sPLT].is_valid = IS_VALID;
    }

    // bit_depth
    if (color_type == PNG_COLOR_TYPE_GRAY) {
        bit_depth = GRAY_DEPTH[rand() % GRAY_DEPTH_SIZE];
    } else if (color_type == PNG_COLOR_TYPE_PALETTE) {
        bit_depth = PALETTE_DEPTH[rand() % PALETTE_DEPTH_SIZE];
    } else {
        bit_depth = BASE_DEPTH[rand() % BASE_DEPTH_SIZE];
    }

    // pixel count
    if (bit_depth < 8) {
        pixel_bytes = 1;
        pixel_count = (pic_size * (8 / bit_depth)) / channels_count(color_type);
    } else {
        pixel_bytes = calculate_bytes_per_pixel(color_type, bit_depth);
        if (pixel_bytes < 0)
            return -1;
        pixel_count = pic_size / pixel_bytes;
    }
    
    if (get_random_hw(pixel_count, &height, &width) < 0)
        return -1;

    compress_method = COMPRESS_METHOD[rand() % COMPRESS_METHOD_SIZE];
    filter_method = FILTER_METHOD[rand() % COMPRESS_METHOD_SIZE];
    interlace_method = INTERLACE_METHOD[rand() % INTERLACE_METHOD_SIZE];

    compression_level = rand_in_range(MIN_COMPRESSION_LEVEL, MAX_COMPRESSION_LEVEL);
    png_set_compression_level(png_prc->png,compression_level);

    png_set_IHDR(png_prc->png, png_prc->info, 
                 width, height, 
                 bit_depth, 
                 color_type, 
                 interlace_method, 
                 compress_method, 
                 filter_method);

    return 0;       
}



int png_set_random_chunks(png_processing_t *png_prc) {
    if (png_prc == NULL)
        return -1;

    for (int i = 0; i < CHUNK_COUNT; ++i) {
        png_prc->chunks[i].is_valid |= rand() % 2;
    }

    return 0;     
}
