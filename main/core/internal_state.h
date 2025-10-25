#ifndef INTERNAL_STATE_H
#define INTERNAL_STATE_H

#include <stdint.h>
#include <string.h>
#include <esp_log.h>
#include "../config.h"

// Структура для хранения состояний
typedef struct {
    float hunger;      // Голод (0.0 - 1.0)
    float happiness;   // Счастье (0.0 - 1.0)
    float health;      // Здоровье (0.0 - 1.0)
    float energy;      // Энергия (0.0 - 1.0)
    float social;      // Социальность (0.0 - 1.0)
    float curiosity;   // Любопытство (0.0 - 1.0)
    uint32_t level;    // Уровень
    uint32_t age;      // Возраст в секундах
    uint32_t time;     // Время в секундах
} InternalStates;

// Эффекты действий на состояния
typedef struct {
    float hunger;
    float happiness;
    float health;
    float energy;
    float social;
    float curiosity;
} ActionEffect;

class InternalState {
private:
    InternalStates states;
    float decay_rates[MAX_STATES];
    ActionEffect action_effects[7]; // 7 действий
    uint32_t last_update_time;
    uint32_t update_interval;

    static const char* TAG;

    void normalize_states();
    void check_level_up();

public:
    InternalState();
    ~InternalState() = default;

    void update();
    void perform_action(const char* action);
    void boost_energy(float amount);
    void reduce_energy(float amount);
    const InternalStates* get_states() const { return &states; }
    void set_states(const InternalStates* new_states);
    uint32_t get_critical_states() const;
    const char* get_priority_action() const;

    // Сериализация для сохранения
    size_t serialize(uint8_t* buffer, size_t buffer_size) const;
    bool deserialize(const uint8_t* buffer, size_t buffer_size);
};

#endif // INTERNAL_STATE_H