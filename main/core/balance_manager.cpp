#include "balance_manager.h"
#include <esp_timer.h>
#include <algorithm>
#include <cstring>

const char* BalanceManager::TAG = "BalanceManager";

BalanceManager::BalanceManager() :
    exploration_rate(0.3f),
    exploitation_rate(0.7f),
    max_history(MAX_ACTION_HISTORY),
    adaptation_rate(ADAPTATION_RATE)
{
    // Приоритеты состояний
    state_priorities[0] = 0.9f; // hunger
    state_priorities[1] = 0.8f; // health
    state_priorities[2] = 0.7f; // energy
    state_priorities[3] = 0.6f; // happiness
    state_priorities[4] = 0.5f; // social
    state_priorities[5] = 0.4f; // curiosity

    last_update_time = esp_timer_get_time() / 1000;

    printf("BalanceManager initialized\n");
}

void BalanceManager::update() {
    uint32_t current_time = esp_timer_get_time() / 1000;
    // uint32_t time_delta = current_time - last_update_time; // Not used

    // Адаптируем баланс на основе истории
    if (action_history.size() > 5) {
        // Анализируем последние действия
        size_t recent_count = std::min((size_t)10, action_history.size());
        auto recent_actions = std::vector<ActionRecord>(
            action_history.end() - recent_count,
            action_history.end()
        );

        const char* exploration_actions[] = {"explore", "play"};
        const char* exploitation_actions[] = {"feed", "heal", "rest", "socialize"};

        int exploration_success = 0, exploration_count = 0;
        int exploitation_success = 0, exploitation_count = 0;

        for (const auto& record : recent_actions) {
            bool is_exploration = false;
            for (const char* action : exploration_actions) {
                if (record.action == action) {
                    is_exploration = true;
                    break;
                }
            }

            if (is_exploration) {
                exploration_count++;
                if (record.success) exploration_success++;
            } else {
                bool is_exploitation = false;
                for (const char* action : exploitation_actions) {
                    if (record.action == action) {
                        is_exploitation = true;
                        break;
                    }
                }
                if (is_exploitation) {
                    exploitation_count++;
                    if (record.success) exploitation_success++;
                }
            }
        }

        // Адаптируем баланс
        if (exploration_count > 0 && exploitation_count > 0) {
            float exploration_rate_val = (float)exploration_success / exploration_count;
            float exploitation_rate_val = (float)exploitation_success / exploitation_count;

            if (exploration_rate_val > exploitation_rate_val) {
                // Исследования более успешны
                exploration_rate = std::min(0.5f, exploration_rate + adaptation_rate);
                exploitation_rate = 1.0f - exploration_rate;
            } else if (exploitation_rate_val > exploration_rate_val) {
                // Эксплуатация более успешна
                exploitation_rate = std::min(0.8f, exploitation_rate + adaptation_rate);
                exploration_rate = 1.0f - exploitation_rate;
            }
        }
    }

    // Ограничиваем историю
    if (action_history.size() > max_history) {
        action_history.erase(action_history.begin(), action_history.end() - max_history);
    }

    last_update_time = current_time;
}

float BalanceManager::get_action_priority(const char* action, const float* states) const {
    float base_priority = 0.5f;

    // Приоритет на основе состояний
    if (strcmp(action, "feed") == 0 && states[0] > 0.7f) { // hunger
        base_priority += 0.3f;
    } else if (strcmp(action, "heal") == 0 && states[1] < 0.4f) { // health
        base_priority += 0.3f;
    } else if (strcmp(action, "rest") == 0 && states[2] < 0.3f) { // energy
        base_priority += 0.3f;
    } else if (strcmp(action, "play") == 0 && states[3] < 0.4f) { // happiness
        base_priority += 0.2f;
    } else if (strcmp(action, "socialize") == 0 && states[4] < 0.3f) { // social
        base_priority += 0.2f;
    } else if (strcmp(action, "explore") == 0 && states[5] > 0.7f) { // curiosity
        base_priority += 0.2f;
    }

    // Учитываем баланс исследование/эксплуатация
    if (strcmp(action, "explore") == 0 || strcmp(action, "play") == 0) {
        base_priority *= exploration_rate;
    } else {
        base_priority *= exploitation_rate;
    }

    return std::max(0.0f, std::min(1.0f, base_priority));
}

void BalanceManager::adjust_social_priority(float adjustment) {
    // Регулируем приоритет социальных действий
    state_priorities[4] = std::max(0.1f, std::min(0.9f, state_priorities[4] + adjustment));
}

void BalanceManager::record_action(const char* action, bool success) {
    ActionRecord record;
    record.action = action;
    record.success = success;
    record.timestamp = esp_timer_get_time() / 1000;

    action_history.push_back(record);

    // Ограничиваем историю
    if (action_history.size() > max_history) {
        action_history.erase(action_history.begin());
    }
}

size_t BalanceManager::serialize(uint8_t* buffer, size_t buffer_size) const {
    size_t required_size = sizeof(float) * 2 + sizeof(float) * 6 + sizeof(uint32_t) +
                          action_history.size() * (sizeof(uint32_t) + 1 + 20); // approx string size

    if (buffer_size < required_size || !buffer) {
        return required_size;
    }

    uint8_t* ptr = buffer;

    // Сохраняем rates
    memcpy(ptr, &exploration_rate, sizeof(float));
    ptr += sizeof(float);
    memcpy(ptr, &exploitation_rate, sizeof(float));
    ptr += sizeof(float);

    // Сохраняем priorities
    memcpy(ptr, state_priorities, sizeof(float) * 6);
    ptr += sizeof(float) * 6;

    // Сохраняем историю
    uint32_t history_size = action_history.size();
    memcpy(ptr, &history_size, sizeof(uint32_t));
    ptr += sizeof(uint32_t);

    for (const auto& record : action_history) {
        uint32_t timestamp = record.timestamp;
        uint8_t success = record.success ? 1 : 0;
        uint8_t action_len = std::min((size_t)19, record.action.length());

        memcpy(ptr, &timestamp, sizeof(uint32_t));
        ptr += sizeof(uint32_t);
        memcpy(ptr, &success, sizeof(uint8_t));
        ptr += sizeof(uint8_t);
        memcpy(ptr, &action_len, sizeof(uint8_t));
        ptr += sizeof(uint8_t);
        memcpy(ptr, record.action.c_str(), action_len);
        ptr += action_len;
    }

    return ptr - buffer;
}

bool BalanceManager::deserialize(const uint8_t* buffer, size_t buffer_size) {
    if (buffer_size < sizeof(float) * 8 + sizeof(uint32_t) || !buffer) {
        return false;
    }

    const uint8_t* ptr = buffer;

    // Загружаем rates
    memcpy(&exploration_rate, ptr, sizeof(float));
    ptr += sizeof(float);
    memcpy(&exploitation_rate, ptr, sizeof(float));
    ptr += sizeof(float);

    // Загружаем priorities
    memcpy(state_priorities, ptr, sizeof(float) * 6);
    ptr += sizeof(float) * 6;

    // Загружаем историю
    uint32_t history_size;
    memcpy(&history_size, ptr, sizeof(uint32_t));
    ptr += sizeof(uint32_t);

    action_history.clear();
    action_history.reserve(std::min((uint32_t)max_history, history_size));

    for (uint32_t i = 0; i < history_size && i < max_history; ++i) {
        if (ptr - buffer + sizeof(uint32_t) + 2 > buffer_size) break;

        ActionRecord record;
        memcpy(&record.timestamp, ptr, sizeof(uint32_t));
        ptr += sizeof(uint32_t);

        uint8_t success;
        memcpy(&success, ptr, sizeof(uint8_t));
        ptr += sizeof(uint8_t);
        record.success = success != 0;

        uint8_t action_len;
        memcpy(&action_len, ptr, sizeof(uint8_t));
        ptr += sizeof(uint8_t);

        if (ptr - buffer + action_len > buffer_size) break;

        record.action.assign((const char*)ptr, action_len);
        ptr += action_len;

        action_history.push_back(record);
    }

    return true;
}