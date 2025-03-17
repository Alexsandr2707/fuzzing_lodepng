#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "png_random_config.h"
#include "write_png.h"

void get_random_hw_test(void) {
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
    size+t pic_size = rand() % 1000;
    void *pic = get_random_data(pic_size);
    if (pic == NULL) 
        return;

    png_set_random_IHDR(&png_prc, pic_size);
    
}

int main() {
    size_t height, width;
    srand(time(NULL));

    
}
