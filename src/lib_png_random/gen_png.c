#include <stdio.h>
#include <stdlib.h>
#include <png.h>

#define WIDTH 256
#define HEIGHT 256
#define OUTPUT_FILE "red_alpha.png"

void write_png(const char* filename, int width, int height) {
    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        fprintf(stderr, "Could not open file %s for writing\n", filename);
        return;
    }

    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr) {
        fclose(fp);
        return;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_write_struct(&png_ptr, NULL);
        fclose(fp);
        return;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        fclose(fp);
        return;
    }

    png_init_io(png_ptr, fp);

    // Настройка параметров изображения (RGBA с 8-битным альфа-каналом)
    png_set_IHDR(png_ptr, info_ptr,
                 width, height,
                 8,                      // 8 бит на канал
                 PNG_COLOR_TYPE_RGBA,    // RGBA (с альфа-каналом)
                 PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_DEFAULT,
                 PNG_FILTER_TYPE_DEFAULT);

     png_color_16 background;
    background.red = 0x00;   // Красный
    background.green = 0x00; // Зеленый
    background.blue = 0xFF;  // Синий
    png_set_bKGD(png_ptr, info_ptr, &background);

    // Создаем массив данных пикселей
    png_bytep row_pointers[HEIGHT];
    for (int y = 0; y < height; y++) {
        row_pointers[y] = (png_bytep)malloc(width * 4); // 4 байта на пиксель (RGBA)
        
        for (int x = 0; x < width * 4; x += 4) {
            // Красный цвет (R=255, G=0, B=0)
            row_pointers[y][x]   = 0xFF; // Red
            row_pointers[y][x+1] = 0x00; // Green
            row_pointers[y][x+2] = 0x00; // Blue
            
            // Альфа-канал (0 = прозрачный, 255 = непрозрачный)
            // Здесь делаем градиент прозрачности сверху вниз
            row_pointers[y][x+3] = y; // Alpha (0-255)
        }
    }

    png_set_rows(png_ptr, info_ptr, row_pointers);
    png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

    // Очистка
    for (int y = 0; y < height; y++) {
        free(row_pointers[y]);
    }
    
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(fp);
}

int main() {
    write_png(OUTPUT_FILE, WIDTH, HEIGHT);
    printf("PNG image created: %s\n", OUTPUT_FILE);
    return 0;
}
