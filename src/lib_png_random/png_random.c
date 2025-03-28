#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include "png_random.h"
#include "png_io.h"
#include "png_processing.h"

typedef struct {
    const char *lang;
    const char **charset;
    size_t charset_size;
    const char **titles;
} Language;

typedef struct {
    char signature[4];    
    uint32_t offset;      
    uint32_t size;   
} icc_tag_t;


const int COLOR_TYPE[] = {PNG_COLOR_TYPE_GRAY, 
                          PNG_COLOR_TYPE_RGB, 
                          PNG_COLOR_TYPE_PALETTE, 
                          PNG_COLOR_TYPE_GRAY_ALPHA, 
                          PNG_COLOR_TYPE_RGBA};

const int GRAY_DEPTH[] = {8, 16}; // also can be = 1, 2, 4
const int PALETTE_DEPTH[] = {8}; // also can be = 1, 2, 4
const int BASE_DEPTH[] = {8, 16};

const int COMPRESS_METHOD[] = {PNG_COMPRESSION_TYPE_DEFAULT};
const int FILTER_METHOD[] = {PNG_FILTER_TYPE_DEFAULT};
const int INTERLACE_METHOD[] = {PNG_INTERLACE_NONE, PNG_INTERLACE_ADAM7};

const int PHYS_UNIT_TYPES[] = {PNG_RESOLUTION_METER, PNG_RESOLUTION_UNKNOWN};

const int SRGB_TYPES[] = {  PNG_sRGB_INTENT_PERCEPTUAL,
                            PNG_sRGB_INTENT_RELATIVE,
                            PNG_sRGB_INTENT_SATURATION,
                            PNG_sRGB_INTENT_ABSOLUTE };

const char *TEXT_TITLES[] = {"Title", "Author", "Descrioption", "Copyright", "Creation Time", 
                             "Comment", "Soft"};

const int TEXT_COMPRESSION_TYPES[] = {PNG_TEXT_COMPRESSION_NONE, PNG_TEXT_COMPRESSION_zTXt};
const int ITXT_COMPRESSION_TYPES[] = {PNG_ITXT_COMPRESSION_NONE, PNG_ITXT_COMPRESSION_zTXt};

const int CSTM_CHUNK_PLACES[] = {PNG_HAVE_IHDR, PNG_HAVE_PLTE, PNG_AFTER_IDAT};

//arrays sizes
enum {
    COLOR_TYPE_SIZE = sizeof(COLOR_TYPE) / sizeof(COLOR_TYPE[0]),

    GRAY_DEPTH_SIZE = sizeof(GRAY_DEPTH) / sizeof(GRAY_DEPTH[0]),
    PALETTE_DEPTH_SIZE = sizeof(PALETTE_DEPTH) / sizeof(PALETTE_DEPTH[0]),
    BASE_DEPTH_SIZE = sizeof(BASE_DEPTH) / sizeof(BASE_DEPTH[0]), 

    COMPRESS_METHOD_SIZE = sizeof(COMPRESS_METHOD) / sizeof(COMPRESS_METHOD[0]), 
    FILTER_METHOD_SIZE = sizeof(FILTER_METHOD) / sizeof(FILTER_METHOD[0]), 
    INTERLACE_METHOD_SIZE = sizeof(INTERLACE_METHOD) / sizeof(INTERLACE_METHOD[0]), 

    PHYS_UNIT_TYPES_SIZE = sizeof(PHYS_UNIT_TYPES) / sizeof(PHYS_UNIT_TYPES[0]),

    SRGB_TYPES_SIZE = sizeof(SRGB_TYPES) / sizeof(SRGB_TYPES[0]),

    TEXT_TITLES_SIZE = sizeof(TEXT_TITLES)/ sizeof(TEXT_TITLES[0]),
    TEXT_CHUNK_MAX_COUNT = TEXT_TITLES_SIZE,
    MIN_TEXT_SIZE = 1,
    MAX_TEXT_SIZE = 100,

    TEXT_COMPRESSION_TYPES_SIZE = sizeof(TEXT_COMPRESSION_TYPES) / sizeof(TEXT_COMPRESSION_TYPES[0]),
    ITXT_COMPRESSION_TYPES_SIZE = sizeof(ITXT_COMPRESSION_TYPES) / sizeof(ITXT_COMPRESSION_TYPES[0]),

    CSTM_CHUNK_PLACES_SIZE = sizeof(CSTM_CHUNK_PLACES) / sizeof(CSTM_CHUNK_PLACES[0]),
};

const char *ENGLISH_CHARSET[] = {
    "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M",
    "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z",
    "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m",
    "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z"
};
const char *RUSSIAN_CHARSET[] = {
        "А", "Б", "В", "Г", "Д", "Е", "Ё", "Ж", "З", "И", "Й", "К", "Л", "М",
        "Н", "О", "П", "Р", "С", "Т", "У", "Ф", "Х", "Ц", "Ч", "Ш", "Щ", "Ъ",
        "Ы", "Ь", "Э", "Ю", "Я",
        "а", "б", "в", "г", "д", "е", "ё", "ж", "з", "и", "й", "к", "л", "м",
        "н", "о", "п", "р", "с", "т", "у", "ф", "х", "ц", "ч", "ш", "щ", "ъ",
        "ы", "ь", "э", "ю", "я"
    };
enum {
    ENGLISH_CHARSET_SIZE = sizeof(ENGLISH_CHARSET) / sizeof(ENGLISH_CHARSET[0]),
    RUSSIAN_CHARSET_SIZE = sizeof(RUSSIAN_CHARSET) / sizeof(RUSSIAN_CHARSET[0]),
};

const char *RUSSIAN_TEXT_TITLES[] = {"Заголовок", "Автор", "Описание", "Авторские права", "Время создания", "Комментарий", "Программа"};

const Language LANGUAGES[] = {{"ru", RUSSIAN_CHARSET, RUSSIAN_CHARSET_SIZE, RUSSIAN_TEXT_TITLES}, 
                              {"en", ENGLISH_CHARSET, ENGLISH_CHARSET_SIZE, TEXT_TITLES}};

enum {
    LANGUAGES_SIZE = sizeof(LANGUAGES) / sizeof(LANGUAGES[0]),
};


enum {
    NOT_USED,
    USED,
};
int used_text_titles[TEXT_TITLES_SIZE] = {};

size_t min(size_t a, size_t b) {
    if (a <= b) 
        return a;
    else 
        return b;
}

size_t max(size_t a, size_t b) {
    if (a >= b) 
        return a;
    else 
        return b;
}

double min_double(double a, double b) {
    if (a <= b)
        return a;
    else 
        return b;
}

double max_double(double a, double b) {
    if (a >= b)
        return a;
    else 
        return b;
}

void make_randomize() {
    int fd = open(RAND_FILE, O_RDONLY);
    unsigned int seed;
    if (read(fd, &seed, sizeof(seed)) != sizeof(seed)) {
        seed = time(NULL);
    }

    srand(seed);
    close(fd);
}

size_t rand_in_range(size_t min, size_t max) {
    return min + (rand() % (max - min + 1));
}

double rand_in_range_double(double min, double max) {
    double random = (double)rand() / (double)RAND_MAX;

    return min + random * (max - min);
}

const char *get_rand_sym(const char *charset[], size_t charset_size) {
    return charset[rand() % charset_size];
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

int write_random_text_to_vector(Vector *vector, size_t text_size, const char **charset, size_t charset_size) {
    if (vector == NULL) 
        return -1;
    
    if (text_size == 0)
        return 0;

    const char *sym;
    for (size_t i = 0; i < text_size; ++i) {
        sym = get_rand_sym(charset, charset_size);
        write_to_vector(vector, sym, strlen(sym));
    }

    sym = "\0";
    write_to_vector(vector, sym, 1);
    return 0;
}   

// pixel_count == height * width (not real size of picture)
int get_random_hw(size_t pixel_count, size_t *height, size_t *width) {
    if (pixel_count < MIN_WIDTH * MIN_HEIGHT || height == NULL || width == NULL)
        return -1;

    if (MIN_HEIGHT <= MIN_WIDTH) {
        *height = rand_in_range(MIN_HEIGHT, min(sqrt(pixel_count), MAX_HEIGHT));
        *width = min(pixel_count / *height, MAX_WIDTH);
    } else {
        *width = rand_in_range(MIN_WIDTH, min(sqrt(pixel_count), MAX_WIDTH));
        *height = min(pixel_count / *width, MAX_HEIGHT);
    }

    return 0;
}

int channels_count(int color_type) {
    int channels = 0;

    switch (color_type) {
        case PNG_COLOR_TYPE_GRAY:
            channels = 1;
            break;
        case PNG_COLOR_TYPE_RGB:
            channels = 3;
            break;
        case PNG_COLOR_TYPE_PALETTE:
            channels = 1;
            break;
        case PNG_COLOR_TYPE_GRAY_ALPHA:
            channels = 2;
            break;
        case PNG_COLOR_TYPE_RGBA:
            channels = 4;
            break;
        default:
            return -1; 
    }

    return channels;
}

int calculate_bytes_per_pixel(int color_type, int bit_depth) {
    int channels = channels_count(color_type);
    if (channels_count < 0) 
        return -1;

    return (bit_depth * channels + 7) / 8; 
}

int get_color_range(png_processing_t *png_prc, size_t *min_color, size_t *max_color) {
    if (png_prc == NULL || min_color == NULL || max_color == NULL) {
        return -1;
    }

    int bit_depth = png_get_bit_depth(png_prc->png, png_prc->info);
    if (bit_depth == 8) {
        *min_color = BIT8_MIN_COLOR;
        *max_color = BIT8_MAX_COLOR;
    } else if (bit_depth == 16) {
        *min_color = BIT16_MIN_COLOR;
        *max_color = BIT16_MAX_COLOR;
    } else {
        return -1;
    }

    return 0;
}

void reset_used_text_titles() {
    for (int i = 0; i < TEXT_TITLES_SIZE; ++i) 
        used_text_titles[i] = NOT_USED;
}

// also set compression level
int png_config_IHDR(png_processing_t *png_prc, size_t pic_size) {
    if (png_prc == NULL || png_prc->chunks[PNG_CHUNK_IHDR].required != IS_REQUIRED || 
        pic_size < MIN_PIC_SIZE) {
        return -1;
    }
    size_t height, width, pixel_count;
    int bit_depth, color_type, pixel_bytes, compression_level;
    int compress_method, filter_method, interlace_method; 

    // color type
    color_type = COLOR_TYPE[rand() % COLOR_TYPE_SIZE];
    if (color_type == PNG_COLOR_TYPE_PALETTE) {
        png_prc->chunks[PNG_CHUNK_PLTE].required = IS_REQUIRED;
    }

    // bit_depth
    if (color_type == PNG_COLOR_TYPE_GRAY) {
        bit_depth = GRAY_DEPTH[rand() % GRAY_DEPTH_SIZE];
    } else if (color_type == PNG_COLOR_TYPE_PALETTE) {
        bit_depth = PALETTE_DEPTH[rand() % PALETTE_DEPTH_SIZE];
    } else {
        bit_depth = BASE_DEPTH[rand() % BASE_DEPTH_SIZE];
    }

    // pixel count
    if (bit_depth < 8) {
        pixel_bytes = 1;
        pixel_count = (pic_size * (8 / bit_depth)) / channels_count(color_type);
    } else {
        pixel_bytes = calculate_bytes_per_pixel(color_type, bit_depth);
        if (pixel_bytes < 0) {
            return -1;
        }
        pixel_count = pic_size / pixel_bytes;
    }
    
    if (get_random_hw(pixel_count, &height, &width) < 0) {
        return -1;
    }

    compress_method = COMPRESS_METHOD[rand() % COMPRESS_METHOD_SIZE];
    filter_method = FILTER_METHOD[rand() % COMPRESS_METHOD_SIZE];
    interlace_method = INTERLACE_METHOD[rand() % INTERLACE_METHOD_SIZE];

    compression_level = rand_in_range(MIN_COMPRESSION_LEVEL, MAX_COMPRESSION_LEVEL);
    png_set_compression_level(png_prc->png,compression_level);

    png_set_IHDR(png_prc->png, png_prc->info, 
                 width, height, 
                 bit_depth, 
                 color_type, 
                 interlace_method, 
                 compress_method, 
                 filter_method);

    png_prc->chunks[PNG_CHUNK_IHDR].valid = IS_VALID;
    
    return 0;       
}

int png_set_random_chunks(png_processing_t *png_prc) {
    if (png_prc == NULL)
        return -1;

    for (int i = BASE_CHUNK_COUNT; i < CHUNK_COUNT; ++i) {
        if (rand() % 2) {
            png_prc->chunks[i].required = IS_REQUIRED;
        }
    }

    return 0;     
}

int png_remove_random_chunks(png_processing_t *png_prc) {
    if (png_prc == NULL)
        return -1;

    for (int i = BASE_CHUNK_COUNT; i < CHUNK_COUNT; ++i) {
        if (rand() % 2) {
            reset_chunk(&(png_prc->chunks[i]), i);
            png_prc->chunks[i].required = NOT_REQUIRED;
        }
    }

    return 0;     
}

int png_clone_random_chunks(png_processing_t *png_prc) {
    if (png_prc == NULL)
        return -1;

    for (int i = BASE_CHUNK_COUNT; i < CHUNK_COUNT; ++i) {
        if (rand() % 2 && png_prc->chunks[i].required == IS_REQUIRED)
            png_prc->chunks[i].cloned = IS_CLONED;
    }

    return 0;     
}

int png_config_bKGD(png_processing_t *png_prc) {
    if (png_prc->chunks[PNG_CHUNK_bKGD].required != IS_REQUIRED)
        return -1;

    png_color_16 background;
    size_t min_color, max_color;
    
    get_color_range(png_prc, &min_color, &max_color);

    Vector *out = &(png_prc->chunks[PNG_CHUNK_bKGD].info);
    clean_vector(out);

    int color_type = png_get_color_type(png_prc->png, png_prc->info);

    if (color_type == PNG_COLOR_TYPE_PALETTE)
        background.index = rand_in_range(BIT8_MIN_COLOR, BIT8_MAX_COLOR);
    else if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA){
        background.gray = rand_in_range(min_color, max_color);
    } else if (color_type == PNG_COLOR_TYPE_RGB || color_type == PNG_COLOR_TYPE_RGBA) {
        background.red = rand_in_range(min_color, max_color);
        background.green = rand_in_range(min_color, max_color);
        background.blue = rand_in_range(min_color, max_color);
    } else {
        return -1;
    }
    
    
    write_to_vector(out, &background, sizeof(background));

    png_set_bKGD(png_prc->png, png_prc->info, out->data);
    png_prc->chunks[PNG_CHUNK_bKGD].valid = IS_VALID;
    return 0;
}


int png_config_tRNS(png_processing_t *png_prc) {
    if (png_prc == NULL || png_prc->chunks[PNG_CHUNK_tRNS].required != IS_REQUIRED) {
            return -1;
    }

    size_t min_color, max_color;
    get_color_range(png_prc, &min_color, &max_color);

    int color_type = png_get_color_type(png_prc->png, png_prc->info);
    Vector *out = &(png_prc->chunks[PNG_CHUNK_tRNS].info);
    clean_vector(out);

    switch (color_type) {
        case PNG_COLOR_TYPE_GRAY: {
            png_color_16 trans_color = { .gray = rand_in_range(min_color, max_color) };
            write_to_vector(out, &trans_color, sizeof(trans_color));
    
            png_set_tRNS(png_prc->png, png_prc->info, NULL, 0, out->data);
            break;
        }
        case PNG_COLOR_TYPE_RGB: {
            png_color_16 trans_color;
            trans_color.red = rand_in_range(min_color, max_color);
            trans_color.green = rand_in_range(min_color, max_color);
            trans_color.blue = rand_in_range(min_color, max_color);

            write_to_vector(out, &trans_color, sizeof(trans_color));
            png_set_tRNS(png_prc->png, png_prc->info, NULL, 0, out->data);
            break;
        }
        case PNG_COLOR_TYPE_PALETTE: {
            png_byte trans_color;
            for (int i = 0; i < 256; i++) {
                trans_color = rand_in_range(BIT8_MIN_COLOR, BIT8_MAX_COLOR);
                write_to_vector(out, &trans_color, sizeof(trans_color));
            }

            png_set_tRNS(png_prc->png, png_prc->info, out->data, out->len, NULL);
            break;
        }
        case PNG_COLOR_TYPE_RGBA:
        case PNG_COLOR_TYPE_GRAY_ALPHA:
            return 0;
        default: 
            return -1;
    }

    png_prc->chunks[PNG_CHUNK_tRNS].valid = IS_VALID;
    
    return 0;
}


int png_config_PLTE(png_processing_t *png_prc) {
    if (png_prc == NULL || png_prc->chunks[PNG_CHUNK_PLTE].required != IS_REQUIRED) 
        return -1;

    int color_type = png_get_color_type(png_prc->png, png_prc->info);
    if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
        return 0;

    size_t min_color = BIT8_MIN_COLOR;
    size_t max_color = BIT8_MAX_COLOR;

    Vector *palette_out = &(png_prc->chunks[PNG_CHUNK_PLTE].info);
    clean_vector(palette_out);

    png_color palette;

    for (int i = 0; i < PALETTE_SIZE; ++i) {
        palette.red = rand_in_range(min_color, max_color);
        palette.green = rand_in_range(min_color ,max_color);
        palette.blue = rand_in_range(min_color, max_color);
        write_to_vector(palette_out, &palette, sizeof(palette));
    }

    png_set_PLTE(png_prc->png, png_prc->info, palette_out->data, PALETTE_SIZE);
    png_prc->chunks[PNG_CHUNK_PLTE].valid = IS_VALID;
    
    return 0;
}

int png_config_sPLT(png_processing_t *png_prc) {
    if (png_prc == NULL || png_prc->chunks[PNG_CHUNK_sPLT].required != IS_REQUIRED) {
        return -1;
    }

    int color_type = png_get_color_type(png_prc->png, png_prc->info);
    if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
        return 0;

    size_t min_color = BIT8_MIN_COLOR;
    size_t max_color = BIT8_MAX_COLOR;

    int bit_depth = png_get_bit_depth(png_prc->png, png_prc->info);
    if (bit_depth == 16) {
        min_color = BIT16_MIN_COLOR;
        max_color = BIT16_MAX_COLOR;
    }

    Vector *palette_out = &(png_prc->chunks[PNG_CHUNK_PLTE].info);
    clean_vector(palette_out);

    png_sPLT_entry palette;

    for (int i = 0; i < PALETTE_SIZE; ++i) {
        palette.red = rand_in_range(min_color, max_color);
        palette.green = rand_in_range(min_color ,max_color);
        palette.blue = rand_in_range(min_color, max_color);
        palette.alpha = rand_in_range(min_color, max_color);
        palette.frequency = rand_in_range(min_color, max_color);
        
        write_to_vector(palette_out, &palette, sizeof(palette));
    }

    png_sPLT_t splt;
    memset(&splt, 0, sizeof(png_sPLT_t));
    
    size_t old_size = palette_out->len;
    write_to_vector(palette_out, &splt, sizeof(splt)); 
    png_sPLT_t *splt_ptr = palette_out->data + old_size;
    
    splt_ptr->name = "Custom SPalette";  // Название палитры
    splt_ptr->depth = bit_depth;               // 16-битная глубина
    splt_ptr->entries = palette_out->data;        // Цвета
    splt_ptr->nentries = PALETTE_SIZE;  // Количество цветов
    
    png_set_sPLT(png_prc->png, png_prc->info, splt_ptr, 1);
    png_prc->chunks[PNG_CHUNK_sPLT].valid = IS_VALID;
    
    return 0;
}

int png_config_gAMA(png_processing_t *png_prc) {
    if (png_prc == NULL || png_prc->chunks[PNG_CHUNK_gAMA].required != IS_REQUIRED) 
        return -1;

    double gamma = rand_in_range_double(MIN_GAMA, MAX_GAMA);
    png_set_gAMA(png_prc->png, png_prc->info, gamma);
    png_prc->chunks[PNG_CHUNK_gAMA].valid = IS_VALID;

    return 0;
}

int png_config_cHRM(png_processing_t *png_prc) {
    if (png_prc == NULL || png_prc->chunks[PNG_CHUNK_cHRM].required != IS_REQUIRED)
        return -1;

    double wx = rand_in_range_double(MIN_CHRM_VAL, MAX_CHRM_VAL);  // White point x
    double wy = rand_in_range_double(MIN_CHRM_VAL, min_double(MAX_CHRM_VAL, 1 - wx));;  // White point y
    double red_x = rand_in_range_double(MIN_CHRM_VAL, MAX_CHRM_VAL);  // Red x
    double red_y = rand_in_range_double(MIN_CHRM_VAL, min_double(MAX_CHRM_VAL, 1 - red_x));;  // Red y
    double green_x = rand_in_range_double(MIN_CHRM_VAL, MAX_CHRM_VAL);  // Green x
    double green_y = rand_in_range_double(MIN_CHRM_VAL, min_double(MAX_CHRM_VAL, 1 - green_x));;  // Green y
    double blue_x = rand_in_range_double(MIN_CHRM_VAL, MAX_CHRM_VAL);  // Blue x
    double blue_y = rand_in_range_double(MIN_CHRM_VAL, min_double(MAX_CHRM_VAL, 1 - blue_x));  // Blue y


    png_set_cHRM(png_prc->png, png_prc->info,
                 wx, wy,
                 red_x, red_y,
                 green_x, green_y,
                 blue_x, blue_y);

    png_prc->chunks[PNG_CHUNK_cHRM].valid = IS_VALID;
    return 0;
}   


#define ICC_HEADER_SIZE 132 

int generate_icc_profile(png_processing_t *png_prc) {
    Vector *profile = &(png_prc->chunks[PNG_CHUNK_iCCP].info);

    uint8_t header[ICC_HEADER_SIZE] = {0};
    uint32_t profile_size, tag_count = 3; // 3 тега: rXYZ, gXYZ, bXYZ

    // --- 1. Заполнение заголовка ICC (132 байта) ---
    // Размер профиля = заголовок + таблица тегов + данные
    profile_size = ICC_HEADER_SIZE + sizeof(icc_tag_t) * tag_count + 36; // 36 байт данных
    *(uint32_t*)(header + 0) = htonl(profile_size);
    // CMM Type (Little CMS)
    memcpy(header + 4, "lcms", 4);

    // Версия (2.1.0)
    header[8] = 0x02;
    header[9] = 0x10;
    header[10] = 0x00;

    // Device Class (Display)
    memcpy(header + 12, "mntr", 4);

    // Color Space (RGB)
    memcpy(header + 16, "RGB ", 4);

    // Дата создания
    time_t now = time(NULL);
    struct tm *tm = gmtime(&now);
    *(uint16_t*)(header + 24) = htons(1900 + tm->tm_year);
    header[26] = tm->tm_mon + 1;
    header[27] = tm->tm_mday;
    header[28] = tm->tm_hour;
    header[29] = tm->tm_min;
    header[30] = tm->tm_sec;

    // Сигнатура 'acsp' (обязательно!)
    memcpy(header + 36, "acsp", 4);

    // Platform (Apple)
    memcpy(header + 40, "APPL", 4);

    // Flags (0)
    *(uint32_t*)(header + 44) = 0;

    // --- 2. Указываем количество тегов (байты 128-131) ---
    *(uint32_t*)(header + 128) = htonl(tag_count);

    // --- 3. Таблица тегов (начинается с байта 132) ---
    icc_tag_t tags[3] = {
        {"rXYZ", htonl(ICC_HEADER_SIZE + 0),  htonl(12)}, // Красный
        {"gXYZ", htonl(ICC_HEADER_SIZE + 12), htonl(12)}, // Зеленый
        {"bXYZ", htonl(ICC_HEADER_SIZE + 24), htonl(12)}  // Синий
    };

    // --- 4. Данные тегов (XYZ-координаты) ---
    uint8_t tags_data[36] = {
        // rXYZ (D50-адаптированные значения)
        0x00, 0x00, 0xF6, 0xD6, // X = 0.9642
        0x00, 0x00, 0x00, 0x00, // Y = 1.0000 
        0x00, 0x00, 0x00, 0x00, // Z = 0.8249

        // gXYZ
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,

        // bXYZ
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00
    };

    // --- 5. Сборка профиля ---
    write_to_vector(profile, header, ICC_HEADER_SIZE);     // Заголовок
    write_to_vector(profile, tags, sizeof(tags));          // Таблица тегов
    write_to_vector(profile, tags_data, sizeof(tags_data)); // Данные

    return 0;
}

png_byte* load_icc_profile(png_processing_t *png_prc, const char* path, png_uint_32* length) {

    Vector *vector = &(png_prc->chunks[PNG_CHUNK_iCCP].info);

    if (png_prc->chunks[PNG_CHUNK_iCCP].info.len) {
        *length = png_prc->chunks[PNG_CHUNK_iCCP].info.len;
        return vector->data;
    }

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

    write_to_vector(vector, data, *length);
    free(data);

    return vector->data;
}


int png_config_iCCP(png_processing_t *png_prc) {
    if (png_prc == NULL || png_prc->chunks[PNG_CHUNK_iCCP].required != IS_REQUIRED) 
        return -1;

    if (png_prc->chunks[PNG_CHUNK_sRGB].valid == IS_VALID)
        return 0;

    png_uint_32 icc_length;
    png_byte* icc_data = load_icc_profile(png_prc, ICC_PATH, &icc_length);
    if (!icc_data) {
        return -1;
    }
/*
    unsigned char* profile;
    size_t profile_size;

    if (generate_icc_profile(png_prc) < 0) {
        return -1;
    }

    profile = png_prc->chunks[PNG_CHUNK_iCCP].info.data;
    profile_size = png_prc->chunks[PNG_CHUNK_iCCP].info.len;
*/
    png_set_iCCP(png_prc->png, png_prc->info, ICC_PROFILE_NAME, 
                 PNG_COMPRESSION_TYPE_BASE, icc_data, icc_length);
    png_prc->chunks[PNG_CHUNK_iCCP].valid = IS_VALID;

    return 0;
}

int png_config_pHYs(png_processing_t *png_prc) {
    if (png_prc == NULL || png_prc->chunks[PNG_CHUNK_pHYs].required != IS_REQUIRED) 
        return -1;

    int unit_type = PHYS_UNIT_TYPES[rand_in_range(0, PHYS_UNIT_TYPES_SIZE - 1)];
    uint32_t res_x, res_y;

    if (unit_type == PNG_RESOLUTION_METER) {
        res_x = rand_in_range(MIN_PHYS_METER, MAX_PHYS_METER);
        res_y = res_x;
    } else if (unit_type == PNG_RESOLUTION_UNKNOWN) {
        res_x = rand_in_range(MIN_PHYS_UNKNOWN, MAX_PHYS_UNKNOWN);
        res_y = rand_in_range(MIN_PHYS_UNKNOWN, MAX_PHYS_UNKNOWN);
    } else {
        return -1;
    }

    png_set_pHYs(png_prc->png, png_prc->info, res_x, res_y, unit_type);
    png_prc->chunks[PNG_CHUNK_pHYs].valid = IS_VALID;

    return 0;

}

int png_config_sBIT(png_processing_t *png_prc) {
    if (png_prc == NULL || png_prc->chunks[PNG_CHUNK_sBIT].required != IS_REQUIRED) 
        return -1;

    int color_type = png_get_color_type(png_prc->png, png_prc->info);
    int bit_depth = png_get_bit_depth(png_prc->png, png_prc->info);
    int min_bit = 1;
    int max_bit = bit_depth;

    png_color_8 sig_bit;
    if (color_type == PNG_COLOR_TYPE_GRAY) {
        sig_bit.gray = rand_in_range(min_bit, max_bit); 
    } else if (color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
        sig_bit.gray = rand_in_range(min_bit, max_bit); 
        sig_bit.alpha = rand_in_range(min_bit, max_bit); 
    } else if (color_type == PNG_COLOR_TYPE_RGB) {
        sig_bit.red = rand_in_range(min_bit, max_bit);
        sig_bit.green = rand_in_range(min_bit, max_bit);
        sig_bit.blue = rand_in_range(min_bit, max_bit);
    } else if (color_type == PNG_COLOR_TYPE_RGB_ALPHA) {
        sig_bit.red = rand_in_range(min_bit, max_bit);
        sig_bit.green = rand_in_range(min_bit, max_bit);
        sig_bit.blue = rand_in_range(min_bit, max_bit); 
        sig_bit.alpha = rand_in_range(min_bit, max_bit);
    } else {
        return 0;
    }

    png_set_sBIT(png_prc->png, png_prc->info, &sig_bit);
    png_prc->chunks[PNG_CHUNK_sBIT].valid = IS_VALID;

    return 0;

}

int png_config_sRGB(png_processing_t *png_prc) {
    if (png_prc == NULL || png_prc->chunks[PNG_CHUNK_sRGB].required != IS_REQUIRED) 
        return -1;

    if (png_prc->chunks[PNG_CHUNK_iCCP].valid == IS_VALID)
        return 0;

    int intend_type = SRGB_TYPES[rand_in_range(0, SRGB_TYPES_SIZE - 1)];
    
    png_set_sRGB(png_prc->png, png_prc->info, intend_type);
    png_prc->chunks[PNG_CHUNK_sRGB].valid = IS_VALID;

    return 0;
}

int png_config_tEXt(png_processing_t *png_prc) {
    if (png_prc == NULL || png_prc->chunks[PNG_CHUNK_tEXt].required != IS_REQUIRED) 
        return -1;

    Vector *full_text = &(png_prc->chunks[PNG_CHUNK_tEXt].info);
    clean_vector(full_text);

    png_text text_chunks[TEXT_CHUNK_MAX_COUNT] = {};
    int chunk_count = 0;

    for (int i = 0; i < TEXT_CHUNK_MAX_COUNT; ++i) {
        if (rand_in_range(0, 1) || used_text_titles[i] == USED) {
            continue;
        }

        used_text_titles[i] = USED;

        text_chunks[chunk_count].compression = TEXT_COMPRESSION_TYPES[rand() % TEXT_COMPRESSION_TYPES_SIZE]; 
        text_chunks[chunk_count].key = TEXT_TITLES[i];
    
        size_t text_size = rand_in_range(MIN_TEXT_SIZE, MAX_TEXT_SIZE);
        if (write_random_text_to_vector(full_text, text_size, ENGLISH_CHARSET, ENGLISH_CHARSET_SIZE) < 0)
            return -1;

        text_chunks[chunk_count].text = full_text->data + full_text->len - text_size;
        ++chunk_count;
    }

    png_set_text(png_prc->png, png_prc->info, text_chunks, chunk_count);
    png_prc->chunks[PNG_CHUNK_tEXt].valid = IS_VALID;

    return 0;
}

int png_config_iTXt(png_processing_t *png_prc) {
    if (png_prc == NULL || png_prc->chunks[PNG_CHUNK_iTXt].required != IS_REQUIRED) 
        return -1;

    Vector *full_text = &(png_prc->chunks[PNG_CHUNK_iTXt].info);
    clean_vector(full_text);

    png_text text_chunks[TEXT_CHUNK_MAX_COUNT] = {};
    int chunk_count = 0;

    for (int i = 0; i < TEXT_CHUNK_MAX_COUNT; ++i) {
        if (rand_in_range(0, 1) || used_text_titles[i] == USED) {
            continue;
        }

        used_text_titles[i] = USED;

        text_chunks[chunk_count].compression = ITXT_COMPRESSION_TYPES[rand() % ITXT_COMPRESSION_TYPES_SIZE]; 
        text_chunks[chunk_count].key = TEXT_TITLES[i];
   
        int language = rand() % LANGUAGES_SIZE;

        text_chunks[chunk_count].lang = LANGUAGES[language].lang;
        text_chunks[chunk_count].lang_key = LANGUAGES[language].titles[i];

        size_t text_size = rand_in_range(MIN_TEXT_SIZE, MAX_TEXT_SIZE);
        size_t old_text_size = full_text->len;

        if (write_random_text_to_vector(full_text, text_size, LANGUAGES[language].charset, LANGUAGES[language].charset_size) < 0)
            return -1;

        text_chunks[chunk_count].text = full_text->data + old_text_size;
        ++chunk_count;
    }

    png_set_text(png_prc->png, png_prc->info, text_chunks, chunk_count);
    png_prc->chunks[PNG_CHUNK_iTXt].valid = IS_VALID;

    return 0;
}

int png_config_tIME(png_processing_t *png_prc) {
    if (png_prc == NULL || png_prc->chunks[PNG_CHUNK_tIME].required != IS_REQUIRED) 
        return -1;

    png_time mod_time;
    time_t current_time = time(NULL);
    png_convert_from_time_t(&mod_time, current_time);

    png_set_tIME(png_prc->png, png_prc->info, &mod_time); 
    png_prc->chunks[PNG_CHUNK_tIME].valid = IS_VALID;

    return 0;
}

// custom chunk
int png_config_cSTM(png_processing_t *png_prc) {
    if (png_prc == NULL || png_prc->chunks[PNG_CHUNK_cSTM].required != IS_REQUIRED) 
        return -1;

    png_unknown_chunk custom_chunk= {};
    const char chunk_name[] = "cSTM";

    Vector *chunk_data = &(png_prc->chunks[PNG_CHUNK_cSTM].info);
    clean_vector(chunk_data);

    size_t text_size = rand_in_range(MIN_TEXT_SIZE, MAX_TEXT_SIZE);
    if (write_random_text_to_vector(chunk_data, text_size, ENGLISH_CHARSET, ENGLISH_CHARSET_SIZE) < 0){
        return -1;
    }


    memcpy(custom_chunk.name, chunk_name, 5); 
    custom_chunk.data = (png_bytep) chunk_data->data; 
    custom_chunk.size = chunk_data->len; 
    custom_chunk.location = CSTM_CHUNK_PLACES[rand() % CSTM_CHUNK_PLACES_SIZE]; 

    int color_type = png_get_color_type(png_prc->png, png_prc->info);
    if (color_type != PNG_COLOR_TYPE_PALETTE && custom_chunk.location == PNG_HAVE_PLTE)
        custom_chunk.location = PNG_HAVE_IHDR; 
    
    png_set_unknown_chunks(png_prc->png, png_prc->info, &custom_chunk, 1);

    png_prc->chunks[PNG_CHUNK_cSTM].valid = IS_VALID;

    return 0;
}

int is_config_chunk(PNGChunk_t *chunk) {
    if (chunk != NULL && 
        chunk->required == IS_REQUIRED && 
        chunk->valid != IS_VALID) { 
        return 1;
    } else {
        return 0;
    }
}

int png_config_chunks(png_processing_t *png_prc, size_t pic_size) {
    if (is_config_chunk(&(png_prc->chunks[PNG_CHUNK_IHDR]))) {
        if (pic_size < MIN_PIC_SIZE || pic_size > MAX_PIC_SIZE)
            return -1;

        if (png_config_IHDR(png_prc, pic_size) < 0) {
            printf("bad config IHDR\n");
            return -1;
        }
    }
    
    if (is_config_chunk(&(png_prc->chunks[PNG_CHUNK_bKGD]))) {
        if (png_config_bKGD(png_prc) < 0) {
            printf("bad config bKGD\n");
            return -1;
        }
    }
    
    if (is_config_chunk(&(png_prc->chunks[PNG_CHUNK_tRNS]))) {
        if (png_config_tRNS(png_prc) < 0) {
            printf("bad config tRNS\n");
            return -1;
        }
    }

    if (is_config_chunk(&(png_prc->chunks[PNG_CHUNK_PLTE]))) {
        if (png_config_PLTE(png_prc) < 0) {
            printf("bad config PLTEE\n");
            return -1;
        }
    }
    
    if (is_config_chunk(&(png_prc->chunks[PNG_CHUNK_sPLT]))) {
        if (png_config_sPLT(png_prc) < 0) {
            printf("bad config sPLT\n");
            return -1;
        }
    }

    if (is_config_chunk(&(png_prc->chunks[PNG_CHUNK_gAMA]))) {
        if (png_config_gAMA(png_prc) < 0) {
            printf("bad config gAMA\n");
            return -1;
        }
    }

    if (is_config_chunk(&(png_prc->chunks[PNG_CHUNK_cHRM]))) {
        if (png_config_cHRM(png_prc) < 0) {
            printf("bad config cHRM\n");
            return -1;
        }
    }
   
    if (is_config_chunk(&(png_prc->chunks[PNG_CHUNK_iCCP]))) {
        if (png_config_iCCP(png_prc) < 0) {
            printf("bad config iCCP\n");
            return -1;
        }
    }

    if (is_config_chunk(&(png_prc->chunks[PNG_CHUNK_pHYs]))) {
        if (png_config_pHYs(png_prc) < 0) {
            printf("bad config pHYs\n");
            return -1;
        }
    }

    if (is_config_chunk(&(png_prc->chunks[PNG_CHUNK_sBIT]))) {
        if (png_config_sBIT(png_prc) < 0) {
            printf("bad config sBIT\n");
            return -1;
        }
    }

    if (is_config_chunk(&(png_prc->chunks[PNG_CHUNK_sRGB]))) {
        if (png_config_sRGB(png_prc) < 0) {
            printf("bad config sRGB\n");
            return -1;
        }
    }

    reset_used_text_titles();
    if (is_config_chunk(&(png_prc->chunks[PNG_CHUNK_tEXt]))) {
        if (png_config_tEXt(png_prc) < 0) {
            printf("bad config tEXt\n");
            return -1;
        }
    }

    if (is_config_chunk(&(png_prc->chunks[PNG_CHUNK_iTXt]))) {
        if (png_config_iTXt(png_prc) < 0) {
            printf("bad config iTXt\n");
            return -1;
        }
    }

    if (is_config_chunk(&(png_prc->chunks[PNG_CHUNK_tIME]))) {
        if (png_config_tIME(png_prc) < 0) {
            printf("bad config tIME\n");
            return -1;
        }
    }

    if (is_config_chunk(&(png_prc->chunks[PNG_CHUNK_cSTM]))) {
        if (png_config_cSTM(png_prc) < 0) {
            printf("bad config cSTM\n");
            return -1;
        }
    } 
    return 0;
}

int make_random_png(png_processing_t *png_prc, uint8_t *pic, size_t pic_size) {
    if (png_prc == NULL) 
        return -1;

    if (pic_size > MAX_PIC_SIZE) {
        return -1;
    }

    int free_flag = 0;
    if (pic == NULL) {
        if (pic_size == 0) 
            pic_size = rand_in_range(MIN_PIC_SIZE, MAX_PIC_SIZE);

        pic = get_random_data(pic_size);
        if (pic == NULL)
            return -1;
        free_flag = 1;
    } else if (pic_size < MIN_PIC_SIZE) {
        return -1;
    }

    reset_png_processing(png_prc);
    png_set_random_chunks(png_prc);

    if (png_config_chunks(png_prc, pic_size) < 0) {
        printf("bad config chunks\n");
        return -1;
    }

    if (png_write(png_prc, pic) < 0) {
        printf("Bad write to vector\n");
        return -1;
    }
    
    if (free_flag)
        free(pic);
    return 0;
}
