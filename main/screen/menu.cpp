// ======================== menu.cpp ========================
#include "menu.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "ssd1306.h"
#include "../characters/test.h"
#include "../fonts/icons.h"

// Подключаем заголовочные файлы для всех созданных экранов
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
// Простые заглушки для отображения времени и батареи
// ────────────────────────────────────────────────
static void get_time_string(char* buf, size_t len) {
    time_t now;
    struct tm* timeinfo;
    time(&now);
    timeinfo = localtime(&now);
    snprintf(buf, len, "%02d:%02d", timeinfo->tm_hour, timeinfo->tm_min);
}
static int get_battery_percent() {
    return 100; // Заглушка - в реальном проекте нужно реализовать чтение сенсора батареи
}

// ────────────────────────────────────────────────
// ⚙️ Отрисовка символов
// ────────────────────────────────────────────────

// Таблица шрифта (обычно 8 байт на символ)
// Удалено extern объявление, так как теперь массив доступен через ssd1306.h

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
    
    // Если мы находимся в подменю, отображаем соответствующий экран
    if (state->mode == SCREEN_SUBMENU) {
        switch (state->currentScreen) {
            case 'E': // Food
            case 'e':
                draw_screen_food();
                break;
            case 'G': // Game
            case 'g':
                draw_screen_game();
                break;
            case 'M': // Medicine
            case 'm':
                draw_screen_medicine();
                break;
            case 'T': // Toilet
            case 't':
                draw_screen_toilet();
                break;
            case 'C': // Status (Character)
            case 'c':
                draw_screen_status();
                break;
            case 'Y': // Study
            case 'y':
                draw_screen_study();
                break;
            case 'D': // Discipline
            case 'd':
                draw_screen_discipline();
                break;
            case 'S': // Settings
            case 's':
                draw_screen_settings();
                break;
            default:
                // Если неизвестный символ, возвращаемся к главному меню
                ssd1306_clear_screen(oled, false);
                MenuState mainState = { .selectedIndex = 0, .currentScreen = 'E', .mode = SCREEN_MAIN };
                menu_draw(oled, &mainState);
                break;
        }
        return;
    }
    
    // Отрисовка главного меню
    ssd1306_clear_screen(oled, false);

    char topBuf[32] = "";
    char bottomBuf[32] = "";

    // --- Верхний ряд меню ---
    for (int i = 0; i < topCount; i++) {
        char tmp[8];
        if (state->mode == SCREEN_MAIN && state->selectedIndex == i)
            snprintf(tmp, sizeof(tmp), "[%s] ", topMenu[i]);
        else
            snprintf(tmp, sizeof(tmp), " %s  ", topMenu[i]);
        strcat(topBuf, tmp);
    }

    // --- Нижний ряд меню ---
    for (int i = 0; i < bottomCount; i++) {
        char tmp[8];
        if (state->mode == SCREEN_MAIN && state->selectedIndex == i + topCount)
            snprintf(tmp, sizeof(tmp), "[%s] ", bottomMenu[i]);
        else
            snprintf(tmp, sizeof(tmp), " %s  ", bottomMenu[i]);
        strcat(bottomBuf, tmp);
    }

    // --- Отрисовка рядов меню ---
    // Очищаем страницы 0 и 6 перед отрисовкой текста, чтобы убрать артефакты
    memset(oled->_page[0]._segs, 0, 128);
    memset(oled->_page[6]._segs, 0, 128);

    ssd1306_display_text(oled, 0, topBuf, strlen(topBuf), false);
    ssd1306_display_text(oled, 6, bottomBuf, strlen(bottomBuf), false);

    // --- Центр экрана ---
    if (state->mode == SCREEN_MAIN) {
        draw_screen_main(oled);
    } 

    // --- Статус: время + батарея ---
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
// 🔁 Обновление верхнего статуса (часы, батарея)
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
// 🎮 Обработка кнопок меню
// ────────────────────────────────────────────────
void menu_handle_input(SSD1306_t* oled, MenuState* state, int btnA, int btnB, int btnC) {
    if (btnA == 0 && state->mode == SCREEN_MAIN) {
        // Листаем пункты меню
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
