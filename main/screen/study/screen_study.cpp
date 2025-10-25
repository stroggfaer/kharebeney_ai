#include "screen_study.h"
#include "../core/kharebeney_agent.h"
#include "../fonts/icons.h"
#include "ssd1306.h"
#include "../display.h"
#include "../lib/utlis.h"

extern KharebeneyAgent* agent;

void draw_screen_study(SSD1306_t* oled) {
    clear_screen_center(oled);

    ssd1306_display_text(oled, 3, "Study", 5, false);
    ssd1306_show_buffer(oled);
}

void handle_study_interaction(int x, int y) {
    // Пока ничего не делаем
}