#include "simple_systems.h"
#include <esp_timer.h>
#include <algorithm>
#include <cstring>

// KnowledgeBase
KnowledgeBase::KnowledgeBase() : max_knowledge(MAX_KNOWLEDGE) {}

void KnowledgeBase::record_action(const char* action, const float* pre_state, const float* post_state) {
    if (knowledge.size() >= max_knowledge) {
        // Удаляем старую запись
        knowledge.erase(knowledge.begin());
    }

    KnowledgeRecord record;
    record.action = action;
    record.timestamp = esp_timer_get_time() / 1000;

    // Копируем состояния (предполагаем 6 состояний: hunger, happiness, health, energy, social, curiosity)
    const char* state_names[] = {"hunger", "happiness", "health", "energy", "social", "curiosity"};
    for (int i = 0; i < 6; ++i) {
        record.pre_state[state_names[i]] = pre_state[i];
        record.post_state[state_names[i]] = post_state[i];
    }

    knowledge.push_back(record);
}

// DecisionEngine
DecisionEngine::DecisionEngine() : learning_rate(0.1f), discount_factor(0.9f), exploration_rate(0.3f) {
    action_weights[0] = 0.8f; // feed
    action_weights[1] = 0.7f; // play
    action_weights[2] = 0.9f; // heal
    action_weights[3] = 0.6f; // rest
    action_weights[4] = 0.5f; // socialize
    action_weights[5] = 0.4f; // explore
    action_weights[6] = 0.3f; // wait

    // Инициализация Q-таблицы
    for (int i = 0; i < 7; i++) {
        for (int j = 0; j < 6; j++) {
            q_table[i][j] = 0.5f; // Начальное значение
        }
    }
}

void DecisionEngine::update_q_table(int action_idx, float reward, const float* prev_states, const float* curr_states) {
    if (action_idx < 0 || action_idx >= 7) return;

    // Обновление Q-значения по формуле Q-learning
    float max_next_q = 0.0f;
    for (int a = 0; a < 7; a++) {
        float avg_state_value = 0.0f;
        for (int s = 0; s < 6; s++) {
            avg_state_value += q_table[a][s];
        }
        avg_state_value /= 6.0f;
        if (avg_state_value > max_next_q) {
            max_next_q = avg_state_value;
        }
    }

    // Формула обновления Q-значения
    float td_target = reward + discount_factor * max_next_q;
    float current_q = 0.0f;
    for (int s = 0; s < 6; s++) {
        current_q += q_table[action_idx][s];
    }
    current_q /= 6.0f;
    float td_error = td_target - current_q;

    // Обновляем Q-значения для всех состояний
    for (int s = 0; s < 6; s++) {
        q_table[action_idx][s] += learning_rate * td_error;
        // Ограничиваем значения в диапазоне [0, 1]
        if (q_table[action_idx][s] < 0.0f) q_table[action_idx][s] = 0.0f;
        if (q_table[action_idx][s] > 1.0f) q_table[action_idx][s] = 1.0f;
    }

    // Обновляем веса действий на основе Q-таблицы
    float avg_q_value = 0.0f;
    for (int s = 0; s < 6; s++) {
        avg_q_value += q_table[action_idx][s];
    }
    avg_q_value /= 6.0f;
    action_weights[action_idx] = avg_q_value;
}

int DecisionEngine::select_action(const float* states, bool* is_exploration) const {
    // Определяем индекс действия на основе эмоционального состояния и текущих состояний
    float rand_val = (float)rand() / RAND_MAX;
    if (is_exploration) *is_exploration = (rand_val < exploration_rate);

    if (rand_val < exploration_rate) {
        // Случайное действие (исследование)
        return rand() % 7;
    } else {
        // Наилучшее действие на основе Q-таблицы
        int best_action = 0;
        float best_value = 0.0f;

        for (int a = 0; a < 7; a++) {
            float action_value = 0.0f;
            for (int s = 0; s < 6; s++) {
                action_value += q_table[a][s] * states[s];
            }

            if (action_value > best_value) {
                best_value = action_value;
                best_action = a;
            }
        }

        return best_action;
    }
}

const char* DecisionEngine::decide(const float* states, const float* sensor_data,
                                  const float* weights, const char* emotional_state) {
    // Используем Q-learning для выбора действия
    bool is_exploration;
    int action_idx = select_action(states, &is_exploration);

    // Возвращаем строку действия по индексу
    const char* actions[] = {"feed", "play", "heal", "rest", "socialize", "explore", "wait"};
    return actions[action_idx];
}

// LifecycleSystem
LifecycleSystem::LifecycleSystem() : age(0), last_update(esp_timer_get_time() / 1000) {
    stage = "newborn";
}

void LifecycleSystem::update() {
    uint32_t current_time = esp_timer_get_time() / 1000;
    uint32_t time_delta = current_time - last_update;
    age += time_delta;

    // Простая логика стадий
    if (age < 100) stage = "newborn";
    else if (age < 300) stage = "child";
    else if (age < 600) stage = "teenager";
    else if (age < 1000) stage = "adult";
    else stage = "elder";

    last_update = current_time;
}

const LifecycleInfo LifecycleSystem::get_current_stage_info() const {
    LifecycleInfo info;
    info.stage_name = stage;
    info.age = age;
    info.biological_age = age / 10.0f;
    info.maturity_level = std::min(1.0f, age / 1000.0f);
    return info;
}

// MemorySystem
MemorySystem::MemorySystem() :
    max_memories(MAX_MEMORIES),
    retrievals(0),
    consolidations(0) {}

void MemorySystem::update() {
    // Удаляем старые воспоминания
    if (memories.size() > max_memories) {
        memories.erase(memories.begin(), memories.end() - max_memories);
    }
}

void MemorySystem::store_memory(const char* action, const float* context, float importance,
                               const char* emotion, const LifecycleInfo* lifecycle_stage) {
    if (memories.size() >= max_memories) {
        // Удаляем наименее важные
        std::sort(memories.begin(), memories.end(),
            [](const MemoryRecord& a, const MemoryRecord& b) {
                return a.importance < b.importance;
            });
        memories.erase(memories.begin(), memories.end() - max_memories / 2);
    }

    MemoryRecord record;
    record.action = action;
    record.importance = importance;
    record.emotion = emotion;
    record.lifecycle_stage = *lifecycle_stage;
    record.timestamp = esp_timer_get_time() / 1000;

    // Копируем контекст
    const char* context_names[] = {"hunger", "happiness", "health", "energy", "social", "curiosity"};
    for (int i = 0; i < 6; ++i) {
        record.context[context_names[i]] = context[i];
    }

    memories.push_back(record);
}

std::vector<MemoryRecord> MemorySystem::search_memories(const char* query) const {
    MemorySystem* non_const_this = const_cast<MemorySystem*>(this);
    non_const_this->retrievals++;
    std::vector<MemoryRecord> results;

    for (const auto& memory : memories) {
        std::string mem_str = memory.action + " " + memory.emotion;
        if (mem_str.find(query) != std::string::npos) {
            results.push_back(memory);
        }
    }

    return results;
}

void MemorySystem::get_memory_summary(uint32_t* total, uint32_t* ret, uint32_t* cons) const {
    *total = memories.size();
    *ret = retrievals;
    *cons = consolidations;
}

// PromptSystem
PromptSystem::PromptSystem() : max_prompts(MAX_PROMPTS) {}

void PromptSystem::set_prompt(const char* prompt_type, const char* content, bool temporary, uint32_t duration) {
    if (temporary) {
        TemporaryPrompt prompt;
        prompt.content = content;
        prompt.expires = (esp_timer_get_time() / 1000) + duration;
        temporary_prompts[prompt_type] = prompt;
    } else {
        prompts[prompt_type] = content;

        if (prompts.size() > max_prompts) {
            // Удаляем старый промт
            auto it = prompts.begin();
            prompts.erase(it);
        }
    }
}

std::string PromptSystem::generate_combined_prompt(const float* states, const char* emotional_state,
                                                 const std::vector<std::string>* learning_recommendations) {
    std::string result;

    // Базовые промты
    for (const auto& pair : prompts) {
        result += pair.second + " ";
    }

    // Временные промты
    uint32_t current_time = esp_timer_get_time() / 1000;
    std::vector<std::string> expired;

    for (const auto& pair : temporary_prompts) {
        if (pair.second.expires > current_time) {
            result += pair.second.content + " ";
        } else {
            expired.push_back(pair.first);
        }
    }

    // Удаляем истекшие
    for (const auto& key : expired) {
        temporary_prompts.erase(key);
    }

    return result;
}

void PromptSystem::optimize_prompts() {
    // Удаляем истекшие временные промты
    uint32_t current_time = esp_timer_get_time() / 1000;
    std::vector<std::string> expired;

    for (const auto& pair : temporary_prompts) {
        if (pair.second.expires <= current_time) {
            expired.push_back(pair.first);
        }
    }

    for (const auto& key : expired) {
        temporary_prompts.erase(key);
    }
}

size_t PromptSystem::get_total_prompts() const {
    return prompts.size() + temporary_prompts.size();
}