#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <png.h>

void write_png_file(const char *filename, int width, int height, png_bytep *row_pointers) {
    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        fprintf(stderr, "Could not open file %s for writing\n", filename);
        exit(1);
    }

    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png) {
        fprintf(stderr, "Could not allocate write struct\n");
        fclose(fp);
        exit(1);
    }

    png_infop info = png_create_info_struct(png);
    if (!info) {
        fprintf(stderr, "Could not allocate info struct\n");
        png_destroy_write_struct(&png, NULL);
        fclose(fp);
        exit(1);
    }

    if (setjmp(png_jmpbuf(png))) {
        fprintf(stderr, "Error during png creation\n");
        png_destroy_write_struct(&png, &info);
        fclose(fp);
        exit(1);
    }

    png_init_io(png, fp);

    // Устанавливаем заголовок PNG
    png_set_IHDR(png, info, width, height, 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    // Создаем пользовательский чанк
    png_unknown_chunk custom_chunk;
    const char *chunk_name = "mYcH"; // Имя чанка (4 символа)
    const char *chunk_data = "This is custom chunk data!"; // Произвольные данные
    png_size_t chunk_size = strlen(chunk_data); // Размер данных

    // Копируем имя чанка
    memcpy(custom_chunk.name, chunk_name, 5); // 5 байт (4 символа + нулевой байт)
    custom_chunk.data = (png_bytep)chunk_data; // Данные чанка
    custom_chunk.size = chunk_size; // Размер данных
    custom_chunk.location = PNG_HAVE_IHDR; // Место вставки чанка (после IHDR)

    // Добавляем пользовательский чанк
    png_set_unknown_chunks(png, info, &custom_chunk, 1);

    // Записываем заголовок и чанки
    png_write_info(png, info);

    // Записываем данные изображения
    png_write_image(png, row_pointers);

    // Завершаем запись
    png_write_end(png, NULL);

    fclose(fp);
    png_destroy_write_struct(&png, &info);
}

int main() {
    const char *filename = "custom_chunk_image.png";
    int width = 640;
    int height = 480;

    // Создаем изображение (просто пример, заполняем его красным цветом)
    png_bytep row_pointers[height];
    for (int y = 0; y < height; y++) {
        row_pointers[y] = (png_bytep)malloc(3 * width);
        for (int x = 0; x < width; x++) {
            row_pointers[y][3 * x + 0] = 255; // Красный
            row_pointers[y][3 * x + 1] = 0;   // Зеленый
            row_pointers[y][3 * x + 2] = 0;   // Синий
        }
    }

    // Записываем PNG с пользовательским чанком
    write_png_file(filename, width, height, row_pointers);

    // Освобождаем память
    for (int y = 0; y < height; y++) {
        free(row_pointers[y]);
    }

    printf("PNG файл с пользовательским чанком создан: %s\n", filename);

    return 0;
}
