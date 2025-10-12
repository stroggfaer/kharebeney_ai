#ifndef BALANCE_MANAGER_H
#define BALANCE_MANAGER_H

#include <stdint.h>
#include <string.h>
#include <vector>
#include <string>
#include "../config.h"

// Структура для записи действия
typedef struct {
    std::string action;
    bool success;
    uint32_t timestamp;
} ActionRecord;

class BalanceManager {
private:
    float exploration_rate;
    float exploitation_rate;
    float state_priorities[6]; // hunger, health, energy, happiness, social, curiosity
    std::vector<ActionRecord> action_history;
    uint32_t max_history;
    uint32_t last_update_time;
    float adaptation_rate;

    static const char* TAG;

public:
    BalanceManager();
    ~BalanceManager() = default;

    void update();
    float get_action_priority(const char* action, const float* states) const;
    void record_action(const char* action, bool success);

    float get_exploration_rate() const { return exploration_rate; }
    float get_exploitation_rate() const { return exploitation_rate; }
    size_t get_history_count() const { return action_history.size(); }

    // Сериализация
    size_t serialize(uint8_t* buffer, size_t buffer_size) const;
    bool deserialize(const uint8_t* buffer, size_t buffer_size);
};

#endif // BALANCE_MANAGER_H