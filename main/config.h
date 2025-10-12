#ifndef CONFIG_H
#define CONFIG_H

// --- Настройки дисплея ---
#define OLED_WIDTH      128
#define OLED_HEIGHT     64
#define OLED_ADDR       0x3C
#define OLED_SDA_PIN    21
#define OLED_SCL_PIN    22

// --- Настройки кнопок ---
#define BTN_A_PIN  12
#define BTN_B_PIN  13
#define BTN_C_PIN  14
#define BTN_D_PIN  15

// --- Настройки агента ---
#define SAVE_INTERVAL 30
#define MAX_EMOTIONS 12
#define MAX_STATES 9
#define MAX_KNOWLEDGE 50
#define MAX_MEMORIES 30
#define MAX_PROMPTS 10
#define MAX_ACTION_HISTORY 20
#define MAX_SUCCESS_HISTORY 50

// --- Настройки систем ---
#define STATE_UPDATE_INTERVAL_MS 100
#define EMOTION_DECAY_RATE 0.01f
#define MAX_ACTIVE_EMOTIONS 6
#define ADAPTATION_RATE 0.01f
#define LEARNING_RATE 0.01f
#define SKILL_DECAY_RATE 0.001f

// --- Настройки Q-learning ---
#define Q_LEARNING_RATE 0.1f
#define Q_DISCOUNT_FACTOR 0.9f
#define Q_EXPLORATION_RATE 0.3f

// --- Настройки отладки ---
#define DEBUG_ENABLED 1
#define MAX_STEPS_DEFAULT 20

#endif // CONFIG_H