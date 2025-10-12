// ======================== main.cpp ========================
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "i2cdev.h"
#include "ssd1306.h"
#include "config.h"
#include "menu.h"
#include "esp_mac.h"

static SSD1306_t oled;
static MenuState menuState;

void init_i2c() {
    i2cdev_init();
}

void init_oled() {
    memset(&oled, 0, sizeof(SSD1306_t));
    i2c_master_init(&oled, (gpio_num_t)OLED_SDA_PIN, (gpio_num_t)OLED_SCL_PIN, (gpio_num_t)-1);
    i2c_init(&oled, OLED_WIDTH, OLED_HEIGHT);
    ssd1306_clear_screen(&oled, false);
}

void init_buttons() {
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << BTN_A_PIN) | (1ULL << BTN_B_PIN) | (1ULL << BTN_C_PIN),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);
}

extern "C" void app_main(void) {
    init_i2c();
    init_oled();
    init_buttons();

    menu_init(&oled);
    menuState.selectedIndex = 0;
    menuState.currentScreen = 'E';
    menuState.mode = SCREEN_MAIN;

    int lastA = 1, lastB = 1, lastC = 1;
    int currentA, currentB, currentC;

    while (1) {
        currentA = gpio_get_level((gpio_num_t)BTN_A_PIN);
        currentB = gpio_get_level((gpio_num_t)BTN_B_PIN);
        currentC = gpio_get_level((gpio_num_t)BTN_C_PIN);

        if (lastA == 1 && currentA == 0) {
            menu_handle_input(&oled, &menuState, 0, 1, 1);
            vTaskDelay(pdMS_TO_TICKS(120));
        }
        if (lastB == 1 && currentB == 0) {
            menu_handle_input(&oled, &menuState, 1, 0, 1);
            vTaskDelay(pdMS_TO_TICKS(120));
        }
        if (lastC == 1 && currentC == 0) {
            menu_handle_input(&oled, &menuState, 1, 1, 0);
            vTaskDelay(pdMS_TO_TICKS(120));
        }

        lastA = currentA;
        lastB = currentB;
        lastC = currentC;

        vTaskDelay(pdMS_TO_TICKS(20));
    }
}