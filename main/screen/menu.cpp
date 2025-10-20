// ======================== menu.cpp ========================
#include "menu.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

static const char* topMenu[] = {"E", "G", "M", "T"};
static const char* bottomMenu[] = {"C", "Y", "D", "S"};
static const int topCount = 4;
static const int bottomCount = 4;

static void get_time_string(char* buf, size_t len) {
    snprintf(buf, len, "12:34");
}

static int get_battery_percent() {
    return 100;
}

// --- Инициализация ---
void menu_init(SSD1306_t* oled) {
    ssd1306_clear_screen(oled, false);
    MenuState initState = { .selectedIndex = 0, .currentScreen = 'E', .mode = SCREEN_MAIN };
    menu_draw(oled, &initState);
}

// --- Отрисовка ---
void menu_draw(SSD1306_t* oled, const MenuState* state) {
    ssd1306_clear_screen(oled, false);

    char buf[32];
    char topBuf[32] = "";
    char bottomBuf[32] = "";

    // --- Верхний ряд ---
    for (int i = 0; i < topCount; i++) {
        char tmp[8];
        if (state->mode == SCREEN_MAIN && state->selectedIndex == i)
            snprintf(tmp, sizeof(tmp), "[%s] ", topMenu[i]);
        else
            snprintf(tmp, sizeof(tmp), " %s  ", topMenu[i]);
        strcat(topBuf, tmp);
    }

    // --- Нижний ряд ---
    for (int i = 0; i < bottomCount; i++) {
        char tmp[8];
        if (state->mode == SCREEN_MAIN && state->selectedIndex == i + topCount)
            snprintf(tmp, sizeof(tmp), "[%s] ", bottomMenu[i]);
        else
            snprintf(tmp, sizeof(tmp), " %s  ", bottomMenu[i]);
        strcat(bottomBuf, tmp);
    }

    ssd1306_display_text(oled, 0, topBuf, strlen(topBuf), false);
    ssd1306_display_text(oled, 6, bottomBuf, strlen(bottomBuf), false);

    // --- Центр экрана ---
    if (state->mode == SCREEN_MAIN) {
        snprintf(buf, sizeof(buf), "Main");
    } else {
        snprintf(buf, sizeof(buf), "Menu: %c", state->currentScreen);
    }
    ssd1306_display_text(oled, 3, buf, strlen(buf), true);

    // --- Статус (часы + батарея) ---
    char timeBuf[16];
    char battBuf[8];
    get_time_string(timeBuf, sizeof(timeBuf));
    snprintf(battBuf, sizeof(battBuf), "%d%%", get_battery_percent());
    char status[32];
    snprintf(status, sizeof(status), "%s %s", timeBuf, battBuf);
    ssd1306_display_text(oled, 1, status, strlen(status), false);

    ssd1306_show_buffer(oled);
}

// --- Обновление верхнего статуса ---
void menu_update_status(SSD1306_t* oled) {
    char timeBuf[16];
    char battBuf[8];
    get_time_string(timeBuf, sizeof(timeBuf));
    snprintf(battBuf, sizeof(battBuf), "%d%%", get_battery_percent());
    char status[32];
    snprintf(status, sizeof(status), "%s %s", timeBuf, battBuf);
    ssd1306_display_text(oled, 1, status, strlen(status), false);
    ssd1306_show_buffer(oled);
}

// --- Обработка ввода ---
void menu_handle_input(SSD1306_t* oled, MenuState* state, int btnA, int btnB, int btnC) {
    if (btnA == 0 && state->mode == SCREEN_MAIN) {
        // Перелистывание в режиме главного меню
        state->selectedIndex = (state->selectedIndex + 1) % (topCount + bottomCount);
        menu_draw(oled, state);
    }

    if (btnB == 0 && state->mode == SCREEN_MAIN) {
        // Подтверждение выбора
        if (state->selectedIndex < topCount)
            state->currentScreen = topMenu[state->selectedIndex][0];
        else
            state->currentScreen = bottomMenu[state->selectedIndex - topCount][0];
        state->mode = SCREEN_SUBMENU;
        menu_draw(oled, state);
    }

    if (btnC == 0 && state->mode == SCREEN_SUBMENU) {
        // Возврат в главное меню
        state->mode = SCREEN_MAIN;
        menu_draw(oled, state);
    }
}