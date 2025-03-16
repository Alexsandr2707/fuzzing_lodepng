#ifndef _H_PNG_RANDOM_CONFIG
#define _H_PNG_RANDOM_CONFIG

#include <png.h>

// chunks
enum {
    IHDR,
    IDAT,
    IEND,
    BASE_CHUNKS_COUNT,


};

enum {
    MIN_WIDTH = 1,
    MIN_HEIGHT = 1,
    MAX_WIDTH = 1000,
    MAX_HEIGHT = 1000,
};

const int COLOR_TYPE[] = {PNG_COLOR_TYPE_GRAY, 
                          PNG_COLOR_TYPE_RGB, 
                          PNG_COLOR_TYPE_PALETTE, 
                          PNG_COLOR_TYPE_GRAY_ALPHA, 
                          PNG_COLOR_TYPE_RGBA};

const int GRAY_DEPTH[] = {1, 2, 4, 8, 16};
const int PALETTE_DEPTH[] = {1, 2, 4, 8};
const int BASE_DEPTH[] = {8, 16};

const int COMPRESS_METHOD[] = {PNG_COMPRESSION_TYPE_DEFAULT};
const int FILTER_METHOD[] = {PNG_FILTER_TYPE_DEFAULT};
const int INTERPLACE_METHOD[] = {PNG_INTERLACE_NONE, PNG_INTERLACE_ADAM7};


int get_random_hw(size_t pic_size, size_t *height, size_t *width);


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

#endif
