// ======================== menu.cpp ========================
#include "menu.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "ssd1306.h"
#include "../characters/test.h"
#include "../fonts/icons.h"

// Подключаем заголовочные файлы экранов
#include "food/screen_food.h"
#include "game/screen_game.h"
#include "medicine/screen_medicine.h"
#include "toilet/screen_toilet.h"
#include "status/screen_status.h"
#include "study/screen_study.h"
#include "discipline/screen_discipline.h"
#include "settings/screen_settings.h"
#include "main/screen_main.h"
#include "../lib/utlis.h"

// ────────────────────────────────────────────────
// Меню: верхний и нижний ряды пунктов
// ────────────────────────────────────────────────
static const char* topMenu[] = {"E", "G", "M", "T"};
static const char* bottomMenu[] = {"C", "Y", "D", "S"};
static const int topCount = 4;
static const int bottomCount = 4;

// ────────────────────────────────────────────────
// Время и батарея (заглушки)
// ────────────────────────────────────────────────
static void get_time_string(char* buf, size_t len) {
    time_t now;
    struct tm* timeinfo;
    time(&now);
    timeinfo = localtime(&now);
    snprintf(buf, len, "%02d:%02d", timeinfo->tm_hour, timeinfo->tm_min);
}
static int get_battery_percent() {
    return 100; // Заглушка: заменить на реальное чтение батареи
}

// ────────────────────────────────────────────────
// Отрисовка верхнего и нижнего меню
// ────────────────────────────────────────────────
static void draw_top_bottom_menu(SSD1306_t* oled, const MenuState* state) {
    char topBuf[32] = "";
    char bottomBuf[32] = "";

    // Верхний ряд меню
    for (int i = 0; i < topCount; i++) {
        char tmp[8];
        if (state->mode == SCREEN_MAIN && state->selectedIndex == i)
            snprintf(tmp, sizeof(tmp), "[%s] ", topMenu[i]);
        else
            snprintf(tmp, sizeof(tmp), " %s  ", topMenu[i]);
        strcat(topBuf, tmp);
    }

    // Нижний ряд меню
    for (int i = 0; i < bottomCount; i++) {
        char tmp[8];
        if (state->mode == SCREEN_MAIN && state->selectedIndex == i + topCount)
            snprintf(tmp, sizeof(tmp), "[%s] ", bottomMenu[i]);
        else
            snprintf(tmp, sizeof(tmp), " %s  ", bottomMenu[i]);
        strcat(bottomBuf, tmp);
    }

    // Чистим только строки, где меню (чтобы избежать мерцания)
    memset(oled->_page[0]._segs, 0, 128);
    memset(oled->_page[6]._segs, 0, 128);

    ssd1306_display_text(oled, 0, topBuf, strlen(topBuf), false);
    ssd1306_display_text(oled, 6, bottomBuf, strlen(bottomBuf), false);
}

// ────────────────────────────────────────────────
// 🧩 Инициализация меню
// ────────────────────────────────────────────────
void menu_init(SSD1306_t* oled) {
    ssd1306_clear_screen(oled, false);
    MenuState initState = { .selectedIndex = 0, .currentScreen = 'E', .mode = SCREEN_MAIN };
    menu_draw(oled, &initState);
}

// ────────────────────────────────────────────────
// 🎨 Основная функция отрисовки меню
// ────────────────────────────────────────────────
void menu_draw(SSD1306_t* oled, const MenuState* state) {
   // bool showMenu = should_show_menu(state->currentScreen);
    // Подменю (экраны)
    if (state->mode == SCREEN_SUBMENU) {
        // Перед отрисовкой экрана — показать верхнее и нижнее меню, кроме Settings
        if (state->currentScreen != 'S' && state->currentScreen != 's') {
            // Рисуем верхнее и нижнее меню статически
            char topBuf[32] = "";
            char bottomBuf[32] = "";

            for (int i = 0; i < topCount; i++) {
                char tmp[8];
                if (topMenu[i][0] == state->currentScreen)
                    snprintf(tmp, sizeof(tmp), "[%s] ", topMenu[i]);
                else
                    snprintf(tmp, sizeof(tmp), " %s  ", topMenu[i]);
                strcat(topBuf, tmp);
            }

            for (int i = 0; i < bottomCount; i++) {
                char tmp[8];
                if (bottomMenu[i][0] == state->currentScreen)
                    snprintf(tmp, sizeof(tmp), "[%s] ", bottomMenu[i]);
                else
                    snprintf(tmp, sizeof(tmp), " %s  ", bottomMenu[i]);
                strcat(bottomBuf, tmp);
            }

            ssd1306_display_text(oled, 0, topBuf, strlen(topBuf), false);
            ssd1306_display_text(oled, 6, bottomBuf, strlen(bottomBuf), false);

            // Обновляем статус (часы, батарея)
            menu_update_status(oled);
        }

        // Теперь рисуем содержимое выбранного экрана
        switch (state->currentScreen) {
            case 'E': draw_screen_food(oled); break;
            case 'G': draw_screen_game(oled); break;
            case 'M': draw_screen_medicine(oled); break;
            case 'T': draw_screen_toilet(oled); break;
            case 'C': draw_screen_status(oled); break;
            case 'Y': draw_screen_study(oled); break;
            case 'D': draw_screen_discipline(oled); break;
            case 'S': draw_screen_settings(oled); break;
            default:
                ssd1306_clear_screen(oled, false);
                MenuState mainState = { .selectedIndex = 0, .currentScreen = 'E', .mode = SCREEN_MAIN };
                menu_draw(oled, &mainState);
                break;
        }

        ssd1306_show_buffer(oled);
        return;
    }
    // Главный экран
    ssd1306_clear_screen(oled, false);
    draw_top_bottom_menu(oled, state);
    draw_screen_main(oled);

    // Время + батарея
    char timeBuf[16], battBuf[8], status[32];
    get_time_string(timeBuf, sizeof(timeBuf));
    snprintf(battBuf, sizeof(battBuf), "%d%%", get_battery_percent());
    snprintf(status, sizeof(status), "%s %s", timeBuf, battBuf);
    ssd1306_display_text(oled, 1, status, strlen(status), false);

    ssd1306_show_buffer(oled);
}






// ────────────────────────────────────────────────
// 🔁 Обновление статуса (часы + батарея)
// ────────────────────────────────────────────────
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

// ────────────────────────────────────────────────
// 🎮 Обработка кнопок
// ────────────────────────────────────────────────
void menu_handle_input(SSD1306_t* oled, MenuState* state, int btnA, int btnB, int btnC) {
    if (btnA == 0 && state->mode == SCREEN_MAIN) {
        // Переключение пунктов меню
        state->selectedIndex = (state->selectedIndex + 1) % (topCount + bottomCount);
        menu_draw(oled, state);
    }

    if (btnB == 0 && state->mode == SCREEN_MAIN) {
        // Переход в подменю
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
