#include "screen_toilet.h"
#include "../core/kharebeney_agent.h"
#include "../fonts/icons.h"
#include "ssd1306.h"
#include "../display.h"
#include "../lib/utlis.h"

extern KharebeneyAgent agent;

void draw_screen_toilet(SSD1306_t* oled) {
    clear_screen_center(oled);

    ssd1306_display_text(oled, 3, "Toilet", 6, false);
    ssd1306_show_buffer(oled);
}

void handle_toilet_interaction(int x, int y) {
    // Пока ничего не делаем
}