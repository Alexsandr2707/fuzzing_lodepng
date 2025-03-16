#include <stdio.h>

#include "vector.h"

int main(void) {
    char s[] = "Hellow world!\n";
    Vector *vector = malloc_vector();
    if (vector == NULL)
        return -1;

    init_vector(vector, NULL, 0, 0, 0);
    print_vector_info(vector);
    write_to_vector(vector, s, sizeof(s));
    print_vector_info(vector);
    write_to_vector(vector, s, sizeof(s));
    print_vector_info(vector);
}

