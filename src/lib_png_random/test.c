#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "png_random_config.h"

int main() {
    size_t height, width;
    srand(time(NULL));

    for (int i = 0; i < 10; ++i) {
        if (get_random_hw(rand() % 100, &height, &width) == 0)
            printf("%lu %lu\n", height, width);
    }
}
