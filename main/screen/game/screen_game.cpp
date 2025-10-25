#include "screen_game.h"
#include "../core/kharebeney_agent.h"
#include "../fonts/icons.h"
#include "ssd1306.h"
#include "../display.h"
#include "../lib/utlis.h"

extern KharebeneyAgent* agent;

void draw_screen_game(SSD1306_t* oled) {
    clear_screen_center(oled);

    ssd1306_display_text(oled, 3, "Game", 4, false);
    ssd1306_show_buffer(oled);
}

void handle_game_interaction(int x, int y) {
    // Пока ничего не делаем
}