#include "learning_system.h"
#include <esp_timer.h>
#include <algorithm>
#include <cstring>

const char* LearningSystem::TAG = "LearningSystem";

LearningSystem::LearningSystem() :
    experience_points(0),
    learning_level(1),
    total_actions(0),
    successful_actions(0),
    max_history(MAX_SUCCESS_HISTORY),
    learning_rate(LEARNING_RATE),
    skill_decay_rate(SKILL_DECAY_RATE),
    last_update(esp_timer_get_time() / 1000)
{
    // Инициализация навыков
    skills[0] = 0.5f; // problem_solving
    skills[1] = 0.5f; // social_interaction
    skills[2] = 0.5f; // emotional_intelligence
    skills[3] = 0.5f; // creativity
    skills[4] = 0.5f; // adaptability
    skills[5] = 0.5f; // memory
    skills[6] = 0.5f; // attention
    skills[7] = 0.5f; // patience

    printf("LearningSystem initialized\n");
}

void LearningSystem::update() {
    uint32_t current_time = esp_timer_get_time() / 1000;
    uint32_t time_delta = current_time - last_update;

    // Затухание навыков
    for (int i = 0; i < 8; ++i) {
        if (skills[i] > 0.1f) {
            skills[i] = std::max(0.1f, skills[i] - skill_decay_rate * time_delta / 1000.0f);
        }
    }

    // Ограничиваем историю
    if (success_history.size() > max_history) {
        success_history.erase(success_history.begin(), success_history.end() - max_history);
    }

    last_update = current_time;
}

void LearningSystem::adjust_learning_rate(float adjustment) {
    // Изменяем скорость обучения с ограничениями
    learning_rate = std::max(0.001f, std::min(0.05f, learning_rate + adjustment));
}

void LearningSystem::learn_from_experience(const char* action, bool success, const float* pre_states, const float* post_states) {
    // Анализируем успех на основе изменения состояний, если доступны
    bool actual_success = success;
    if (pre_states && post_states) {
        actual_success = analyze_success(action, pre_states, post_states);
    }

    total_actions++;
    if (actual_success) successful_actions++;

    float exp_gain = calculate_experience_gain(action, actual_success, pre_states);
    experience_points += (uint32_t)exp_gain;

    update_skills(action, actual_success);
    check_level_up();

    SuccessRecord record;
    record.action = action;
    record.success = actual_success;
    record.timestamp = esp_timer_get_time() / 1000;
    record.experience_gain = exp_gain;

    success_history.push_back(record);

    // Ограничиваем размер истории для экономии памяти
    if (success_history.size() > max_history) {
        success_history.erase(success_history.begin());
    }

    printf("Learning: %s, success: %d, exp: +%.2f\n", action, actual_success, exp_gain);
}

float LearningSystem::calculate_experience_gain(const char* action, bool success, const float* context) const {
    float base_exp = 1.0f;

    // Множители для разных действий
    float multipliers[] = {1.2f, 1.5f, 1.3f, 1.1f, 0.8f, 0.5f}; // play, learn, explore, socialize, rest, wait
    const char* actions[] = {"play", "learn", "explore", "socialize", "rest", "wait"};

    for (size_t i = 0; i < sizeof(actions)/sizeof(actions[0]); ++i) {
        if (strcmp(action, actions[i]) == 0) {
            base_exp *= multipliers[i];
            break;
        }
    }

    if (success) base_exp *= 1.5f;

    if (context && context[0] < 0.5f) base_exp *= 1.2f; // бонус за сложность

    return base_exp * learning_rate;
}

void LearningSystem::update_skills(const char* action, bool success) {
    // Маппинг действий на навыки
    const char* action_skill_map[][3] = {
        {"play", "creativity", "attention"},
        {"learn", "problem_solving", "memory"},
        {"explore", "curiosity", "adaptability"},
        {"socialize", "social_interaction", "emotional_intelligence"},
        {"rest", "patience", "emotional_intelligence"},
        {"wait", "patience", "attention"}
    };

    // const char* skill_names[] = {"creativity", "attention", "problem_solving", "memory",
    //                             "curiosity", "adaptability", "social_interaction", "emotional_intelligence",
    //                             "patience"}; // Not used

    int skill_indices[][2] = {
        {3, 6}, // play: creativity, attention
        {0, 5}, // learn: problem_solving, memory
        {4, 4}, // explore: curiosity, adaptability (curiosity не в skills, используем adaptability)
        {1, 2}, // socialize: social_interaction, emotional_intelligence
        {7, 2}, // rest: patience, emotional_intelligence
        {7, 6}  // wait: patience, attention
    };

    for (size_t i = 0; i < sizeof(action_skill_map)/sizeof(action_skill_map[0]); ++i) {
        if (strcmp(action, action_skill_map[i][0]) == 0) {
            float skill_gain = success ? 0.01f : 0.005f;
            skills[skill_indices[i][0]] = std::min(1.0f, skills[skill_indices[i][0]] + skill_gain);
            skills[skill_indices[i][1]] = std::min(1.0f, skills[skill_indices[i][1]] + skill_gain);
            break;
        }
    }
}

void LearningSystem::check_level_up() {
    uint32_t required_exp = learning_level * 100;

    if (experience_points >= required_exp) {
        learning_level++;
        printf("Learning level up! New level: %u\n", (unsigned int)learning_level);

        // Бонус за уровень
        for (int i = 0; i < 8; ++i) {
            skills[i] = std::min(1.0f, skills[i] + 0.05f);
        }
    }
}

std::vector<std::string> LearningSystem::get_recommendations() const {
    std::vector<std::string> recommendations;

    for (int i = 0; i < 8; ++i) {
        if (skills[i] < 0.3f) {
            const char* skill_names[] = {"problem_solving", "social_interaction", "emotional_intelligence",
                                        "creativity", "adaptability", "memory", "attention", "patience"};
            recommendations.push_back(std::string("Develop skill: ") + skill_names[i]);
        } else if (skills[i] > 0.8f) {
            const char* skill_names[] = {"problem_solving", "social_interaction", "emotional_intelligence",
                                        "creativity", "adaptability", "memory", "attention", "patience"};
            recommendations.push_back(std::string("Skill ") + skill_names[i] + " is well developed");
        }
    }

    if (experience_points < 100) {
        recommendations.push_back("Experiment more");
    }

    return recommendations;
}

LearningInsights LearningSystem::get_insights() const {
    LearningInsights insights;

    if (!success_history.empty()) {
        std::unordered_map<std::string, std::pair<int, int>> action_stats;

        for (const auto& record : success_history) {
            action_stats[record.action].first++; // total
            if (record.success) action_stats[record.action].second++; // success
        }

        for (const auto& pair : action_stats) {
            float success_rate = (float)pair.second.second / pair.second.first;
            if (success_rate > 0.7f) {
                insights.best_actions.push_back(pair.first + " (" + std::to_string(success_rate) + ")");
            } else if (success_rate < 0.3f) {
                insights.worst_actions.push_back(pair.first + " (" + std::to_string(success_rate) + ")");
            }
        }
    }

    return insights;
}

void LearningSystem::get_status(uint32_t* level, uint32_t* exp, const float** skills_ptr,
                               uint32_t* total, uint32_t* successful, float* success_rate) const {
    *level = learning_level;
    *exp = experience_points;
    *skills_ptr = skills;
    *total = total_actions;
    *successful = successful_actions;
    *success_rate = total_actions > 0 ? (float)successful_actions / total_actions : 0.0f;
}

void LearningSystem::optimize_learning() {
    if (total_actions > 10) {
        float success_rate = (float)successful_actions / total_actions;

        if (success_rate < 0.3f) {
            learning_rate = std::min(0.02f, learning_rate * 1.1f);
            printf("Increased learning rate\n");
        } else if (success_rate > 0.8f) {
            learning_rate = std::max(0.005f, learning_rate * 0.9f);
            printf("Decreased learning rate\n");
        }
    }

    if (success_history.size() > max_history) {
        success_history.erase(success_history.begin(), success_history.end() - max_history);
    }
}

size_t LearningSystem::serialize(uint8_t* buffer, size_t buffer_size) const {
    size_t required_size = sizeof(uint32_t) * 4 + sizeof(float) * 8 +
                          success_history.size() * (sizeof(uint32_t) * 2 + 1 + 20);

    if (buffer_size < required_size || !buffer) {
        return required_size;
    }

    uint8_t* ptr = buffer;

    memcpy(ptr, &experience_points, sizeof(uint32_t));
    ptr += sizeof(uint32_t);
    memcpy(ptr, &learning_level, sizeof(uint32_t));
    ptr += sizeof(uint32_t);
    memcpy(ptr, &total_actions, sizeof(uint32_t));
    ptr += sizeof(uint32_t);
    memcpy(ptr, &successful_actions, sizeof(uint32_t));
    ptr += sizeof(uint32_t);

    memcpy(ptr, skills, sizeof(float) * 8);
    ptr += sizeof(float) * 8;

    uint32_t history_size = success_history.size();
    memcpy(ptr, &history_size, sizeof(uint32_t));
    ptr += sizeof(uint32_t);

    for (const auto& record : success_history) {
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
        memcpy(ptr, &record.experience_gain, sizeof(float));
        ptr += sizeof(float);
    }

    return ptr - buffer;
}

bool LearningSystem::deserialize(const uint8_t* buffer, size_t buffer_size) {
    if (buffer_size < sizeof(uint32_t) * 5 + sizeof(float) * 8 || !buffer) {
        return false;
    }

    const uint8_t* ptr = buffer;

    memcpy(&experience_points, ptr, sizeof(uint32_t));
    ptr += sizeof(uint32_t);
    memcpy(&learning_level, ptr, sizeof(uint32_t));
    ptr += sizeof(uint32_t);
    memcpy(&total_actions, ptr, sizeof(uint32_t));
    ptr += sizeof(uint32_t);
    memcpy(&successful_actions, ptr, sizeof(uint32_t));
    ptr += sizeof(uint32_t);

    memcpy(skills, ptr, sizeof(float) * 8);
    ptr += sizeof(float) * 8;

    uint32_t history_size;
    memcpy(&history_size, ptr, sizeof(uint32_t));
    ptr += sizeof(uint32_t);

    success_history.clear();
    success_history.reserve(std::min((uint32_t)max_history, history_size));

    for (uint32_t i = 0; i < history_size && i < max_history; ++i) {
        if (ptr - buffer + sizeof(uint32_t) + 2 + sizeof(float) > buffer_size) break;

        SuccessRecord record;
        memcpy(&record.timestamp, ptr, sizeof(uint32_t));
        ptr += sizeof(uint32_t);

        uint8_t success;
        memcpy(&success, ptr, sizeof(uint8_t));
        ptr += sizeof(uint8_t);
        record.success = success != 0;

        uint8_t action_len;
        memcpy(&action_len, ptr, sizeof(uint8_t));
        ptr += sizeof(uint8_t);

        if (ptr - buffer + action_len + sizeof(float) > buffer_size) break;

        char action_buf[20] = {0};
        memcpy(action_buf, ptr, action_len);
        ptr += action_len;
        record.action = action_buf;

        memcpy(&record.experience_gain, ptr, sizeof(float));
        ptr += sizeof(float);

        success_history.push_back(record);
    }

    return true;
}

bool LearningSystem::analyze_success(const char* action, const float* pre_states, const float* post_states) const {
    // Анализируем, улучшилось ли состояние после действия
    if (!pre_states || !post_states) return false;

    // Определяем ожидаемый эффект действия
    if (strcmp(action, "feed") == 0) {
        // Ожидаем улучшение голодного состояния
        return post_states[0] < pre_states[0] - 0.05f; // hunger уменьшился
    } else if (strcmp(action, "heal") == 0) {
        // Ожидаем улучшение здоровья
        return post_states[1] > pre_states[1] + 0.05f; // health увеличился
    } else if (strcmp(action, "rest") == 0) {
        // Ожидаем восстановление энергии
        return post_states[2] > pre_states[2] + 0.05f; // energy увеличился
    } else if (strcmp(action, "play") == 0) {
        // Ожидаем улучшение счастья
        return post_states[3] > pre_states[3] + 0.05f; // happiness увеличился
    } else if (strcmp(action, "socialize") == 0) {
        // Ожидаем улучшение социального состояния
        return post_states[4] > pre_states[4] + 0.05f; // social увеличился
    } else if (strcmp(action, "explore") == 0) {
        // Ожидаем улучшение любопытства
        return post_states[5] > pre_states[5] + 0.05f; // curiosity увеличился
    } else {
        // Для других действий анализируем общий баланс положительных изменений
        int positive_changes = 0;
        int negative_changes = 0;

        for (int i = 0; i < 6; i++) {
            float change = post_states[i] - pre_states[i];
            if (change > 0.05f) positive_changes++;
            else if (change < -0.05f) negative_changes++;
        }

        return positive_changes >= negative_changes;
    }
}