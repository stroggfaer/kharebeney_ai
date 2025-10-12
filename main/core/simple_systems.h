#ifndef SIMPLE_SYSTEMS_H
#define SIMPLE_SYSTEMS_H

#include <stdint.h>
#include <vector>
#include <string>
#include <unordered_map>
#include "../config.h"

// KnowledgeBase
typedef struct {
    std::string action;
    std::unordered_map<std::string, float> pre_state;
    std::unordered_map<std::string, float> post_state;
    uint32_t timestamp;
} KnowledgeRecord;

class KnowledgeBase {
private:
    std::vector<KnowledgeRecord> knowledge;
    uint32_t max_knowledge;

public:
    KnowledgeBase();
    void record_action(const char* action, const float* pre_state, const float* post_state);
    const std::vector<KnowledgeRecord>& get_knowledge() const { return knowledge; }
};

// DecisionEngine
class DecisionEngine {
private:
    float action_weights[7]; // feed, play, heal, rest, socialize, explore, wait
    float q_table[7][6]; // Q-таблица: 7 действий, 6 состояний
    float learning_rate; // Скорость обучения
    float discount_factor; // Фактор дисконтирования
    float exploration_rate; // Вероятность исследования

public:
    DecisionEngine();
    const float* get_action_weights() const { return action_weights; }
    void update_q_table(int action_idx, float reward, const float* prev_states, const float* curr_states);
    int select_action(const float* states, bool* is_exploration = nullptr) const;
    const char* decide(const float* states, const float* sensor_data, const float* weights, const char* emotional_state);
};

// LifecycleSystem
typedef struct {
    std::string stage_name;
    uint32_t age;
    float biological_age;
    float maturity_level;
} LifecycleInfo;

class LifecycleSystem {
private:
    uint32_t age;
    std::string stage;
    uint32_t last_update;

public:
    LifecycleSystem();
    void update();
    const LifecycleInfo get_current_stage_info() const;
};

// MemorySystem
typedef struct {
    std::string action;
    std::unordered_map<std::string, float> context;
    float importance;
    std::string emotion;
    LifecycleInfo lifecycle_stage;
    uint32_t timestamp;
} MemoryRecord;

class MemorySystem {
private:
    std::vector<MemoryRecord> memories;
    uint32_t max_memories;
    uint32_t retrievals;
    uint32_t consolidations;

public:
    MemorySystem();
    void update();
    void store_memory(const char* action, const float* context, float importance,
                     const char* emotion, const LifecycleInfo* lifecycle_stage);
    std::vector<MemoryRecord> search_memories(const char* query) const;
    void get_memory_summary(uint32_t* total, uint32_t* retrievals, uint32_t* consolidations) const;
};

// PromptSystem
typedef struct {
    std::string content;
    uint32_t expires;
} TemporaryPrompt;

class PromptSystem {
private:
    std::unordered_map<std::string, std::string> prompts;
    std::unordered_map<std::string, TemporaryPrompt> temporary_prompts;
    uint32_t max_prompts;

public:
    PromptSystem();
    void set_prompt(const char* prompt_type, const char* content, bool temporary, uint32_t duration);
    std::string generate_combined_prompt(const float* states, const char* emotional_state,
                                       const std::vector<std::string>* learning_recommendations);
    void optimize_prompts();
    size_t get_total_prompts() const;
};

#endif // SIMPLE_SYSTEMS_H