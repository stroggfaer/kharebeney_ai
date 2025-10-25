#ifndef KHAREBNEY_AGENT_H
#define KHAREBNEY_AGENT_H

#include "internal_state.h"
#include "balance_manager.h"
#include "emotion_system.h"
#include "learning_system.h"
#include "simple_systems.h"
#include "embedding.h"  // Добавляем систему эмбеддингов
#include "../config.h"

// Перечисление для типов поведенческих состояний
typedef enum {
    EMOTION_STATE = 0,    // Отвечает за эмоции и влияет на анимацию
    BEHAVIOR_STATE,       // Отвечает за поведение и влияет на принятие решений
    APPEARANCE_STATE,     // Отвечает за визуальное отображение и анимацию
    MEMORY_STATE,         // Отвечает за приоритеты памяти
    LEARNING_STATE,       // Отвечает за обучение и адаптацию
    SOCIAL_STATE,         // Отвечает за социальные взаимодействия
    ENERGY_STATE          // Отвечает за уровень активности
} BehaviorStateType;

class KharebeneyAgent {
private:
    InternalState internal_state;
    BalanceManager balance_manager;
    KnowledgeBase knowledge_base;
    DecisionEngine decision_engine;
    EmotionSystem emotion_system;
    LearningSystem learning_system;
    LifecycleSystem lifecycle_system;
    MemorySystem memory_system;
    PromptSystem prompt_system;

    uint32_t step_count;
    const char* current_action;
    uint32_t last_update_time;
    uint32_t save_interval;

    static const char* TAG;

    const char* decide_enhanced();
    void act_enhanced(const char* action);
    float calculate_memory_importance(const char* action, const InternalStates* pre_state,
                                    const InternalStates* post_state) const;
    void apply_behavior_state(BehaviorStateType type, const char* state, float intensity = 1.0f, uint32_t duration = 0);

public:
    KharebeneyAgent();
    ~KharebeneyAgent() = default;

    const char* update();
    void set_prompt(const char* prompt_type, const char* content, bool temporary = false, uint32_t duration = 3600);
    void set_behavior_state(BehaviorStateType type, const char* state, float intensity = 1.0f, uint32_t duration = 0);
    void trigger_emotion(const char* emotion, float intensity);
    const char* get_emotional_description() const;
    std::vector<std::string> get_learning_insights() const;
    const LifecycleInfo get_lifecycle_info() const;
    std::vector<MemoryRecord> search_memories(const char* query) const;
    std::vector<MemoryRecord> search_memories_by_embedding(const Embedding& query_embedding, float threshold = 0.7f) const;
    void get_memory_summary(uint32_t* total, uint32_t* retrievals, uint32_t* consolidations) const;
    void optimize_behavior();

    // Сохранение/загрузка
    bool save_state(const char* filename = nullptr);
    bool load_state(const char* filename = nullptr);
    bool reset_knowledge_base(); // Сброс базы знаний

    // Геттеры для отладки
    uint32_t get_step_count() const { return step_count; }
    const char* get_current_action() const { return current_action; }
    const InternalStates* get_internal_states() const { return internal_state.get_states(); }
    const Emotion* get_emotional_state() const { return emotion_system.get_current_emotion(); }
    const LearningSystem& get_learning_system() const { return learning_system; }
};

// Объявление глобальной переменной агента
extern KharebeneyAgent* agent;

#endif // KHAREBNEY_AGENT_H