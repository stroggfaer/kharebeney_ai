#include "internal_state.h"
#include <esp_timer.h>
#include <algorithm>
#include <cmath>

const char* InternalState::TAG = "InternalState";

InternalState::InternalState() {
    // Инициализация состояний
    states.hunger = 0.5f;
    states.happiness = 0.5f;
    states.health = 0.8f;
    states.energy = 0.7f;
    states.social = 0.5f;
    states.curiosity = 0.6f;
    states.level = 1;
    states.age = 0;
    states.time = 0;

    // Скорости затухания
    decay_rates[0] = 0.01f;  // hunger (увеличивается)
    decay_rates[1] = 0.005f; // happiness
    decay_rates[2] = 0.002f; // health
    decay_rates[3] = 0.008f; // energy
    decay_rates[4] = 0.003f; // social
    decay_rates[5] = 0.004f; // curiosity

    // Эффекты действий
    // feed
    action_effects[0] = {-0.3f, 0.1f, 0.05f, 0.0f, 0.0f, 0.0f};
    // play
    action_effects[1] = {0.0f, 0.2f, 0.0f, -0.1f, 0.0f, 0.1f};
    // heal
    action_effects[2] = {0.0f, 0.0f, 0.3f, 0.1f, 0.0f, 0.0f};
    // rest
    action_effects[3] = {0.0f, 0.05f, 0.1f, 0.4f, 0.0f, 0.0f};
    // socialize
    action_effects[4] = {0.0f, 0.15f, 0.0f, -0.05f, 0.2f, 0.0f};
    // explore
    action_effects[5] = {0.0f, 0.1f, 0.0f, -0.1f, 0.0f, 0.2f};
    // wait
    action_effects[6] = {0.0f, 0.0f, 0.0f, 0.1f, 0.0f, 0.0f};

    last_update_time = esp_timer_get_time() / 1000;
    update_interval = STATE_UPDATE_INTERVAL_MS;

    printf("InternalState initialized\n");
}

void InternalState::update() {
    uint32_t current_time = esp_timer_get_time() / 1000;
    uint32_t time_delta = current_time - last_update_time;

    if (time_delta >= update_interval) {
        // Применяем затухание
        states.hunger = std::min(1.0f, states.hunger + decay_rates[0] * (time_delta / 1000.0f));
        states.happiness = std::max(0.0f, states.happiness - decay_rates[1] * (time_delta / 1000.0f));
        states.health = std::max(0.0f, states.health - decay_rates[2] * (time_delta / 1000.0f));
        states.energy = std::max(0.0f, states.energy - decay_rates[3] * (time_delta / 1000.0f));
        states.social = std::max(0.0f, states.social - decay_rates[4] * (time_delta / 1000.0f));
        states.curiosity = std::max(0.0f, states.curiosity - decay_rates[5] * (time_delta / 1000.0f));

        // Обновляем время
        states.time += time_delta / 1000;
        states.age += time_delta / 1000;

        check_level_up();
        last_update_time = current_time;
    }
}

void InternalState::perform_action(const char* action) {
    int action_index = -1;

    // Определяем индекс действия
    if (strcmp(action, "feed") == 0) action_index = 0;
    else if (strcmp(action, "play") == 0) action_index = 1;
    else if (strcmp(action, "heal") == 0) action_index = 2;
    else if (strcmp(action, "rest") == 0) action_index = 3;
    else if (strcmp(action, "socialize") == 0) action_index = 4;
    else if (strcmp(action, "explore") == 0) action_index = 5;
    else if (strcmp(action, "wait") == 0) action_index = 6;

    if (action_index >= 0) {
        ActionEffect effect = action_effects[action_index];

        // Применяем эффекты
        if (action_index == 0) { // feed - hunger уменьшается
            states.hunger = std::max(0.0f, states.hunger + effect.hunger);
        } else {
            states.hunger = std::min(1.0f, states.hunger + effect.hunger);
        }

        states.happiness = std::min(1.0f, states.happiness + effect.happiness);
        states.health = std::min(1.0f, states.health + effect.health);
        states.energy = std::min(1.0f, states.energy + effect.energy);
        states.social = std::min(1.0f, states.social + effect.social);
        states.curiosity = std::min(1.0f, states.curiosity + effect.curiosity);

        normalize_states();
        printf("Action performed: %s\n", action);
    }
}

void InternalState::set_states(const InternalStates* new_states) {
    if (new_states) {
        memcpy(&states, new_states, sizeof(InternalStates));
        normalize_states();
        printf("States set\n");
    }
}

uint32_t InternalState::get_critical_states() const {
    uint32_t critical = 0;

    if (states.hunger > 0.8f) critical |= (1 << 0); // hungry
    if (states.health < 0.3f) critical |= (1 << 1); // sick
    if (states.energy < 0.2f) critical |= (1 << 2); // tired
    if (states.happiness < 0.2f) critical |= (1 << 3); // sad

    return critical;
}

const char* InternalState::get_priority_action() const {
    uint32_t critical = get_critical_states();

    if (critical & (1 << 0)) return "feed";      // hungry
    if (critical & (1 << 1)) return "heal";      // sick
    if (critical & (1 << 2)) return "rest";      // tired
    if (critical & (1 << 3)) return "play";      // sad

    if (states.curiosity > 0.7f) return "explore";
    if (states.social < 0.3f) return "socialize";

    return "play";
}

void InternalState::normalize_states() {
    // Ограничиваем значения
    states.hunger = std::max(0.0f, std::min(1.0f, states.hunger));
    states.happiness = std::max(0.0f, std::min(1.0f, states.happiness));
    states.health = std::max(0.0f, std::min(1.0f, states.health));
    states.energy = std::max(0.0f, std::min(1.0f, states.energy));
    states.social = std::max(0.0f, std::min(1.0f, states.social));
    states.curiosity = std::max(0.0f, std::min(1.0f, states.curiosity));
}

void InternalState::check_level_up() {
    uint32_t required_time = states.level * 100;

    if (states.age >= required_time) {
        states.level++;
        printf("Level up! New level: %u\n", (unsigned int)states.level);

        // Бонус за уровень
        states.health = std::min(1.0f, states.health + 0.1f);
        states.energy = std::min(1.0f, states.energy + 0.1f);
        states.happiness = std::min(1.0f, states.happiness + 0.1f);
    }
}

size_t InternalState::serialize(uint8_t* buffer, size_t buffer_size) const {
    if (buffer_size < sizeof(InternalStates)) {
        return 0;
    }

    memcpy(buffer, &states, sizeof(InternalStates));
    return sizeof(InternalStates);
}

bool InternalState::deserialize(const uint8_t* buffer, size_t buffer_size) {
    if (buffer_size < sizeof(InternalStates)) {
        return false;
    }

    memcpy(&states, buffer, sizeof(InternalStates));
    normalize_states();
    return true;
}