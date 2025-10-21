#include "utlis.h"
#include <string.h>

// Функция для центрирования и отображения спрайта на экране
void display_centered_sprite(SSD1306_t* oled, const uint8_t* sprite_data, int sprite_width, int sprite_height) {
    if (!oled || !sprite_data) return;
    
    int screen_width = 128;  // ширина экрана SSD1306
    int screen_height = 64;  // высота экрана SSD1306
    int x = (screen_width - sprite_width) / 2;   // центрирование по X
    int y = (screen_height - sprite_height) / 2; // центрирование по Y
    ssd1306_bitmaps(oled, x, y, (uint8_t*)sprite_data, sprite_width, sprite_height, false);
}

// Функция для центрирования и отображения текста на экране
void display_centered_text(SSD1306_t* oled, const char* text) {
    if (!oled || !text) return;
    
    int screen_width = 128;  // ширина экрана SSD1306
    int text_len = strlen(text);
    int x = (screen_width - (text_len * 8)) / 2; // 8 - ширина символа в шрифте 8x8
    int y = 3; // строка 3 - центр экрана
    // Используем ssd1306_display_text_box1 для отображения текста с заданной позицией X
    ssd1306_display_text_box1(oled, y, x, text, text_len, text_len, true, 0);
}