#include "screen_main.h"
#include "../core/kharebeney_agent.h"
#include "../fonts/icons.h"
#include "ssd1306.h"
#include "../display.h"
#include "../lib/utlis.h"

extern KharebeneyAgent* agent;

void draw_screen_main(SSD1306_t* oled) {
    // Выводим тестовый спрайт вместо текста "Main" (32x32 пикселей) по центру
    display_centered_sprite(oled, (const uint8_t*)icon_food, 16, 16);
    ssd1306_show_buffer(oled);
}

void handle_main_interaction(int x, int y) {
    // Пока ничего не делаем
}