#include <png.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

void abort_with_error(const char *message) {
    fprintf(stderr, "Error: %s\n", message);
    abort();
}

png_bytep* create_sample_image(int width, int height) {
    png_bytep* row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * height);
    for (int y = 0; y < height; y++) {
        row_pointers[y] = (png_byte*)malloc(width * 3);
        for (int x = 0; x < width; x++) {
            row_pointers[y][x * 3 + 0] = (x + y) % 256;  // R
            row_pointers[y][x * 3 + 1] = x % 256;          // G
            row_pointers[y][x * 3 + 2] = y % 256;          // B
        }
    }
    return row_pointers;
}

void free_image(png_bytep* row_pointers, int height) {
    for (int y = 0; y < height; y++) {
        free(row_pointers[y]);
    }
    free(row_pointers);
}

png_byte* load_icc_profile(const char* path, png_uint_32* length) {
    FILE* f = fopen(path, "rb");
    if (!f) return NULL;
    
    fseek(f, 0, SEEK_END);
    *length = (png_uint_32)ftell(f);
    rewind(f);
    
    png_byte* data = (png_byte*)malloc(*length);
    if (!data) {
        fclose(f);
        return NULL;
    }
    
    if (fread(data, 1, *length, f) != *length) {
        free(data);
        fclose(f);
        return NULL;
    }
    
    fclose(f);
    return data;
}

void write_png_with_icc(const char* filename, int width, int height, 
                       png_bytep* row_pointers, const char* icc_path) {
    // Загружаем ICC профиль
    png_uint_32 icc_length;
    png_byte* icc_data = load_icc_profile(icc_path, &icc_length);
    if (!icc_data) {
        abort_with_error("Failed to load ICC profile");
    }

    FILE* fp = fopen(filename, "wb");
    if (!fp) {
        free(icc_data);
        abort_with_error("Could not open file for writing");
    }

    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png) {
        free(icc_data);
        fclose(fp);
        abort_with_error("Could not allocate write struct");
    }

    png_infop info = png_create_info_struct(png);
    if (!info) {
        free(icc_data);
        fclose(fp);
        png_destroy_write_struct(&png, NULL);
        abort_with_error("Could not allocate info struct");
    }

    if (setjmp(png_jmpbuf(png))) {
        free(icc_data);
        fclose(fp);
        png_destroy_write_struct(&png, &info);
        abort_with_error("Error during PNG creation");
    }

    png_init_io(png, fp);

    // Настройка изображения
    png_set_IHDR(png, info, width, height, 8, PNG_COLOR_TYPE_RGB,
                PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, 
                PNG_FILTER_TYPE_DEFAULT);

    // Добавляем ICC профиль
    png_set_iCCP(png, info, "sRGB IEC61966-2.1", 
                PNG_COMPRESSION_TYPE_BASE, icc_data, icc_length);

    png_write_info(png, info);
    png_write_image(png, row_pointers);
    png_write_end(png, NULL);

    free(icc_data);
    fclose(fp);
    png_destroy_write_struct(&png, &info);
}

int main() {
    const int width = 256;
    const int height = 256;
    
    // Создаем тестовое изображение
    png_bytep* image = create_sample_image(width, height);
    
    // Сохраняем PNG с ICC профилем
    write_png_with_icc("output.png", width, height, image, "sRGB.icc");
    
    // Очищаем память
    free_image(image, height);
    
    printf("PNG with ICC profile created successfully!\n");
    return 0;
}
