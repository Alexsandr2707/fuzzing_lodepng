#include <png.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include "png_io.h"
#include "png_processing.h"
#include "png_random.h"
#include "afl_vector.h"

enum {
    PNG_SIG_SIZE = 8,

    PNG_CHUNK_LEN_SIZE = 4,
    PNG_CHUNK_NAME_SIZE = 4,
    PNG_CHUNK_CRC_SIZE = 4,

    PNG_CHUNK_INFO_SIZE = PNG_CHUNK_LEN_SIZE + PNG_CHUNK_NAME_SIZE + PNG_CHUNK_CRC_SIZE,
};

const int SAFE_COPY_CHUNKS[] = {
//    PNG_CHUNK_tRNS,
//    PNG_CHUNK_cHRM,
//    PNG_CHUNK_gAMA,
//    PNG_CHUNK_sBIT,
//    PNG_CHUNK_sRGB,
//    PNG_CHUNK_iCCP,
      PNG_CHUNK_tEXt,
      PNG_CHUNK_zTXt,
      PNG_CHUNK_iTXt,
//    PNG_CHUNK_bKGD,
//    PNG_CHUNK_hIST, 
      PNG_CHUNK_cSTM,
//    PNG_CHUNK_pHYs,
      PNG_CHUNK_sPLT,
//    PNG_CHUNK_tIME,
};

enum {
    SAFE_COPY_CHUNKS_SIZE = sizeof(SAFE_COPY_CHUNKS) / sizeof(SAFE_COPY_CHUNKS[0]),
};

typedef struct {
    const unsigned char *data;
    size_t size;
    size_t offset;
} MemoryReaderState;

int is_safe_copy(int type) {
    for(int i = 0; i < SAFE_COPY_CHUNKS_SIZE; ++i) {
       if (SAFE_COPY_CHUNKS[i] == type) 
           return 1;
    }

    return 0;
}

int png_get_offset_info(png_processing_t *png_prc, png_bytep file, size_t file_size) {
    size_t chunk_offset = PNG_SIG_SIZE;
    uint32_t chunk_length = 0;
    char name[PNG_CHUNK_NAME_SIZE + 1] = {};
    int chunk_type = 0;

    while (chunk_offset + PNG_CHUNK_INFO_SIZE <= file_size && 
          (chunk_length = *(uint32_t *) (file + chunk_offset))) {
        strncpy((char *)&name, (char *)file + chunk_offset + PNG_CHUNK_LEN_SIZE, PNG_CHUNK_NAME_SIZE);
        chunk_type = png_get_chunk_type((char *)&name);
        chunk_length = ntohl(chunk_length);
        if (chunk_type < 0) {
            chunk_offset += chunk_length + PNG_CHUNK_INFO_SIZE;
            continue;
        }

        png_prc->chunks[chunk_type].file_pointer = file + chunk_offset;
        png_prc->chunks[chunk_type].file_size = chunk_length + PNG_CHUNK_INFO_SIZE;
        chunk_offset += chunk_length + PNG_CHUNK_INFO_SIZE;
    }

    strncpy((char *)&name, (char *)file + chunk_offset + PNG_CHUNK_LEN_SIZE, PNG_CHUNK_NAME_SIZE);
    chunk_type = png_get_chunk_type((char *)&name);

    if (chunk_length == 0 && chunk_type != PNG_CHUNK_IEND)
        return -1;

    chunk_length = ntohl(chunk_length);

    png_prc->chunks[chunk_type].file_pointer = file + chunk_offset;
    png_prc->chunks[chunk_type].file_size = chunk_length + PNG_CHUNK_INFO_SIZE;

    return 0;
}

int copy_IHDR_info(png_structp wpng_ptr, png_infop winfo_ptr, 
                  const png_structp rpng_ptr, const png_infop rinfo_ptr) {
    if (!wpng_ptr || !winfo_ptr || !rpng_ptr || !rinfo_ptr) {
        return -1; 
    }

    png_uint_32 width = png_get_image_width(rpng_ptr, rinfo_ptr);
    png_uint_32 height = png_get_image_height(rpng_ptr, rinfo_ptr);
    int bit_depth = png_get_bit_depth(rpng_ptr, rinfo_ptr);
    int color_type = png_get_color_type(rpng_ptr, rinfo_ptr);
    int interlace_type = png_get_interlace_type(rpng_ptr, rinfo_ptr);
    int compression_type = png_get_compression_type(rpng_ptr, rinfo_ptr);
    int filter_type = png_get_filter_type(rpng_ptr, rinfo_ptr);

    png_set_IHDR(wpng_ptr, winfo_ptr,
                 width, height,
                 bit_depth, color_type,
                 interlace_type,
                 compression_type,
                 filter_type);

    return 0; 
}

void free_row_pointers(png_processing_t *png_prc) {
    png_uint_32 width, height;
    int bit_depth, color_type;
    png_get_IHDR(png_prc->png, png_prc->info, &width, &height, &bit_depth,
                 &color_type, NULL, NULL, NULL);

    png_bytepp rows = png_get_rows(png_prc->png, png_prc->info);
    for (png_uint_32 y = 0; y < height; y++) {
        png_free(png_prc->png, rows[y]);
    }
    png_free(png_prc->png, rows);
}

int copy_IDAT_info(png_structp wpng_ptr, png_infop winfo_ptr, 
                  const png_structp rpng_ptr, const png_infop rinfo_ptr) {
    if (!wpng_ptr || !winfo_ptr || !rpng_ptr || !rinfo_ptr) {
        return -1; 
    }
     png_uint_32 width, height;
    int bit_depth, color_type;
    png_get_IHDR(rpng_ptr, rinfo_ptr, &width, &height, &bit_depth,
                &color_type, NULL, NULL, NULL);

    png_bytepp row_pointers = png_get_rows(rpng_ptr, rinfo_ptr);
    png_bytepp new_rows = (png_bytepp)png_malloc(wpng_ptr,
        height * sizeof(png_bytep));

    size_t bytes_per_row = png_get_rowbytes(rpng_ptr, rinfo_ptr);

    for (png_uint_32 y = 0; y < height; y++) {
        new_rows[y] = (png_bytep)png_malloc(wpng_ptr, bytes_per_row);
        memcpy(new_rows[y], row_pointers[y], bytes_per_row);
    }

    png_set_rows(wpng_ptr, winfo_ptr, new_rows);

    return 0; 
}

int copy_bKGD_info(png_structp wpng_ptr, png_infop winfo_ptr,
                  const png_structp rpng_ptr, const png_infop rinfo_ptr) {
    png_color_16p background;
    
    if (!png_get_bKGD(rpng_ptr, rinfo_ptr, &background)) {
        return -1; 
    }

    png_set_bKGD(wpng_ptr, winfo_ptr, background);
    
    return 0;
}

int copy_cHRM_info(png_structp wpng_ptr, png_infop winfo_ptr,
                  const png_structp rpng_ptr, const png_infop rinfo_ptr) {
    double white_x, white_y;
    double red_x, red_y;
    double green_x, green_y;
    double blue_x, blue_y;

    if (!png_get_cHRM(rpng_ptr, rinfo_ptr,
                     &white_x, &white_y,
                     &red_x, &red_y,
                     &green_x, &green_y,
                     &blue_x, &blue_y)) {
        return -1; 
    }

    png_set_cHRM(wpng_ptr, winfo_ptr,
                 white_x, white_y,
                 red_x, red_y,
                 green_x, green_y,
                 blue_x, blue_y);
    
    return 0;
}

int copy_gAMA_info(png_structp wpng_ptr, png_infop winfo_ptr,
                  const png_structp rpng_ptr, const png_infop rinfo_ptr) {
    double gamma;

    if (!png_get_gAMA(rpng_ptr, rinfo_ptr, &gamma)) {
        return -1; 
    }

    png_set_gAMA(wpng_ptr, winfo_ptr, gamma);

    return 0; 
}

int copy_iCCP_info(png_structp wpng_ptr, png_infop winfo_ptr,
                  const png_structp rpng_ptr, const png_infop rinfo_ptr) {
    png_charp name;
    int compression_type;
    png_bytep profile;
    png_uint_32 proflen;

    if (!png_get_iCCP(rpng_ptr, rinfo_ptr, &name, &compression_type, &profile, &proflen)) {
        return -1; 
    }

    png_charp new_name = png_malloc(wpng_ptr, strlen(name) + 1);
    if (!new_name) {
        return -1;
    }
    strcpy(new_name, name);

    png_bytep new_profile = png_malloc(wpng_ptr, proflen);
    if (!new_profile) {
        png_free(wpng_ptr, new_name);
        return -1; 
    }
    memcpy(new_profile, profile, proflen);

    png_set_iCCP(wpng_ptr, winfo_ptr, new_name, compression_type, new_profile, proflen);

    png_free(wpng_ptr, new_name);
    png_free(wpng_ptr, new_profile);

    return 0; 
}

int copy_iTXt_info(png_structp wpng_ptr, png_infop winfo_ptr,
                  const png_structp rpng_ptr, const png_infop rinfo_ptr) {
    png_textp text_ptr;
    int num_text = 0;

    if (png_get_text(rpng_ptr, rinfo_ptr, &text_ptr, &num_text) <= 0) {
        return -1; 
    }

    int success = 0;
    
    for (int i = 0; i < num_text; i++) {
        if (text_ptr[i].compression == PNG_ITXT_COMPRESSION_NONE ||
            text_ptr[i].compression == PNG_ITXT_COMPRESSION_zTXt) {
            
            png_text copy;
            memset(&copy, 0, sizeof(copy));
            
            copy.compression = text_ptr[i].compression;
            copy.key = png_malloc(wpng_ptr, strlen(text_ptr[i].key) + 1);
            strcpy((char*)copy.key, text_ptr[i].key);
            
            if (text_ptr[i].text) {
                copy.text = png_malloc(wpng_ptr, strlen(text_ptr[i].text) + 1);
                strcpy((char*)copy.text, text_ptr[i].text);
            }
            
            if (text_ptr[i].lang) {
                copy.lang = png_malloc(wpng_ptr, strlen(text_ptr[i].lang) + 1);
                strcpy((char*)copy.lang, text_ptr[i].lang);
            }
            
            if (text_ptr[i].lang_key) {
                copy.lang_key = png_malloc(wpng_ptr, strlen(text_ptr[i].lang_key) + 1);
                strcpy((char*)copy.lang_key, text_ptr[i].lang_key);
            }
            
            png_set_text(wpng_ptr, winfo_ptr, &copy, 1);
            
            png_free(wpng_ptr, (void*)copy.key);
            if (copy.text) png_free(wpng_ptr, (void*)copy.text);
            if (copy.lang) png_free(wpng_ptr, (void*)copy.lang);
            if (copy.lang_key) png_free(wpng_ptr, (void*)copy.lang_key);
            
            success = 1; 
        }
    }

    return success ? 0 : -1;
}

int copy_tEXt_info(png_structp wpng_ptr, png_infop winfo_ptr,
                    const png_structp rpng_ptr, const png_infop rinfo_ptr) {
    png_textp text_ptr;
    int num_text = 0;

    if (png_get_text(rpng_ptr, rinfo_ptr, &text_ptr, &num_text) <= 0) {
        return -1; 
    }

    int success = 0;
    
    for (int i = 0; i < num_text; i++) {
        if (text_ptr[i].compression == PNG_ITXT_COMPRESSION_NONE ||
            text_ptr[i].compression == PNG_ITXT_COMPRESSION_zTXt) {
            continue;
        }

        png_text copy;
        memset(&copy, 0, sizeof(copy));
        
        copy.compression = text_ptr[i].compression;
        
        copy.key = png_malloc(wpng_ptr, strlen(text_ptr[i].key) + 1);
        strcpy((char*)copy.key, text_ptr[i].key);
        
        if (text_ptr[i].compression == PNG_TEXT_COMPRESSION_zTXt) {
            copy.text_length = text_ptr[i].text_length;
            copy.text = png_malloc(wpng_ptr, text_ptr[i].text_length);
            memcpy((void*)copy.text, text_ptr[i].text, text_ptr[i].text_length);
        } else if (text_ptr[i].text) {
            copy.text = png_malloc(wpng_ptr, strlen(text_ptr[i].text) + 1);
            strcpy((char*)copy.text, text_ptr[i].text);
        }
        
        png_set_text(wpng_ptr, winfo_ptr, &copy, 1);
        
        png_free(wpng_ptr, (void*)copy.key);
        if (copy.text) png_free(wpng_ptr, (void*)copy.text);
        
        success = 1;
    }

    return success ? 0 : -1;
}

int copy_pHYs_info(png_structp wpng_ptr, png_infop winfo_ptr,
                  const png_structp rpng_ptr, const png_infop rinfo_ptr) {
    png_uint_32 res_x, res_y;
    int unit_type;

    if (!png_get_pHYs(rpng_ptr, rinfo_ptr, &res_x, &res_y, &unit_type)) {
        return -1; 
    }

    png_set_pHYs(wpng_ptr, winfo_ptr, res_x, res_y, unit_type);
    
    return 0; 
}

int copy_sBIT_info(png_structp wpng_ptr, png_infop winfo_ptr,
                  const png_structp rpng_ptr, const png_infop rinfo_ptr) {
    png_color_8p sig_bit;

    if (!png_get_sBIT(rpng_ptr, rinfo_ptr, &sig_bit)) {
        return -1; 
    }

    png_set_sBIT(wpng_ptr, winfo_ptr, sig_bit);
    
    return 0; 
}

int copy_PLTE_info(png_structp wpng_ptr, png_infop winfo_ptr, Vector *vector,
                       const png_structp rpng_ptr, const png_infop rinfo_ptr) {
    png_colorp src_palette;
    int num_entries;

    if (!png_get_PLTE(rpng_ptr, rinfo_ptr, &src_palette, &num_entries)) {
        return -1; 
    }
    
    write_to_vector(vector, src_palette, num_entries * sizeof(png_color));

    png_set_PLTE(wpng_ptr, winfo_ptr, src_palette, num_entries);

    return 0;
}

int copy_sPLT_info(png_structp wpng_ptr, png_infop winfo_ptr,
                  const png_structp rpng_ptr, const png_infop rinfo_ptr) {
    png_sPLT_tp palette;
    int num_palette = 0;

    if ((num_palette = png_get_sPLT(rpng_ptr, rinfo_ptr, &palette)) <= 0) {
        return -1; 
    }

    for (int i = 0; i < num_palette; i++) {
        png_sPLT_t new_palette;
        memset(&new_palette, 0, sizeof(new_palette));

        new_palette.name = png_malloc(wpng_ptr, strlen(palette[i].name) + 1);
        strcpy((char*)new_palette.name, palette[i].name);

        new_palette.depth = palette[i].depth;

        new_palette.entries = png_malloc(wpng_ptr, palette[i].nentries * sizeof(png_sPLT_entry));
        new_palette.nentries = palette[i].nentries;

        memcpy(new_palette.entries, palette[i].entries, 
              palette[i].nentries * sizeof(png_sPLT_entry));

        png_set_sPLT(wpng_ptr, winfo_ptr, &new_palette, 1);

        png_free(wpng_ptr, (void*)new_palette.name);
        png_free(wpng_ptr, (void*)new_palette.entries);
    }

    return 0; 
}

int copy_sRGB_info(png_structp wpng_ptr, png_infop winfo_ptr,
                  const png_structp rpng_ptr, const png_infop rinfo_ptr) {
    int intent;

    if (!png_get_sRGB(rpng_ptr, rinfo_ptr, &intent)) {
        return -1; 
    }

    png_set_sRGB(wpng_ptr, winfo_ptr, intent);
    
    return 0; 
}

int copy_tIME_info(png_structp wpng_ptr, png_infop winfo_ptr,
                  const png_structp rpng_ptr, const png_infop rinfo_ptr) {
    png_timep mod_time;

    if (!png_get_tIME(rpng_ptr, rinfo_ptr, &mod_time)) {
        return -1; 
    }

    png_set_tIME(wpng_ptr, winfo_ptr, mod_time);
    
    return 0; 
}

int copy_tRNS_info(png_structp wpng_ptr, png_infop winfo_ptr,
                  const png_structp rpng_ptr, const png_infop rinfo_ptr) {
    png_bytep trans_alpha = NULL;
    int num_trans = 0;
    png_color_16p trans_color = NULL;

    int color_type = png_get_color_type(rpng_ptr, rinfo_ptr);

    if (color_type == PNG_COLOR_TYPE_PALETTE) {
        if (!png_get_tRNS(rpng_ptr, rinfo_ptr, &trans_alpha, &num_trans, NULL)) {
            return -1;
        }

        png_bytep new_trans = png_malloc(wpng_ptr, num_trans);
        memcpy(new_trans, trans_alpha, num_trans);
        png_set_tRNS(wpng_ptr, winfo_ptr, new_trans, num_trans, NULL);

    } else if (color_type == PNG_COLOR_TYPE_RGB ||
               color_type == PNG_COLOR_TYPE_GRAY) {
        if (!png_get_tRNS(rpng_ptr, rinfo_ptr, NULL, NULL, &trans_color)) {
            return -1;
        }

        png_color_16p new_trans_color = png_malloc(wpng_ptr, sizeof(png_color_16));
        memcpy(new_trans_color, trans_color, sizeof(png_color_16));
        png_set_tRNS(wpng_ptr, winfo_ptr, NULL, 0, new_trans_color);
    } else {
        return -1; 
    }

    return 0;
}

int copy_unknown_chunks(png_structp wpng_ptr, png_infop winfo_ptr,
                      const png_structp rpng_ptr, const png_infop rinfo_ptr) {
    png_unknown_chunkp unknowns;
    int num_unknowns = 0;

    num_unknowns = png_get_unknown_chunks(rpng_ptr, rinfo_ptr, &unknowns);
    if (num_unknowns <= 0) {
        return -1; 
    }

    for (int i = 0; i < num_unknowns; i++) {
        png_unknown_chunk new_chunk;
        
        memcpy(new_chunk.name, unknowns[i].name, 5);
        
        new_chunk.data = png_malloc(wpng_ptr, unknowns[i].size);
        memcpy(new_chunk.data, unknowns[i].data, unknowns[i].size);
        new_chunk.size = unknowns[i].size;
        
        new_chunk.location = unknowns[i].location;
        
        png_set_unknown_chunks(wpng_ptr, winfo_ptr, &new_chunk, 1);
        
        png_set_unknown_chunk_location(wpng_ptr, winfo_ptr, 0, new_chunk.location);
        
        png_free(wpng_ptr, new_chunk.data);
    }

    return 0;
}



// copy all info from png_ptr and info_ptr
int copy_chunk_info(png_processing_t *png_prc, const png_structp png_ptr, const png_infop info_ptr) {
    if (!png_prc || !png_ptr || !info_ptr)
        return -1;

    if (copy_IHDR_info(png_prc->png, png_prc->info, png_ptr, info_ptr) < 0) {
        printf("copy IHDR error\n");
        return -1;
    }
    png_prc->chunks[PNG_CHUNK_IHDR].required = IS_REQUIRED;
    png_prc->chunks[PNG_CHUNK_IHDR].valid = IS_VALID;

    int color_type = png_get_color_type(png_prc->png, png_prc->info);
    if (color_type == PNG_COLOR_TYPE_PALETTE)
        png_prc->chunks[PNG_CHUNK_PLTE].required = IS_REQUIRED;


    if (copy_IDAT_info(png_prc->png, png_prc->info, png_ptr, info_ptr) < 0)
        return -1;

    png_prc->chunks[PNG_CHUNK_IDAT].required = IS_REQUIRED;
    png_prc->chunks[PNG_CHUNK_IDAT].valid = IS_VALID;

    if (png_prc->chunks[PNG_CHUNK_bKGD].required != NOT_REQUIRED &&
        copy_bKGD_info(png_prc->png, png_prc->info, png_ptr, info_ptr) == 0) {
        png_prc->chunks[PNG_CHUNK_bKGD].required = IS_REQUIRED;
        png_prc->chunks[PNG_CHUNK_bKGD].valid = IS_VALID;
    }

    if (png_prc->chunks[PNG_CHUNK_cHRM].required != NOT_REQUIRED &&
        copy_cHRM_info(png_prc->png, png_prc->info, png_ptr, info_ptr) == 0) {
        png_prc->chunks[PNG_CHUNK_cHRM].required = IS_REQUIRED;
        png_prc->chunks[PNG_CHUNK_cHRM].valid = IS_VALID;
    }

    if (png_prc->chunks[PNG_CHUNK_gAMA].required != NOT_REQUIRED &&
        copy_gAMA_info(png_prc->png, png_prc->info, png_ptr, info_ptr) == 0) {
      png_prc->chunks[PNG_CHUNK_gAMA].required = IS_REQUIRED;
      png_prc->chunks[PNG_CHUNK_gAMA].valid = IS_VALID;
    }

    if (png_prc->chunks[PNG_CHUNK_iCCP].required != NOT_REQUIRED &&
        copy_iCCP_info(png_prc->png, png_prc->info, png_ptr, info_ptr) == 0) {
      png_prc->chunks[PNG_CHUNK_iCCP].required = IS_REQUIRED;
      png_prc->chunks[PNG_CHUNK_iCCP].valid = IS_VALID;
    }

    if (png_prc->chunks[PNG_CHUNK_iTXt].required != NOT_REQUIRED &&
        copy_iTXt_info(png_prc->png, png_prc->info, png_ptr, info_ptr) == 0) {
      png_prc->chunks[PNG_CHUNK_iTXt].required = IS_REQUIRED;
      png_prc->chunks[PNG_CHUNK_iTXt].valid = IS_VALID;
    }

    if (png_prc->chunks[PNG_CHUNK_tEXt].required != NOT_REQUIRED &&
        copy_tEXt_info(png_prc->png, png_prc->info, png_ptr, info_ptr) == 0) {
      png_prc->chunks[PNG_CHUNK_tEXt].required = IS_REQUIRED;
      png_prc->chunks[PNG_CHUNK_tEXt].valid = IS_VALID;
    }

    if (png_prc->chunks[PNG_CHUNK_pHYs].required != NOT_REQUIRED &&
        copy_pHYs_info(png_prc->png, png_prc->info, png_ptr, info_ptr) == 0) {
      png_prc->chunks[PNG_CHUNK_pHYs].required = IS_REQUIRED;
      png_prc->chunks[PNG_CHUNK_pHYs].valid = IS_VALID;
    }

    if (png_prc->chunks[PNG_CHUNK_sBIT].required != NOT_REQUIRED &&
        copy_sBIT_info(png_prc->png, png_prc->info, png_ptr, info_ptr) == 0) {
      png_prc->chunks[PNG_CHUNK_sBIT].required = IS_REQUIRED;
      png_prc->chunks[PNG_CHUNK_sBIT].valid = IS_VALID;
    }

    if (png_prc->chunks[PNG_CHUNK_sPLT].required != NOT_REQUIRED &&
        copy_sPLT_info(png_prc->png, png_prc->info, png_ptr, info_ptr) == 0) {
      png_prc->chunks[PNG_CHUNK_sPLT].required = IS_REQUIRED;
      png_prc->chunks[PNG_CHUNK_sPLT].valid = IS_VALID;
    }

    if (png_prc->chunks[PNG_CHUNK_PLTE].required != NOT_REQUIRED &&
        copy_PLTE_info(png_prc->png, png_prc->info, 
        &(png_prc->chunks[PNG_CHUNK_PLTE].info),
        png_ptr, info_ptr) == 0) {
      png_prc->chunks[PNG_CHUNK_PLTE].required = IS_REQUIRED;
      png_prc->chunks[PNG_CHUNK_PLTE].valid = IS_VALID;
    }

    if (png_prc->chunks[PNG_CHUNK_sRGB].required != NOT_REQUIRED &&
        copy_sRGB_info(png_prc->png, png_prc->info, png_ptr, info_ptr) == 0) {
      png_prc->chunks[PNG_CHUNK_sRGB].required = IS_REQUIRED;
      png_prc->chunks[PNG_CHUNK_sRGB].valid = IS_VALID;
    }

    if (png_prc->chunks[PNG_CHUNK_tIME].required != NOT_REQUIRED &&
copy_tIME_info(png_prc->png, png_prc->info, png_ptr, info_ptr) == 0) {
      png_prc->chunks[PNG_CHUNK_tIME].required = IS_REQUIRED;
      png_prc->chunks[PNG_CHUNK_tIME].valid = IS_VALID;
    }

    if (png_prc->chunks[PNG_CHUNK_tRNS].required != NOT_REQUIRED &&
copy_tRNS_info(png_prc->png, png_prc->info, png_ptr, info_ptr) == 0) {
      png_prc->chunks[PNG_CHUNK_tRNS].required = IS_REQUIRED;
      png_prc->chunks[PNG_CHUNK_tRNS].valid = IS_VALID;
    }
    
    return 0;

}

void png_read_memory_data(png_structp png_ptr, png_bytep out, png_size_t length) {
    MemoryReaderState *state = (MemoryReaderState*)png_get_io_ptr(png_ptr);
    
    if (state->offset + length > state->size) {
        return;
    }
    
    memcpy(out, state->data + state->offset, length);
    state->offset += length;
}


int png_read(png_processing_t *png_prc, uint8_t *file, size_t file_size) {

    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr) {
        return -1;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        return -1;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        return -1;
    }

    MemoryReaderState state;
    state.data = file;
    state.size = file_size;
    state.offset = 0;

    png_set_read_fn(png_ptr, &state, png_read_memory_data);

    png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

    if (copy_chunk_info(png_prc, png_ptr, info_ptr) < 0) {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        return -1;
    }

    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    return 0;
}


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

int clone_chunk(AFLVector *vector, png_processing_t *png_prc, int type) {
    uint8_t *file_pointer = png_prc->chunks[type].file_pointer;
    size_t file_size = png_prc->chunks[type].file_size;

    if (write_to_afl_vector(vector, file_pointer, file_size) < 0)
        return -1;

    return 0;
}

int png_make_clones(png_processing_t *png_prc) {
    if (png_prc == NULL)
        return -1;

    png_get_offset_info(png_prc, png_prc->png_out.data, png_prc->png_out.len);

    AFLVector new_png_out;
    init_afl_vector(&new_png_out, NULL, 0, 0, 0);
    
    write_to_afl_vector(&new_png_out, png_prc->png_out.data, PNG_SIG_SIZE);

    clone_chunk(&new_png_out, png_prc, PNG_CHUNK_IHDR);

    for (int i = BASE_CHUNK_COUNT; i < CHUNK_COUNT; ++i) {
        if (png_prc->chunks[i].valid == IS_VALID && 
            png_prc->chunks[i].required == IS_REQUIRED) {
            clone_chunk(&new_png_out, png_prc, i);
        }
    }

    for (int i = 0; i < SAFE_COPY_CHUNKS_SIZE; ++i) {
        int type = SAFE_COPY_CHUNKS[i];

        if (png_prc->chunks[type].valid == IS_VALID && 
            png_prc->chunks[type].required == IS_REQUIRED &&
            png_prc->chunks[type].cloned == IS_CLONED) {
            clone_chunk(&new_png_out, png_prc, type);
        }
    }

    clone_chunk(&new_png_out, png_prc, PNG_CHUNK_IDAT);
    clone_chunk(&new_png_out, png_prc, PNG_CHUNK_IEND);
    
    deinit_afl_vector(&(png_prc->png_out), AFL_VECTOR_STATIC);
    png_prc->png_out = new_png_out;

    return 0;
}

int png_write(png_processing_t *png_prc, png_bytep pic) {
    AFLVector *vector = &(png_prc->png_out);
    vector->len = 0;

    png_bytep *row_pointers = NULL;

    png_set_write_fn(png_prc->png, vector, png_write_to_afl_vector, NULL);

    if (pic) {
        row_pointers = make_row_pointers(png_prc, pic);
        if (row_pointers == NULL)
            return -1;

        png_write_info(png_prc->png, png_prc->info);
        png_write_image(png_prc->png, row_pointers);
        png_write_end(png_prc->png, NULL);
        free(row_pointers);
    } else {
        png_write_png(png_prc->png, png_prc->info, PNG_TRANSFORM_IDENTITY, NULL);
        free_row_pointers(png_prc);

        png_make_clones(png_prc);
    }

    return 0;
}
