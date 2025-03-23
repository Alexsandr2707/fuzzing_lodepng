#include <png.h>
#include <string.h>
#include <stdio.h>
#include "png_print.h"
#include "png_processing.h"

#define PRINT " print " 
#define INFO " info "
#define END "END "

void print_chunk_header(const char chunk_name[16], char fill_sym, int len) {
    len = (len - strlen(PRINT) - strlen(chunk_name) - strlen(INFO)) / 2;
    if (len < 0) 
        len = 0;

    for (int i = 0; i < len; ++i) {
        printf("%c", fill_sym);
    }

    printf(PRINT);
    printf("%s", chunk_name);
    printf(INFO);

    for (int i = 0; i < len; ++i) {
        printf("%c", fill_sym);
    }
    printf("\n");
}

void print_chunk_end(const char chunk_name[5], char fill_sym, int len) {
    len = (len - strlen(PRINT) - strlen(chunk_name) - strlen(INFO) - strlen(END)) / 2;
    if (len < 0) 
        len = 0;

    for (int i = 0; i < len; ++i) {
        printf("%c", fill_sym);
    }

    printf(PRINT);
    printf("%s", chunk_name);
    printf(INFO);
    printf(END);

    for (int i = 0; i < len; ++i) {
        printf("%c", fill_sym);
    }
    printf("\n");
}

void print_hw(int height, int width) {
    printf("  height: %d\n", height);
    printf("  width: %d\n", width);
}

void print_color_type(int color_type) {
    printf("  color_type: ");
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
    printf("  bit_depth: %d\n", bit_depth);
}

void print_compress_method(int compress_method) {
    printf("  compress_method: ");
    switch (compress_method) {
        case PNG_COMPRESSION_TYPE_DEFAULT: printf("PNG_COMPRESSION_TYPE_DEFAULT"); break;
        default: printf("undifined compression type");
    }

    printf("\n");
}

void print_filter_method(int filter_method) {
    printf("  filter_method; ");
    switch (filter_method) {
        case PNG_FILTER_TYPE_DEFAULT: printf("PNG_FILTER_TYPE_DEFAULT"); break;
        default: printf("undefinded filter method");
    }
    printf("\n");
}

void print_interlace_method(int interlace_method) {
    printf("  interlace_method: ");
    switch (interlace_method) {
        case PNG_INTERLACE_NONE: printf("PNG_INTERLACE_NONE"); break;
        case PNG_INTERLACE_ADAM7: printf("PNG_INTERLACE_ADAM7"); break;
        default: printf("undefined interlace method");
    }

    printf("\n");
}

void print_PNGChunk_info(PNGChunk_t *chunk) {
    if (chunk == NULL) { 
        printf("Bad pointer\n");
        return;
    }

    if (chunk->required == IS_REQUIRED) {
        printf("  required: IS_REQUIRED\n");
    } else if (chunk->required == NOT_REQUIRED) {
        printf("  required: NOT_REQUIRED\n");
    } else {
        printf("  required: undefind\n");
    }

    if (chunk->valid == IS_VALID) {
        printf("  valid: IS_VALID\n");
    } else if (chunk->valid == NOT_VALID) {
        printf("  valid: NOT_VALID\n");
    } else {
        printf("  valid: undefind\n");
    }
    print_vector_info(&(chunk->info));
    printf("\n");
}

void print_IHDR_info(png_processing_t *png_prc) {
    print_chunk_header("IHDR", '-', PRINT_HEADER_LENGTH);

    if (png_prc == NULL)         
        return;
    
    print_PNGChunk_info(&(png_prc->chunks[PNG_CHUNK_IHDR]));

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
    
    print_chunk_end("IHDR", '-', PRINT_END_LENGTH);
}

void print_bKGD_info(png_processing_t *png_prc) {
    print_chunk_header("bKGD", '-', PRINT_HEADER_LENGTH);

    if (png_prc == NULL)
        return ;

    print_PNGChunk_info(&(png_prc->chunks[PNG_CHUNK_bKGD]));
    
    png_color_16p background;

    png_structp png = png_prc->png;
    png_infop info = png_prc->info;

    if (png_get_bKGD(png, info, &background)) {
        png_byte color_type = png_get_color_type(png, info);

        if (color_type == PNG_COLOR_TYPE_PALETTE) {
            printf("  Background color index: %d\n", background->index);
        } else if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
            printf("  Background gray value: %d\n", background->gray);
        } else if (color_type == PNG_COLOR_TYPE_RGB || color_type == PNG_COLOR_TYPE_RGB_ALPHA) {
            printf("  Background RGB color: (%d, %d, %d)\n", background->red, background->green, background->blue);
        }
    } else {
        printf("No bKGD chunk found in the PNG file\n");
    }

    print_chunk_end("bKGD", '-', PRINT_END_LENGTH);
}

void print_tRNS_info(png_processing_t *png_prc) {
    print_chunk_header("tRNS", '-', PRINT_HEADER_LENGTH);

    if (png_prc == NULL)
        return ;
    print_PNGChunk_info(&(png_prc->chunks[PNG_CHUNK_tRNS]));

    print_chunk_end("tRNS", '-', PRINT_END_LENGTH);
}

void print_sPLT_info(png_processing_t *png_prc) {
    print_chunk_header("sPLT", '-', PRINT_HEADER_LENGTH);
    if (png_prc == NULL)
        return ;
    print_PNGChunk_info(&(png_prc->chunks[PNG_CHUNK_sPLT]));

    print_chunk_end("sPLT", '-', PRINT_END_LENGTH);
}

void print_gAMA_info(png_processing_t *png_prc) {
    print_chunk_header("gAMA", '-', PRINT_HEADER_LENGTH);

    if (png_prc == NULL)
        return ;
    print_PNGChunk_info(&(png_prc->chunks[PNG_CHUNK_gAMA]));

    double gamma;
    if (png_get_gAMA(png_prc->png, png_prc->info, &gamma)) 
        printf("  gamma: %lf\n", gamma);
    else 
        printf("gamma: is undefined\n");

    print_chunk_end("gAMA", '-', PRINT_END_LENGTH);
}

void print_cHRM_info(png_processing_t *png_prc) {
    print_chunk_header("cHRM", '-', PRINT_HEADER_LENGTH);

    if (png_prc == NULL)
        return ;
    print_PNGChunk_info(&(png_prc->chunks[PNG_CHUNK_cHRM]));

    double wx, wy, red_x, red_y, green_x, green_y, blue_x, blue_y;
    if (png_get_cHRM(png_prc->png, png_prc->info, &wx, &wy, &red_x, &red_y, &green_x, &green_y, &blue_x, &blue_y)) {
        printf("cHRM chunk finded:\n");
        printf("  White point: wx = %.4f, wy = %.4f\n", wx, wy);
        printf("  Red:         x = %.4f, y = %.4f\n", red_x, red_y);
        printf("  Green:       x = %.4f, y = %.4f\n", green_x, green_y);
        printf("  Blue:        x = %.4f, y = %.4f\n", blue_x, blue_y);
    } else {
        printf("cHRM chunk undefined\n");
    }

    print_chunk_end("cHRM", '-', PRINT_END_LENGTH);
}

void print_iCCP_info(png_processing_t *png_prc) {
    print_chunk_header("iCCP", '-', PRINT_HEADER_LENGTH);

    if (png_prc == NULL)
        return ;

    print_PNGChunk_info(&(png_prc->chunks[PNG_CHUNK_iCCP]));

    const size_t out_profile_bytes = 20;

    png_charp profile_name;
    png_bytep profile_data;
    png_uint_32 profile_length;
    int compression_type;

    if (png_get_iCCP(png_prc->png, png_prc->info, &profile_name, &compression_type, &profile_data, &profile_length)) {
        printf("iCCP chunk found:\n");
        printf("  Profile name: %s\n", profile_name);
        printf("  Compression type: %d\n", compression_type);
        printf("  Profile length: %u bytes\n", profile_length);

        printf("  First 16 bytes of profile data: ");
        for (png_uint_32 i = 0; i < profile_length && i < out_profile_bytes; ++i) {
            printf("%02X ", profile_data[i]);
        }

        if (profile_length > out_profile_bytes) 
            printf("...");

        printf("\n");
    } else {
        printf("iCCP chunk not found in the file\n");
    }

    print_chunk_end("iCCP", '-', PRINT_END_LENGTH);
}

void print_pHYs_info(png_processing_t *png_prc) {
    print_chunk_header("pHYs", '-', PRINT_HEADER_LENGTH);

    if (png_prc == NULL)
        return ;
    uint32_t res_x, res_y;
    int unit_type;

    print_PNGChunk_info(&(png_prc->chunks[PNG_CHUNK_pHYs]));

    if (png_get_pHYs(png_prc->png, png_prc->info, &res_x, &res_y, &unit_type)) {
        printf("pHYs chunk data:\n");
        printf("  Resolution X: %u pixels per unit\n", res_x);
        printf("  Resolution Y: %u pixels per unit\n", res_y);

        switch (unit_type) {
            case PNG_RESOLUTION_UNKNOWN:
                printf("  Unit type: Unknown (aspect ratio)\n");
                break;
            case PNG_RESOLUTION_METER:
                printf("  Unit type: Meters\n");
                if (res_x == res_y) {
                    double dpi = res_x * 0.0254; 
                    printf("  Equivalent DPI: %.2f\n", dpi);
                } 
                break;
            default:
                printf("  Unit type: Invalid\n");
                break;
        }
    } else {
        printf("pHYs chunk not found in the PNG file.\n");
    }
    
    print_chunk_end("pHYs", '-', PRINT_END_LENGTH);
}

void print_sBIT_info(png_processing_t *png_prc) {
    print_chunk_header("sBIT", '-', PRINT_HEADER_LENGTH);

    if (png_prc == NULL)
        return ;

    print_PNGChunk_info(&(png_prc->chunks[PNG_CHUNK_sBIT]));

    png_color_8p sBIT;
    int color_type = png_get_color_type(png_prc->png, png_prc->info);

    if (png_get_sBIT(png_prc->png, png_prc->info, &sBIT)) {
        printf("Contents of sBIT chunk:\n");

        switch (color_type) {
            case PNG_COLOR_TYPE_GRAY:
                printf("  Grayscale: %d significant bits\n", sBIT->gray);
                break;
            case PNG_COLOR_TYPE_GRAY_ALPHA:
                printf("  Grayscale: %d significant bits\n", sBIT->gray);
                printf("  Alpha: %d significant bits\n", sBIT->alpha);
                break;
            case PNG_COLOR_TYPE_RGB:
                printf("  Red: %d significant bits\n", sBIT->red);
                printf("  Green: %d significant bits\n", sBIT->green);
                printf("  Blue: %d significant bits\n", sBIT->blue);
                break;
            case PNG_COLOR_TYPE_RGB_ALPHA:
                printf("  Red: %d significant bits\n", sBIT->red);
                printf("  Green: %d significant bits\n", sBIT->green);
                printf("  Blue: %d significant bits\n", sBIT->blue);
                printf("  Alpha: %d significant bits\n", sBIT->alpha);
                break;
            case PNG_COLOR_TYPE_PALETTE:
                printf("  Palette: sBIT is not used\n");
                break;
            default:
                printf("  Unknown color type\n");
                break;
        }
    } else {
        printf("sBIT chunk not found\n");
    }

    print_chunk_end("sBIT", '-', PRINT_END_LENGTH);
}

void print_png_info(png_processing_t *png_prc) {
    print_chunk_header("PNGs", '#', PRINT_HEADER_LENGTH);

    if (png_prc == NULL)
        return ;

    print_IHDR_info(png_prc);
    printf("\n");

    print_bKGD_info(png_prc);
    printf("\n");
   
    print_tRNS_info(png_prc);
    printf("\n");

    print_sPLT_info(png_prc);
    printf("\n");

    print_gAMA_info(png_prc);
    printf("\n");

    print_cHRM_info(png_prc);
    printf("\n");

    print_iCCP_info(png_prc);
    printf("\n");

    print_pHYs_info(png_prc);
    printf("\n");

    print_sBIT_info(png_prc);
    printf("\n");

    print_chunk_end("PNGs", '#', PRINT_END_LENGTH);
}

