// ======================== menu.h ========================
#ifndef MENU_H
#define MENU_H

#include "ssd1306.h"
#include "config.h"

typedef enum {
    SCREEN_MAIN,
    SCREEN_SUBMENU
} ScreenMode;

typedef struct {
    int selectedIndex;
    char currentScreen;
    ScreenMode mode;
} MenuState;

void menu_init(SSD1306_t* oled);
void menu_draw(SSD1306_t* oled, const MenuState* state);
void menu_update_status(SSD1306_t* oled);
void menu_handle_input(SSD1306_t* oled, MenuState* state, int btnA, int btnB, int btnC);

// Функции экранов
void draw_screen_food(SSD1306_t* oled);
void draw_screen_game(SSD1306_t* oled);
void draw_screen_medicine(SSD1306_t* oled);
void draw_screen_toilet(SSD1306_t* oled);
void draw_screen_status(SSD1306_t* oled);
void draw_screen_study(SSD1306_t* oled);
void draw_screen_discipline(SSD1306_t* oled);
void draw_screen_settings(SSD1306_t* oled);
void draw_screen_main(SSD1306_t* oled);

#endif // MENU_H
