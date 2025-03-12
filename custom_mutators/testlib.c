#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include "afl-fuzz.h"
#include "compress_png.c"
#include <png.h>

int main(void)
{
    unsigned char pic[1000] = {};
    for (int i = 0; i < 1000; i += 3) 
        pic[i] = 255;

    void *stat = afl_custom_init(NULL);
    void *out_buf = NULL;
    size_t png_size =  afl_custom_post_process(stat, pic, 1000, (uint8_t **)&out_buf);

    print_png_stat(stat);

    int fd = open("test_png.png", O_CREAT | O_WRONLY | O_TRUNC, 0666);
    write(fd, out_buf, png_size);
    close(fd);

    afl_custom_deinit(stat);

}
