#include <png.h>
#include <string.h>
#include <stdlib.h>
#include "png_processing.h"
#include "afl_vector.h"
#include "vector.h"

enum {
    PNG_CHUNK_NAME_SIZE = 4,
};

typedef struct {
    int type;
    uint8_t name[PNG_CHUNK_NAME_SIZE + 1];
} ChunkType;

const ChunkType PNG_CHUNK_TYPES[] = {
    {PNG_CHUNK_IHDR, "IHDR"},
    {PNG_CHUNK_IDAT, "IDAT"},
    {PNG_CHUNK_IEND, "IEND"},
    
    {PNG_CHUNK_cHRM, "cHRM"},
    {PNG_CHUNK_gAMA, "gAMA"},
    {PNG_CHUNK_sRGB, "sRGB"},
    {PNG_CHUNK_iCCP, "iCCP"},
    {PNG_CHUNK_sBIT, "sBIT"},
    {PNG_CHUNK_PLTE, "PLTE"},
    {PNG_CHUNK_bKGD, "bKGD"},
    {PNG_CHUNK_hIST, "hIST"},
    {PNG_CHUNK_iTXt, "iTXt"},
    {PNG_CHUNK_pHYs, "pHYs"},
    {PNG_CHUNK_sPLT, "sPLT"},
    {PNG_CHUNK_sTER, "sTER"},
    {PNG_CHUNK_tEXt, "tEXt"},
    {PNG_CHUNK_tIME, "tIME"},
    {PNG_CHUNK_tRNS, "tRNS"},
    {PNG_CHUNK_zTXt, "zTXt"},
    
    {PNG_CHUNK_cSTM, "cSTM"},


};

enum {
    PNG_CHUNK_TYPES_SIZE = sizeof(PNG_CHUNK_TYPES) / sizeof(PNG_CHUNK_TYPES[0]),
};

int png_get_chunk_type(const char *name) {
    for (int i = 0; i < PNG_CHUNK_TYPES_SIZE; ++i)  {
        if (strncmp((const char *)&PNG_CHUNK_TYPES[i].name, (const char *)name, PNG_CHUNK_NAME_SIZE) == 0) 
            return PNG_CHUNK_TYPES[i].type;
    }

    return -1;
};

const char *png_get_chunk_name(int type) {
    if (type >= 0 && type < CHUNK_COUNT)
        return (const char *)&(PNG_CHUNK_TYPES[type].name);
    else 
        return NULL;
};

int is_const_info(int type) {
    if (type == PNG_CHUNK_iCCP) {
        return 1;
    } else {
        return 0;
    }
}

int reset_chunk(PNGChunk_t *chunk, int type) {
    if (chunk == NULL)
        return -1;

    chunk->required = UNDEFINED;
    chunk->valid = NOT_VALID;
    chunk->cloned = NOT_CLONED;

    chunk->file_pointer = NULL;
    chunk->file_size = 0;

    if (!is_const_info(type))
        clean_vector(&(chunk->info));

    return 0;
}

int init_chunk(PNGChunk_t *chunk) {
    if (chunk == NULL)
        return -1;

    chunk->required = UNDEFINED;
    chunk->valid = NOT_VALID;
    chunk->cloned = NOT_CLONED;

    chunk->file_pointer = NULL;
    chunk->file_size = 0;

    init_vector(&(chunk->info), NULL, 0, 0, 0);

    return 0;
}

int reset_png_processing(png_processing_t *png_prc) {
    if (png_prc == NULL)
        return -1;

    png_destroy_write_struct(&(png_prc->png), &(png_prc->info));

    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png) {
        return -1;
    }

    png_infop info = png_create_info_struct(png);
    if (!info) {
        png_destroy_write_struct(&png, NULL);
            return -1;
    }

    
    png_prc->png = png;
    png_prc->info = info;

    clean_afl_vector(&(png_prc->png_out));

    for (int i = 0; i < CHUNK_COUNT; ++i) {
       reset_chunk(&(png_prc->chunks[i]), i);
    }

    png_prc->chunks[PNG_CHUNK_IHDR].required = IS_REQUIRED;
    png_prc->chunks[PNG_CHUNK_IDAT].required = IS_REQUIRED;
    png_prc->chunks[PNG_CHUNK_IEND].required = IS_REQUIRED;

    return 0;
}

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
    init_afl_vector(&(png_prc->png_out), NULL, 0, 0, AFL_VECTOR_HARD_MAX_SIZE);

    for (int i = 0; i < CHUNK_COUNT; ++i) {
        init_chunk(&(png_prc->chunks[i]));
    }

    png_prc->chunks[PNG_CHUNK_IHDR].required = IS_REQUIRED;
    png_prc->chunks[PNG_CHUNK_IDAT].required = IS_REQUIRED;
    png_prc->chunks[PNG_CHUNK_IEND].required = IS_REQUIRED;

    return png_prc;
}

void destroy_png_processing(png_processing_t *png_prc) {
    png_destroy_write_struct(&(png_prc->png), &(png_prc->info));
    deinit_afl_vector(&(png_prc->png_out), AFL_VECTOR_STATIC);
    for (int i = 0; i < CHUNK_COUNT; ++i) {
            deinit_vector(&(png_prc->chunks[i].info), VECTOR_STATIC);
    }
    free(png_prc);
}
