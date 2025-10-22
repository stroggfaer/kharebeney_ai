#ifndef UTILS_H
#define UTILS_H

#include "ssd1306.h"

// Функция для центрирования и отображения спрайта на экране
void display_centered_sprite(SSD1306_t* oled, const uint8_t* sprite_data, int sprite_width, int sprite_height);

// Функция для центрирования и отображения текста на экране
void display_centered_text(SSD1306_t* oled, const char* text);

void clear_screen_center(SSD1306_t* oled);

#endif // UTILS_H
