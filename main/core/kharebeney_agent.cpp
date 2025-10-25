#include "kharebeney_agent.h"
#include <esp_timer.h>
#include <cstring>
#include <cmath>

// Глобальная переменная агента
KharebeneyAgent* agent = nullptr;
const char* KharebeneyAgent::TAG = "KharebeneyAgent";

KharebeneyAgent::KharebeneyAgent() :
    step_count(0),
    current_action(nullptr),
    last_update_time(esp_timer_get_time() / 1000),
    save_interval(SAVE_INTERVAL)
{
    // Устанавливаем базовые промты
    set_prompt("personality", "You are a smart AI agent for ESP32-S3.");
    set_prompt("behavior", "You are optimized for microcontroller operation.");

    printf("KharebeneyAgent initialized\n");
}

const char* KharebeneyAgent::update() {
    step_count++;
    uint32_t current_time = esp_timer_get_time() / 1000;

    // Обновляем жизненный цикл
    lifecycle_system.update();

    // Обновляем эмоции
    emotion_system.update();

    // Обновляем память
    memory_system.update();

    // Принимаем решение
    const char* action = decide_enhanced();
    current_action = action;

    // Выполняем действие
    act_enhanced(action);

    // Управление памятью
    if (step_count % 10 == 0) {
        // Принудительная сборка мусора (в ESP32 это не нужно, но для совместимости)
    }

    last_update_time = current_time;

    return action;
}

const char* KharebeneyAgent::decide_enhanced() {
    const InternalStates* states = internal_state.get_states();
    const Emotion* emotional_state = emotion_system.get_current_emotion();
    std::vector<std::string> learning_recommendations = learning_system.get_recommendations();

    // Генерируем комбинированный промт
    std::string combined_prompt = prompt_system.generate_combined_prompt(
        (const float*)states, emotional_state ? emotional_state->name.c_str() : "neutral",
        &learning_recommendations
    );

    // Получаем веса действий
    const float* action_weights = decision_engine.get_action_weights();

    // Принимаем решение
    return decision_engine.decide((const float*)states, nullptr, action_weights,
                                 emotional_state ? emotional_state->name.c_str() : "neutral");
}

void KharebeneyAgent::act_enhanced(const char* action) {
    const InternalStates* pre_state = internal_state.get_states();

    // Создаем эмбеддинг для предыдущего состояния
    Embedding pre_embedding = EmbeddingConverter::states_to_embedding((const float*)pre_state);

    // Выполняем действие
    internal_state.perform_action(action);

    const InternalStates* post_state = internal_state.get_states();

    // Создаем эмбеддинг для последующего состояния
    Embedding post_embedding = EmbeddingConverter::states_to_embedding((const float*)post_state);

    // Рассчитываем важность для памяти
    float importance = calculate_memory_importance(action, pre_state, post_state);

    // Записываем в память
    const LifecycleInfo lifecycle_info = lifecycle_system.get_current_stage_info();
    const Emotion* emotion = emotion_system.get_current_emotion();

    memory_system.store_memory(action, (const float*)pre_state, importance,
                              emotion ? emotion->name.c_str() : "neutral", &lifecycle_info);

    // Создаем комбинированный эмбеддинг для хранения в памяти
    std::string emotion_str = emotion ? emotion->name.c_str() : "neutral";
    Embedding combined_embedding = EmbeddingConverter::combined_embedding((const float*)pre_state, emotion_str, action);

    // Записываем эмбеддинги в систему памяти для семантического поиска
    memory_system.store_embedding(action, pre_embedding, post_embedding, combined_embedding, importance);

    // Записываем в базу знаний
    knowledge_base.record_action(action, (const float*)pre_state, (const float*)post_state);

    // Обучаемся на опыте с анализом изменений состояний
    learning_system.learn_from_experience(action, true, (const float*)pre_state, (const float*)post_state);

    // Обновляем Q-таблицу на основе награды (успеха)
    float reward = importance; // Используем важность как базовую награду
    const char* actions[] = {"feed", "play", "heal", "rest", "socialize", "explore", "wait"};
    int action_idx = -1;
    for (int i = 0; i < 7; i++) {
        if (strcmp(action, actions[i]) == 0) {
            action_idx = i;
            break;
        }
    }
    if (action_idx >= 0) {
        decision_engine.update_q_table(action_idx, reward, (const float*)pre_state, (const float*)post_state);
    }

    // Записываем действие в менеджер баланса
    balance_manager.record_action(action, true);
}

float KharebeneyAgent::calculate_memory_importance(const char* action, const InternalStates* pre_state,
                                                  const InternalStates* post_state) const {
    float importance = 0.5f;

    // Увеличиваем важность для значимых действий
    if (strcmp(action, "play") == 0 || strcmp(action, "learn") == 0 || strcmp(action, "socialize") == 0) {
        importance += 0.2f;
    }

    // Учитываем изменения состояний
    float changes[] = {
        fabsf(post_state->hunger - pre_state->hunger),
        fabsf(post_state->happiness - pre_state->happiness),
        fabsf(post_state->health - pre_state->health),
        fabsf(post_state->energy - pre_state->energy),
        fabsf(post_state->social - pre_state->social),
        fabsf(post_state->curiosity - pre_state->curiosity)
    };

    for (float change : changes) {
        if (change > 0.1f) {
            importance += 0.1f;
        }
    }

    return std::min(importance, 1.0f);
}

void KharebeneyAgent::set_prompt(const char* prompt_type, const char* content, bool temporary, uint32_t duration) {
    prompt_system.set_prompt(prompt_type, content, temporary, duration);
    printf("Prompt set: %s\n", prompt_type);
}

void KharebeneyAgent::trigger_emotion(const char* emotion, float intensity) {
    emotion_system.trigger_emotion(emotion, intensity);
    printf("Emotion triggered: %s\n", emotion);
}

const char* KharebeneyAgent::get_emotional_description() const {
    return emotion_system.get_emotional_description();
}

std::vector<std::string> KharebeneyAgent::get_learning_insights() const {
    LearningInsights insights = learning_system.get_insights();
    std::vector<std::string> result;

    result.insert(result.end(), insights.best_actions.begin(), insights.best_actions.end());
    result.insert(result.end(), insights.worst_actions.begin(), insights.worst_actions.end());

    return result;
}

const LifecycleInfo KharebeneyAgent::get_lifecycle_info() const {
    return lifecycle_system.get_current_stage_info();
}

std::vector<MemoryRecord> KharebeneyAgent::search_memories(const char* query) const {
    return memory_system.search_memories(query);
}

std::vector<MemoryRecord> KharebeneyAgent::search_memories_by_embedding(const Embedding& query_embedding, float threshold) const {
    return memory_system.search_memories_by_embedding(query_embedding, threshold);
}

void KharebeneyAgent::get_memory_summary(uint32_t* total, uint32_t* retrievals, uint32_t* consolidations) const {
    memory_system.get_memory_summary(total, retrievals, consolidations);
}

void KharebeneyAgent::optimize_behavior() {
    learning_system.optimize_learning();
    prompt_system.optimize_prompts();
    balance_manager.update(); // Адаптируем баланс

    printf("Behavior optimized\n");
}

bool KharebeneyAgent::save_state(const char* filename) {
    // Для ESP32 сохраняем в файл или флеш память
    // Упрощенная версия - сохраняем в бинарный файл
    const char* actual_filename = filename ? filename : "/spiffs/agent_state.bin";

    FILE* file = fopen(actual_filename, "wb");
    if (!file) {
        printf("Failed to open file for saving: %s\n", actual_filename);
        return false;
    }

    // Сохраняем каждую систему
    uint8_t buffer[2048];
    size_t total_size = 0;

    // InternalState
    size_t size = internal_state.serialize(buffer + total_size, sizeof(buffer) - total_size);
    if (size == 0) {
        fclose(file);
        return false;
    }
    total_size += size;

    // EmotionSystem
    size = emotion_system.serialize(buffer + total_size, sizeof(buffer) - total_size);
    if (size == 0) {
        fclose(file);
        return false;
    }
    total_size += size;

    // LearningSystem
    size = learning_system.serialize(buffer + total_size, sizeof(buffer) - total_size);
    if (size == 0) {
        fclose(file);
        return false;
    }
    total_size += size;

    // BalanceManager
    size = balance_manager.serialize(buffer + total_size, sizeof(buffer) - total_size);
    if (size == 0) {
        fclose(file);
        return false;
    }
    total_size += size;

    // MemorySystem
    size = memory_system.serialize(buffer + total_size, sizeof(buffer) - total_size);
    if (size == 0) {
        fclose(file);
        return false;
    }
    total_size += size;

    // Записываем метаданные
    uint32_t metadata[2] = {step_count, last_update_time};
    if (fwrite(metadata, sizeof(metadata), 1, file) != 1) goto save_error;
    if (fwrite(&total_size, sizeof(total_size), 1, file) != 1) goto save_error;
    if (fwrite(buffer, 1, total_size, file) != total_size) goto save_error;

    fclose(file);
    printf("State saved to %s\n", actual_filename);
    return true;

save_error:
    fclose(file);
    printf("Failed to save state\n");
    return false;
}

bool KharebeneyAgent::load_state(const char* filename) {
    const char* actual_filename = filename ? filename : "/spiffs/agent_state.bin";

    FILE* file = fopen(actual_filename, "rb");
    if (!file) {
        printf("State file not found: %s\n", actual_filename);
        return false;
    }

    // Читаем метаданные
    uint32_t metadata[2];
    size_t total_size;

    if (fread(metadata, sizeof(metadata), 1, file) != 1) {
        fclose(file);
        return false;
    }
    if (fread(&total_size, sizeof(total_size), 1, file) != 1) {
        fclose(file);
        return false;
    }

    step_count = metadata[0];
    last_update_time = metadata[1];

    // Читаем данные
    uint8_t* buffer = new uint8_t[total_size];
    if (!buffer) {
        fclose(file);
        return false;
    }

    if (fread(buffer, 1, total_size, file) != total_size) {
        delete[] buffer;
        fclose(file);
        return false;
    }

    // Десериализуем системы
    size_t offset = 0;

    size_t size = internal_state.deserialize(buffer + offset, total_size - offset);
    if (size == 0) {
        delete[] buffer;
        fclose(file);
        return false;
    }
    offset += size;

    size = emotion_system.deserialize(buffer + offset, total_size - offset);
    if (size == 0) {
        delete[] buffer;
        fclose(file);
        return false;
    }
    offset += size;

    size = learning_system.deserialize(buffer + offset, total_size - offset);
    if (size == 0) {
        delete[] buffer;
        fclose(file);
        return false;
    }
    offset += size;

    size = balance_manager.deserialize(buffer + offset, total_size - offset);
    if (size == 0) {
        delete[] buffer;
        fclose(file);
        return false;
    }
    offset += size;

    // MemorySystem
    size = memory_system.deserialize(buffer + offset, total_size - offset);
    if (size == 0) {
        delete[] buffer;
        fclose(file);
        return false;
    }
    offset += size;

    delete[] buffer;
    fclose(file);
    printf("State loaded from %s\n", actual_filename);
    return true;
}

bool KharebeneyAgent::reset_knowledge_base() {
    // Сброс всех систем к начальному состоянию
    internal_state = InternalState();
    balance_manager = BalanceManager();
    knowledge_base = KnowledgeBase();
    emotion_system = EmotionSystem();
    learning_system = LearningSystem();
    lifecycle_system = LifecycleSystem();
    memory_system = MemorySystem();
    prompt_system = PromptSystem();

    // Сброс внутренних переменных
    step_count = 0;
    current_action = nullptr;
    last_update_time = esp_timer_get_time() / 1000;

    // Установка начальных промтов
    set_prompt("personality", "You are a smart AI agent for ESP32-S3.");
    set_prompt("behavior", "You are optimized for microcontroller operation.");

    printf("Knowledge base reset complete\n");
    return true;
}