#include <stdio.h>

#include "afl_vector.h"

int main(void) {
    char s[] = "Hellow world!\n";
    AFL_Vector *afl_vector = malloc_afl_vector();
    if (afl_vector == NULL)
        return -1;

    if (init_afl_vector(afl_vector, NULL, 0, 0, 0) < 0)
        return -1;
    print_afl_vector_info(afl_vector);
    write_to_afl_vector(afl_vector, s, sizeof(s));
    print_afl_vector_info(afl_vector);
    write_to_afl_vector(afl_vector, s, sizeof(s));
    print_afl_vector_info(afl_vector);
    printf("%s%s\n", afl_vector->data, afl_vector->data + sizeof(s));
}

