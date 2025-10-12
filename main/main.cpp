#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"

#include "i2cdev.h"
#include "ssd1306.h"

// C++ headers
#include "core/kharebeney_agent.h"
#include "config.h"

// static const char *TAG = "MAIN"; // Not used
static SSD1306_t oled;
static KharebeneyAgent* agent = nullptr;

// --- Инициализация I2C ---
void init_i2c()
{
    ESP_ERROR_CHECK(i2cdev_init());
    printf("I2C initialized on SDA=%d, SCL=%d\n", OLED_SDA_PIN, OLED_SCL_PIN);
}

// --- Инициализация OLED ---
void init_oled()
{
    // Настройка структуры
    memset(&oled, 0, sizeof(SSD1306_t));

    i2c_master_init(&oled, OLED_SDA_PIN, OLED_SCL_PIN, -1);
    i2c_init(&oled, OLED_WIDTH, OLED_HEIGHT);

    ssd1306_clear_screen(&oled, false);
    ssd1306_display_text(&oled, 0, "SSD1306 Ready", 14, true);
    ssd1306_show_buffer(&oled);
    printf("SSD1306 initialized OK\n");
}

// --- Инициализация кнопок ---
void init_buttons()
{
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << BTN_A_PIN) | (1ULL << BTN_B_PIN) |
                        (1ULL << BTN_C_PIN) | (1ULL << BTN_D_PIN),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);
    printf("Buttons configured on pins 12,13,14,15\n");
}

// --- Проверка состояния кнопок ---
void check_buttons()
{
    int a = gpio_get_level((gpio_num_t)BTN_A_PIN);
    int b = gpio_get_level((gpio_num_t)BTN_B_PIN);
    int c = gpio_get_level((gpio_num_t)BTN_C_PIN);
    int d = gpio_get_level((gpio_num_t)BTN_D_PIN);

    printf("Buttons: A=%d B=%d C=%d D=%d\n", a, b, c, d);
}

// --- Функция отображения сообщения ---
void display_message(const char* msg)
{
    printf("%s\n", msg);
    ssd1306_clear_screen(&oled, false);

    if (strlen(msg) > 16) {
        char line1[17];
        char line2[17];
        char line3[17];

        strncpy(line1, msg, 16);
        line1[16] = '\0';

        if (strlen(msg) > 16) {
            strncpy(line2, msg + 16, 16);
            line2[16] = '\0';
        } else {
            line2[0] = '\0';
        }

        if (strlen(msg) > 32) {
            strncpy(line3, msg + 32, 16);
            line3[16] = '\0';
        } else {
            line3[0] = '\0';
        }

        ssd1306_display_text(&oled, 0, line1, 16, false);
        if (line2[0]) ssd1306_display_text(&oled, 1, line2, 16, false);
        if (line3[0]) ssd1306_display_text(&oled, 2, line3, 16, false);
    } else {
        ssd1306_display_text(&oled, 0, msg, strlen(msg), true);
    }

    ssd1306_show_buffer(&oled);
}

// --- Отладочная информация ---
void debug_agent_status(uint32_t step)
{
    if (!agent) return;

    printf("=== DEBUG STEP %u ===\n", (unsigned int)step);
    printf("Action: %s\n", agent->get_current_action() ? agent->get_current_action() : "N/A");

    const InternalStates* states = agent->get_internal_states();
    if (states) {
        printf("Level: %u\n", (unsigned int)states->level);
        printf("Hunger: %.2f\n", states->hunger);
        printf("Happiness: %.2f\n", states->happiness);
        printf("Health: %.2f\n", states->health);
        printf("Energy: %.2f\n", states->energy);
        printf("Social: %.2f\n", states->social);
        printf("Curiosity: %.2f\n", states->curiosity);
        printf("Age: %u\n", (unsigned int)states->age);
    }

    const Emotion* emotion = agent->get_emotional_state();
    if (emotion) {
        printf("Emotion: %s (intensity: %.2f)\n", emotion->name.c_str(), emotion->intensity);
    }

    // Дополнительная информация о системе обучения
    uint32_t level, exp, total_actions, successful_actions;
    float success_rate;
    const float* skills;
    agent->get_learning_system().get_status(&level, &exp, &skills, &total_actions, &successful_actions, &success_rate);
    printf("Learning - Level: %u, Exp: %u, Success rate: %.2f\n", (unsigned int)level, (unsigned int)exp, success_rate);
    printf("Skills: PS:%.2f, SI:%.2f, EI:%.2f, CR:%.2f, AD:%.2f, ME:%.2f, AT:%.2f, PA:%.2f\n",
           skills[0], skills[1], skills[2], skills[3], skills[4], skills[5], skills[6], skills[7]);

    // Выводим статистику памяти
    uint32_t total_memories, retrievals, consolidations;
    agent->get_memory_summary(&total_memories, &retrievals, &consolidations);
    printf("Memory - Total: %u, Retri: %u, Cons: %u\n", (unsigned int)total_memories, (unsigned int)retrievals, (unsigned int)consolidations);

    // Выводим информацию о жизненном цикле
    LifecycleInfo lifecycle = agent->get_lifecycle_info();
    printf("Lifecycle - Stage: %s, Age: %u, Maturity: %.2f\n", lifecycle.stage_name.c_str(), (unsigned int)lifecycle.age, lifecycle.maturity_level);

    printf("=======================\n");
}

// --- Главная функция ---
extern "C" void app_main(void)
{
    init_i2c();
    init_oled();
    init_buttons();

    display_message("Initializing AI...");

    // Создаем агента
    agent = new KharebeneyAgent();
    printf("Agent created\n");

    // Загружаем состояние, если существует
    if (!agent->load_state()) {
        printf("New agent created\n");
        display_message("New agent");
    } else {
        display_message("State loaded");
    }

    display_message("AI Ready");
    vTaskDelay(pdMS_TO_TICKS(1000));

    uint32_t step_count = 0;
    const uint32_t max_steps = 20;  // Ограниченное количество шагов для демонстрации

    while (step_count < max_steps) {
        // Обновляем агента
        const char* action = agent->update();
        step_count++;

        printf("Step %u - Action: %s\n", (unsigned int)step_count, action);

        // Отображаем текущее действие
        char msg[32];
        snprintf(msg, sizeof(msg), "Step %u: %s", (unsigned int)step_count, action);
        display_message(msg);

        // Показываем отладочную информацию каждые 5 шагов
        if (step_count % 5 == 0) {
            debug_agent_status(step_count);
        }

        // Ждем немного между шагами
        vTaskDelay(pdMS_TO_TICKS(500));

        // Проверяем кнопки
        for (int btn_pin : {BTN_A_PIN, BTN_B_PIN, BTN_C_PIN, BTN_D_PIN}) {
            if (gpio_get_level((gpio_num_t)btn_pin) == 0) {  // кнопка нажата
                printf("Button %d pressed!\n", btn_pin);
                
                // Определяем какая кнопка была нажата
                char btn_char = 'X';
                if (btn_pin == BTN_A_PIN) btn_char = 'A';
                else if (btn_pin == BTN_B_PIN) btn_char = 'B';
                else if (btn_pin == BTN_C_PIN) btn_char = 'C';
                else if (btn_pin == BTN_D_PIN) btn_char = 'D';

                // Отображаем информацию о нажатой кнопке
                char btn_msg[32];
                snprintf(btn_msg, sizeof(btn_msg), "Btn %c pressed", btn_char);
                display_message(btn_msg);
                
                // Разные действия для разных кнопок
                if (btn_pin == BTN_A_PIN) {
                    // Триггерим эмоцию при нажатии кнопки A
                    agent->trigger_emotion("joy", 0.7f);
                    printf("Joy emotion triggered\n");
                    // Отображаем текущее состояние агента
                    const InternalStates* states = agent->get_internal_states();
                    if (states) {
                        char state_msg[64];
                        snprintf(state_msg, sizeof(state_msg), "H:%.2f Hp:%.2f E:%.2f",
                                states->hunger, states->happiness, states->energy);
                        printf("Current states: %s\n", state_msg);
                        display_message(state_msg);
                        vTaskDelay(pdMS_TO_TICKS(1000)); // Пауза для просмотра состояния
                    }
                } else if (btn_pin == BTN_B_PIN) {
                    // Сброс базы знаний при нажатии кнопки B
                    agent->reset_knowledge_base();
                    display_message("Knowledge reset!");
                    printf("Knowledge base reset triggered\n");
                } else if (btn_pin == BTN_C_PIN) {
                    // Отображаем расширенную информацию о состоянии агента
                    const InternalStates* states = agent->get_internal_states();
                    const Emotion* emotion = agent->get_emotional_state();
                    if (states && emotion) {
                        char extended_msg[64];
                        snprintf(extended_msg, sizeof(extended_msg), "L:%u E:%.2f Em:%s",
                                (unsigned int)states->level, states->energy, emotion->name.c_str());
                        printf("Extended info: %s\n", extended_msg);
                        display_message(extended_msg);
                        vTaskDelay(pdMS_TO_TICKS(1000)); // Пауза для просмотра информации
                    }
                } else if (btn_pin == BTN_D_PIN) {
                    // Сброс агента к начальному состоянию
                    delete agent;
                    agent = new KharebeneyAgent();
                    display_message("Agent reset!");
                    printf("Agent reset to initial state\n");
                }

                // Ждем отпускания кнопки
                while (gpio_get_level((gpio_num_t)btn_pin) == 0) {
                    vTaskDelay(pdMS_TO_TICKS(10));
                }
                vTaskDelay(pdMS_TO_TICKS(100));  // антидребезг

                // Возвращаем отображение текущего действия после нажатия кнопки
                const char* current_action = agent->get_current_action();
                if (current_action) {
                    char action_msg[32];
                    snprintf(action_msg, sizeof(action_msg), "Action: %s", current_action);
                    display_message(action_msg);
                }

                break;  // Выходим из цикла проверки кнопок
            }
        }

        vTaskDelay(pdMS_TO_TICKS(50));
    }

    // Сохраняем состояние перед выходом
    if (agent->save_state()) {
        printf("State saved\n");
        display_message("State saved");
    } else {
        display_message("Save failed");
    }

    vTaskDelay(pdMS_TO_TICKS(2000));
    display_message("Demo complete");
    printf("Demo complete\n");

    // Вместо завершения программы, переходим в режим ожидания кнопок
    while(1) {
        // Проверяем кнопки в бесконечном цикле после завершения демо
        for (int btn_pin : {BTN_A_PIN, BTN_B_PIN, BTN_C_PIN, BTN_D_PIN}) {
            if (gpio_get_level((gpio_num_t)btn_pin) == 0) {  // кнопка нажата
                printf("Button %d pressed after demo!\n", btn_pin);
                
                // Определяем какая кнопка была нажата
                char btn_char = 'X';
                if (btn_pin == BTN_A_PIN) btn_char = 'A';
                else if (btn_pin == BTN_B_PIN) btn_char = 'B';
                else if (btn_pin == BTN_C_PIN) btn_char = 'C';
                else if (btn_pin == BTN_D_PIN) btn_char = 'D';

                // Отображаем информацию о нажатой кнопке
                char btn_msg[32];
                snprintf(btn_msg, sizeof(btn_msg), "Btn %c pressed", btn_char);
                display_message(btn_msg);
                
                // Разные действия для разных кнопок
                if (btn_pin == BTN_A_PIN) {
                    // Триггерим эмоцию при нажатии кнопки A
                    agent->trigger_emotion("joy", 0.7f);
                    printf("Joy emotion triggered\n");
                    // Отображаем текущее состояние агента
                    const InternalStates* states = agent->get_internal_states();
                    if (states) {
                        char state_msg[64];
                        snprintf(state_msg, sizeof(state_msg), "H:%.2f Hp:%.2f E:%.2f",
                                states->hunger, states->happiness, states->energy);
                        printf("Current states: %s\n", state_msg);
                        display_message(state_msg);
                        vTaskDelay(pdMS_TO_TICKS(1000)); // Пауза для просмотра состояния
                    }
                } else if (btn_pin == BTN_B_PIN) {
                    // Сброс базы знаний при нажатии кнопки B
                    agent->reset_knowledge_base();
                    display_message("Knowledge reset!");
                    printf("Knowledge base reset triggered\n");
                } else if (btn_pin == BTN_C_PIN) {
                    // Отображаем расширенную информацию о состоянии агента
                    const InternalStates* states = agent->get_internal_states();
                    const Emotion* emotion = agent->get_emotional_state();
                    if (states && emotion) {
                        char extended_msg[64];
                        snprintf(extended_msg, sizeof(extended_msg), "L:%u E:%.2f Em:%s",
                                (unsigned int)states->level, states->energy, emotion->name.c_str());
                        printf("Extended info: %s\n", extended_msg);
                        display_message(extended_msg);
                        vTaskDelay(pdMS_TO_TICKS(1000)); // Пауза для просмотра информации
                    }
                } else if (btn_pin == BTN_D_PIN) {
                    // Сброс агента к начальному состоянию
                    delete agent;
                    agent = new KharebeneyAgent();
                    display_message("Agent reset!");
                    printf("Agent reset to initial state\n");
                }

                // Ждем отпускания кнопки
                while (gpio_get_level((gpio_num_t)btn_pin) == 0) {
                    vTaskDelay(pdMS_TO_TICKS(10));
                }
                vTaskDelay(pdMS_TO_TICKS(100));  // антидребезг

                // Возвращаем отображение текущего действия после нажатия кнопки
                const char* current_action = agent->get_current_action();
                if (current_action) {
                    char action_msg[32];
                    snprintf(action_msg, sizeof(action_msg), "Action: %s", current_action);
                    display_message(action_msg);
                }

                break;  // Выходим из цикла проверки кнопок
            }
        }

        // Небольшая задержка для экономии ресурсов процессора
        vTaskDelay(pdMS_TO_TICKS(50));
    }

    // Очищаем память
    delete agent;
    agent = nullptr;
}
