#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <png.h>
#include <math.h>
#include "afl-fuzz.h"
#include "alloc-inl.h"

enum {
    MAX_HEADER_SIZE = 1000,
    MIN_DEPTH = 8,// one color of RGB
    MAX_DEPTH = 8,
    MIN_WIDTH = 1,
    MIN_HEIGHT = 1,
    MAX_WIDTH = 1000,
    MAX_HEIGHT = MAX_WIDTH,
    PIXEL_BYTE = 3,
    MIN_PIC_SIZE = MIN_WIDTH * MIN_HEIGHT * PIXEL_BYTE,
    MAX_PIC_SIZE = MAX_WIDTH * MAX_HEIGHT * PIXEL_BYTE,
    MAX_FILE_SIZE = MAX_PIC_SIZE + MAX_HEADER_SIZE, //working with RGB files
    MAX_COMPRESSION = 9,
};


enum {
    NO_VALID = 0,
    INITIALIZED = 1,
    STORE_PIC = 0b10,
    COMPRESSED = 0b100,
};

typedef struct {
    void *data;
    size_t len;
    size_t size;
    size_t max_size;
} Vector;

typedef struct png_stat_t {
    int status;
    uint8_t *pic; 
    size_t pic_size;

    size_t width;
    size_t height;
    int bit_depth;
    int color_type;
    
    Vector png_vector;
    struct afl_alloc_buf *out_buf_stat;
    uint8_t *out_buf;

    int compression_type; // 0 - 9
} png_stat_t;

void print_png_stat(png_stat_t *png_stat) {
    fprintf(stderr, "-----------------print_png_stat-----------------------\n");

    fprintf(stderr, "status %d\n", png_stat->status);
    fprintf(stderr, "pic %lu\n", png_stat->pic);
    fprintf(stderr, "pic_size %zu\n", png_stat->pic_size);
    fprintf(stderr, "width  %d\n", png_stat->width);
    fprintf(stderr, "height %d\n", png_stat->height);
    fprintf(stderr, "bit_depth %d\n", png_stat->bit_depth);
    fprintf(stderr, "color_type %d\n", png_stat->color_type);
    fprintf(stderr, "compression_type %d\n", png_stat->compression_type);
    fprintf(stderr, "png_vector len %llu\n", png_stat->png_vector.len);
    
    fprintf(stderr, "-----------------print_png_stat DONE-----------------------\n");

}

png_stat_t *afl_custom_init(void *afl) {
   // fprintf(stderr, "-----------------afl_custom_init-----------------------\n");
    png_stat_t *png_stat = (png_stat_t *) calloc(1, sizeof(png_stat_t));

    if (!png_stat) {
        perror("calloc");
        return NULL;
    }

    png_stat->pic = NULL;
    png_stat->pic_size = 0;
    png_stat->png_vector.data = malloc(MAX_FILE_SIZE);
    if (png_stat->png_vector.data == NULL) {
        fprintf(stderr, "BAD MALLOC\n");
        free(png_stat);
        return NULL;
    }
    png_stat->png_vector.len = 0;
    png_stat->png_vector.size = MAX_FILE_SIZE;
    png_stat->png_vector.max_size = MAX_FILE_SIZE;

    png_stat->out_buf_stat = calloc(1, sizeof(struct afl_alloc_buf));
    if (png_stat->out_buf_stat == NULL) {
        fprintf(stderr, "BAD CALLOC\n");
        free(png_stat->png_vector.data);
        free(png_stat);
        return NULL;
    }
        
    png_stat->out_buf_stat->complete_size = sizeof(struct afl_alloc_buf);
    png_stat->out_buf = png_stat->out_buf_stat->buf;

    png_stat->status = INITIALIZED;
    //fprintf(stderr, "-----------------afl_custom_init DONE-----------------------\n");
    return png_stat;
}

void afl_custom_deinit(png_stat_t *png_stat) {
    //fprintf(stderr, "-----------------afl_custom_deinit-----------------------\n");
    if (png_stat == NULL) 
            return;

    png_stat->status = NO_VALID;
    if (png_stat->png_vector.data)
        free(png_stat->png_vector.data);
    if (png_stat->out_buf_stat) {
        free(png_stat->out_buf_stat);
    }

    free(png_stat);
    //fprintf(stderr, "-----------------afl_custom_deinit DONE-----------------------\n");
}


ssize_t format_png(png_stat_t *png_stat, uint8_t *buf, size_t buf_size) {

    //fprintf(stderr, "-----------------format_png-----------------------\n");
    if (buf_size < MIN_PIC_SIZE || (png_stat->status & INITIALIZED) == 0) {
        return -1; 
    }
    
    png_stat->width = sqrt(buf_size / PIXEL_BYTE);
    if (png_stat->width > MAX_WIDTH)
        png_stat->width = MAX_WIDTH;

    png_stat->height = png_stat->width;
    png_stat->pic_size = png_stat->width * png_stat->height * PIXEL_BYTE;

    if (png_stat->pic_size < MIN_PIC_SIZE) {
        return -1;
    }

    png_stat->color_type = PNG_COLOR_TYPE_RGB;
    png_stat->bit_depth = MIN_DEPTH;

    png_stat->pic = buf;
    png_stat->status |= STORE_PIC;

    //fprintf(stderr, "-----------------format_png DONE-----------------------\n");
    return png_stat->pic_size;
}

int vector_write(Vector *vector, void *data, size_t len)
{
   // fprintf(stderr, "-----------------vector write-----------------------\n");
     
    if (vector->len + len > vector->size) {
        return -1;
    }

    memcpy((uint8_t *) vector->data + vector->len, data, len);
    vector->len += len;
    //fprintf(stderr, "-----------------vector write DONE-----------------------\n");
    return 0;
}

// write PNG to buffer
void custom_png_write_function(png_structp png, png_bytep data, png_size_t size) {
    Vector *vector = (Vector *) png_get_io_ptr(png);

    if (vector_write(vector, data, size) < 0) {
            fprintf(stderr, "bad write to vector in my_write_function\n");
            _exit(1);
    }
}

Vector *png_write_to_vector(png_structp png, png_infop info,  png_stat_t *png_stat,
                           png_bytep *row_pointers) {
    Vector *vector = &(png_stat->png_vector);
    vector->len = 0;

    png_set_write_fn(png, vector, custom_png_write_function, NULL);

    png_set_IHDR(png, info, png_stat->width, png_stat->height, png_stat->bit_depth,
                 PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, 
                 PNG_FILTER_TYPE_DEFAULT);
    png_write_info(png, info);

    png_write_image(png, row_pointers);
    png_write_end(png, NULL);

    return vector; 
}

size_t afl_custom_fuzz(png_stat_t *png_stat, uint8_t *buf, size_t buf_size,
                       uint8_t **out_buf, uint8_t *add_buf,
                       size_t add_buf_size,  // add_buf can be NULL
                       size_t max_size) {
//    fprintf(stderr, "-----------------afl_custom_fuzz-----------------------\n");
    if (format_png(png_stat, buf, buf_size) < 0) {
        return 0;
    }

    uint8_t *pic = png_stat->pic;
    size_t pic_size = png_stat->pic_size;
    for (size_t i = 0; i < pic_size; ++i) {
        pic[i] = (pic[i] << 1) + (rand() % 2);
    }

    *out_buf = png_stat->pic;

  //  fprintf(stderr, "-----------------afl_custom_fuzz DONE-----------------------\n");
    return buf_size;
}


// compress data 
size_t afl_custom_post_process(png_stat_t *png_stat, uint8_t *buf, size_t buf_size, uint8_t **out_buf) {
    //fprintf(stderr, "-----------------afl_custom_post_process-----------------------\n");
    if (format_png(png_stat, buf, buf_size) < 0) {
        return 0;
    }

    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png) 
        return 0;

    png_infop info = png_create_info_struct(png);
    if (!info) {
        png_destroy_write_struct(&png, NULL);
        return 0;
    }

    if (setjmp(png_jmpbuf(png))) {
        png_destroy_write_struct(&png, &info);
        return 0;
    }

    png_set_IHDR(
        png, info,
        png_stat->width, png_stat->height, png_stat->bit_depth,
        png_stat->color_type, PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT
    );

    png_bytep *row_pointers = (png_bytep*)calloc(png_stat->height, sizeof(png_bytep));
    if (!row_pointers) {
        png_destroy_write_struct(&png, &info);
        return 0;
    }

    for (size_t i = 0; i < png_stat->height; ++i) {
        row_pointers[i] = (png_bytep)((uint8_t *)png_stat->pic + i * png_stat->width * PIXEL_BYTE);
    }

    png_set_rows(png, info, row_pointers);
    png_set_compression_level(png, MAX_COMPRESSION);

    png_set_filter(png, 0, PNG_ALL_FILTERS);
    if (png_write_to_vector(png, info, png_stat, row_pointers) == NULL) {
        png_destroy_write_struct(&png, &info);
        free(row_pointers);
        return 0;
    }

    png_stat->status |= COMPRESSED;
    png_stat->compression_type = MAX_COMPRESSION;
    
    png_destroy_write_struct(&png, &info);
    free(row_pointers);

    *out_buf = png_stat->out_buf;
    size_t out_buf_size = 0;
    if (*out_buf != NULL) 
        out_buf_size = afl_alloc_bufsize(*out_buf);

    //fprintf(stderr, "out_buf_size = %llu\n", out_buf_size);

    if (out_buf_size < png_stat->png_vector.len) {
        void *new_buf = afl_realloc(out_buf,  png_stat->png_vector.len);
        if (new_buf == NULL) {
            *out_buf = NULL;
            fprintf(stderr, "bad realloc\n");
            return 0;
        }
        png_stat->out_buf_stat = afl_alloc_bufptr(new_buf);
        png_stat->out_buf = new_buf;
    }
    
    memcpy(*out_buf, png_stat->png_vector.data,  png_stat->png_vector.len);
    //print_png_stat(png_stat);
  
    //fprintf(stderr, "-----------------afl_custom_post_process DONE-----------------------\n");
    return png_stat->png_vector.len;
}

