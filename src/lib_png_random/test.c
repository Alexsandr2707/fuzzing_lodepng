#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include "png_random.h"
#include "png_print.h"
#include "png_io.h"
#include "png_processing.h"

void get_random_hw_test(void) {
    size_t height, width;
    for (int i = 0; i < 10; ++i) {
        if (get_random_hw(rand() % 100, &height, &width) == 0)
            printf("%lu %lu\n", height, width);
    }
}


void write_png_test() {
    make_randomize(); 

    png_processing_t *png_prc = create_png_processing(); 
    if (make_random_png(png_prc, NULL, 10 + rand() % 100) < 0) {
        printf("Bad make png\n");
        return ;
    }

    print_png_info(png_prc);

    print_afl_vector_info(&(png_prc->png_out));
    int fd = open("output_creat.png", O_CREAT | O_TRUNC | O_WRONLY, 0666);

    write(fd, png_prc->png_out.data, png_prc->png_out.len);

    close(fd);

    printf("PNG is writed\n");

    destroy_png_processing(png_prc);
}

void read_png_test() {
    int fd = open("output_creat.png", O_CREAT | O_RDONLY, 0666);
    uint8_t *data = NULL;
    size_t data_size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);

    data = malloc(data_size);
    read(fd, data, data_size);

    close(fd);

    png_processing_t *png_prc = create_png_processing();

    png_set_random_chunks(png_prc);
    png_remove_random_chunks(png_prc);
    png_clone_random_chunks(png_prc);

    if (png_read(png_prc, data, data_size)) 
        return;
    
    
    png_config_chunks(png_prc, 0);

    png_write(png_prc, NULL);
    print_png_info(png_prc);

    print_afl_vector_info(&(png_prc->png_out));
    fd = open("output.png", O_CREAT | O_TRUNC | O_WRONLY, 0666);

    write(fd, png_prc->png_out.data, png_prc->png_out.len);

    close(fd);

    printf("PNG is writed\n");

    destroy_png_processing(png_prc);
    
}

int main() {
    fprintf(stderr, "run testing\n");
    write_png_test();
    read_png_test();
}
