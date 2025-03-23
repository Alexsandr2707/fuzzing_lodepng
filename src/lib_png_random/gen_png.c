#include <stdio.h>
#include <stdlib.h>
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

    // Создаем текстовый чанк
    png_text text_chunk;
    text_chunk.compression = PNG_TEXT_COMPRESSION_NONE; // Для tEXT
    text_chunk.key = "Title";                          // Ключ
    text_chunk.text = "Example PNG Image";             // Текст

    // Добавляем текстовый чанк в PNG
    png_set_text(png, info, &text_chunk, 1);

    // После вызова png_set_text структура text_chunk больше не нужна
    // Можно освободить память, если она была выделена динамически

    // Записываем заголовок и текстовые чанки
    png_write_info(png, info);

    // Записываем данные изображения
    png_write_image(png, row_pointers);

    // Завершаем запись
    png_write_end(png, NULL);

    fclose(fp);
    png_destroy_write_struct(&png, &info);
}

int main() {
    const char *filename = "text_image.png";
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

    // Записываем PNG с текстовым чанком
    write_png_file(filename, width, height, row_pointers);

    // Освобождаем память
    for (int y = 0; y < height; y++) {
        free(row_pointers[y]);
    }

    printf("PNG file with text chunk created: %s\n", filename);
    return 0;
}
