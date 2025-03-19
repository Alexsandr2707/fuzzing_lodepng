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
    if (png_prc == NULL || png_prc->chunks[PNG_CHUNK_IHDR].required != IS_REQUIRED)
        return;

    printf("----- print IHDR info -----\n");

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
    
    printf("----- print IHDR info DONE -----\n");
}

void print_bKGD_info(png_processing_t *png_prc) {
    printf("----- print bKGD info -----\n");
    png_color_16p background;

    png_structp png = png_prc->png;
    png_infop info = png_prc->info;

    if (png_get_bKGD(png, info, &background)) {
        png_byte color_type = png_get_color_type(png, info);

        if (color_type == PNG_COLOR_TYPE_PALETTE) {
            printf("Background color index: %d\n", background->index);
        } else if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
            printf("Background gray value: %d\n", background->gray);
        } else if (color_type == PNG_COLOR_TYPE_RGB || color_type == PNG_COLOR_TYPE_RGB_ALPHA) {
            printf("Background RGB color: (%d, %d, %d)\n", background->red, background->green, background->blue);
        }
    } else {
        printf("No bKGD chunk found in the PNG file.\n");
    }

    printf("----- print bKGD info DONE -----\n");
}

void print_png_info(png_processing_t *png_prc) {
    printf("---------- print PNG info -----------\n\n");
    print_IHDR_info(png_prc);
    printf("\n");

    print_bKGD_info(png_prc);
    printf("\n");
    
    printf("---------- print PNG info DONE -----------\n\n");
}

// pixel_count == height * width (not real size of picture)
int get_random_hw(size_t pixel_count, size_t *height, size_t *width) {
    if (pixel_count < MIN_WIDTH * MIN_HEIGHT || height == NULL || width == NULL)
        return -1;

    if (MIN_HEIGHT <= MIN_WIDTH) {
        *height = rand_in_range(MIN_HEIGHT, min(sqrt(pixel_count), MAX_HEIGHT));
        *width = min(pixel_count / *height, MAX_WIDTH);
    } else {
        *width = rand_in_range(MIN_WIDTH, min(sqrt(pixel_count), MAX_WIDTH));
        *height = min(pixel_count / *width, MAX_HEIGHT);
    }

    return 0;
}


// also set compression level
int png_config_IHDR(png_processing_t *png_prc, size_t pic_size) {
    if (png_prc == NULL)
        return -1;

    size_t height, width, pixel_count;
    int bit_depth, color_type, pixel_bytes, compression_level;
    int compress_method, filter_method, interlace_method; 

    // color type
    color_type = COLOR_TYPE[rand() % COLOR_TYPE_SIZE];
    if (color_type == PNG_COLOR_TYPE_PALETTE) {
        png_prc->chunks[PNG_CHUNK_sPLT].required = IS_REQUIRED;
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
        png_prc->chunks[i].required |= rand() % 2;
    }

    return 0;     
}

int png_config_bKGD(png_processing_t *png_prc) {
    png_color_16 background;
    int color_type = png_get_color_type(png_prc->png, png_prc->info);

    if (color_type == PNG_COLOR_TYPE_PALETTE)
        background.index = rand_in_range(MIN_COLOR, MAX_COLOR);
    else if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA){
        background.gray = rand_in_range(MIN_COLOR, MAX_COLOR);
    } else {
        background.red = rand_in_range(MIN_COLOR, MAX_COLOR);
        background.green = rand_in_range(MIN_COLOR, MAX_COLOR);
        background.blue = rand_in_range(MIN_COLOR, MAX_COLOR);
    }

    png_set_bKGD(png_prc->png, png_prc->info, &background);
    return 0;
}

int png_config_sPLT(png_processing_t *png_prc) {
    if (png_prc->chunks[PNG_CHUNK_sPLT].required != IS_REQUIRED) 
        return -1;

    png_color *palette = png_prc->chunks[PNG_CHUNK_sPLT].info;
    if (palette == NULL) {
        palette = malloc(sizeof(palette) * PALETTE_SIZE);
        if (palette == NULL)
            return -1;

        png_prc->chunks[PNG_CHUNK_sPLT].info = palette;
    }


    for (int i = 0; i < PALETTE_SIZE; ++i) {
        palette[i].red = rand_in_range(MIN_COLOR, MAX_COLOR);
        palette[i].green = rand_in_range(MIN_COLOR ,MAX_COLOR);
        palette[i].blue = rand_in_range(MIN_COLOR, MAX_COLOR);
    }

    png_set_PLTE(png_prc->png, png_prc->info, palette, PALETTE_SIZE);

    return 0;
}


int png_config_chunks(png_processing_t *png_prc, size_t pic_size) {
    if (png_prc->chunks[PNG_CHUNK_IHDR].required == IS_REQUIRED && 
        png_prc->chunks[PNG_CHUNK_IHDR].valid != IS_VALID) { 
        if (png_config_IHDR(png_prc, pic_size) < 0)
            return -1;
    }
    
    if (png_prc->chunks[PNG_CHUNK_bKGD].required == IS_REQUIRED &&
        png_prc->chunks[PNG_CHUNK_bKGD].valid != IS_VALID) { 
        if (png_config_bKGD(png_prc) < 0)
            return -1;
    }
    
    if (png_prc->chunks[PNG_CHUNK_sPLT].required == IS_REQUIRED &&
        png_prc->chunks[PNG_CHUNK_sPLT].valid != IS_VALID) { 
        if (png_config_sPLT(png_prc) < 0)
            return -1;
    }

    return 0;
}
