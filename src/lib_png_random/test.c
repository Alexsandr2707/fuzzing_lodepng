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
    size_t pic_size = rand() % 10000000;
    void *pic = get_random_data(pic_size);
    if (pic == NULL) 
        return;
    png_processing_t *png_prc = create_png_processing(); 
    png_set_random_IHDR(png_prc, pic_size);
    print_IHDR_info(png_prc);
    
    png_write(png_prc, pic);
    
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
