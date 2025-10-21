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

// Функции для отрисовки специфичных экранов
void draw_screen_food();
void draw_screen_game();
void draw_screen_medicine();
void draw_screen_toilet();
void draw_screen_status();
void draw_screen_study();
void draw_screen_discipline();
void draw_screen_settings();
void draw_screen_main();

#endif // MENU_H