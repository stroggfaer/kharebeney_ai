#ifndef LEARNING_SYSTEM_H
#define LEARNING_SYSTEM_H

#include <stdint.h>
#include <vector>
#include <string>
#include <unordered_map>
#include "../config.h"

typedef struct {
    std::string action;
    bool success;
    uint32_t timestamp;
    float experience_gain;
} SuccessRecord;

typedef struct {
    std::vector<std::string> best_actions;
    std::vector<std::string> worst_actions;
    std::vector<std::string> learning_trends;
} LearningInsights;

class LearningSystem {
private:
    uint32_t experience_points;
    uint32_t learning_level;
    float skills[8]; // problem_solving, social_interaction, emotional_intelligence, creativity, adaptability, memory, attention, patience
    uint32_t total_actions;
    uint32_t successful_actions;
    std::vector<SuccessRecord> success_history;
    uint32_t max_history;
    float learning_rate;
    float skill_decay_rate;
    uint32_t last_update;

    static const char* TAG;

    float calculate_experience_gain(const char* action, bool success, const float* context) const;
    void update_skills(const char* action, bool success);
    void check_level_up();
    bool analyze_success(const char* action, const float* pre_states, const float* post_states) const;

public:
    LearningSystem();
    ~LearningSystem() = default;

    void update();
    void learn_from_experience(const char* action, bool success, const float* pre_states = nullptr, const float* post_states = nullptr);
    void adjust_learning_rate(float adjustment);
    std::vector<std::string> get_recommendations() const;
    LearningInsights get_insights() const;
    void get_status(uint32_t* level, uint32_t* exp, const float** skills_ptr,
                   uint32_t* total, uint32_t* successful, float* success_rate) const;
    void optimize_learning();

    // Сериализация
    size_t serialize(uint8_t* buffer, size_t buffer_size) const;
    bool deserialize(const uint8_t* buffer, size_t buffer_size);
};

#endif // LEARNING_SYSTEM_H