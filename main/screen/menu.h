// ======================== menu.h ========================
#ifndef MENU_H
#define MENU_H

#include "ssd1306.h"
#include "config.h"

typedef struct {
    int selectedIndex;   // индекс текущего выбора
    char currentScreen;  // текущий экран (символ)
} MenuState;

void menu_init(SSD1306_t* oled);
void menu_draw(SSD1306_t* oled, const MenuState* state);
void menu_update_status(SSD1306_t* oled);
void menu_handle_input(SSD1306_t* oled, MenuState* state, int btnA, int btnB, int btnC);

#endif // MENU_H