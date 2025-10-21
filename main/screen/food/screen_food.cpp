#include "screen_food.h"
#include "../core/kharebeney_agent.h"
#include "../fonts/icons.h"
#include "ssd1306.h"
#include "../display.h"

extern KharebeneyAgent agent;

void draw_screen_food() {
    ssd1306_clear_screen(&oled, false);
    ssd1306_display_text(&oled, 3, "Food", 4, false);
    ssd1306_show_buffer(&oled);
}

void handle_food_interaction(int x, int y) {
    // Пока ничего не делаем
}