#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include "png_random.h"
#include "png_write.h"
#include "png_processing.h"

void get_random_hw_test(void) {
    size_t height, width;
    for (int i = 0; i < 10; ++i) {
        if (get_random_hw(rand() % 100, &height, &width) == 0)
            printf("%lu %lu\n", height, width);
    }
}


void *get_random_data(size_t size) {
    uint8_t *data = malloc(size);
    if (data == NULL) 
        return NULL;

    for (size_t i = 0; i < size; ++i) {
        data[i] = rand() % 256;
    }

    return data;
}

void write_png_test() {
    size_t pic_size = rand() % 1000;
    printf("pic_size = %lu\n", pic_size);

    void *pic = get_random_data(pic_size);
    if (pic == NULL) 
        return;
    png_processing_t *png_prc = create_png_processing(); 
//    if (png_config_IHDR(png_prc, pic_size) < 0) 
//        return;
    png_set_random_chunks(png_prc);

    if (png_config_chunks(png_prc, pic_size) < 0) {
        printf("bad config chunks\n");
        return ;
    }
    print_png_info(png_prc);

    png_write(png_prc, pic);
    
    print_afl_vector_info(&(png_prc->png_out));
    int fd = open("output.png", O_CREAT | O_TRUNC | O_WRONLY, 0666);

    write(fd, png_prc->png_out.data, png_prc->png_out.len);

    close(fd);

    free(pic);
    destroy_png_processing(png_prc);
}

int main() {
    srand(time(NULL));
    write_png_test();
    
}
