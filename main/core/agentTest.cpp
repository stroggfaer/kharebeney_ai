#include "agentTest.h"
#include "kharebeney_agent.h"
#include <esp_timer.h>
#include <cstdio>
#include <vector>
#include <string>

const char* AgentTest::TAG = "AgentTest";

void AgentTest::runAllTests(KharebeneyAgent* agent) {
    if (!agent) {
        printf("[%s] ERROR: Agent is null\n", TAG);
        return;
    }

    printf("\n=== НАЧАЛО ДИАГНОСТИКИ АГЕНТА KHAREBENEY ===\n");
    
    // Тестирование внутренних состояний
    testInternalStates(agent);
    
    // Тестирование эмоциональной системы
    testEmotionSystem(agent);
    
    // Тестирование системы обучения
    testLearningSystem(agent);
    
    // Тестирование системы баланса
    testBalanceManager(agent);
    
    // Тестирование системы памяти
    testMemorySystem(agent);
    
    // Тестирование системы принятия решений
    testDecisionSystem(agent);
    
    // Тестирование системы жизненного цикла
    testLifecycleSystem(agent);
    
    // Тестирование системы эмбеддингов
    testEmbeddingSystem(agent);
    
    // Тестирование системы промтов
    testPromptSystem(agent);
    
    // Тестирование производительности
    testPerformance(agent);
    
    printf("=== КОНЕЦ ДИАГНОСТИКИ АГЕНТА ===\n\n");
}

// Удаляем дублирующийся метод

void AgentTest::testInternalStates(KharebeneyAgent* agent) {
    printf("\n--- ТЕСТИРОВАНИЕ ВНУТРЕННИХ СОСТОЯНИЙ ---\n");
    
    const InternalStates* states = agent->get_internal_states();
    if (states) {
        printf("Текущие внутренние состояния:\n");
        printf("  Голод: %.3f (норма: 0.0-1.0)\n", states->hunger);
        printf("  Счастье: %.3f (норма: 0.0-1.0)\n", states->happiness);
        printf("  Здоровье: %.3f (норма: 0.0-1.0)\n", states->health);
        printf("  Энергия: %.3f (норма: 0.0-1.0)\n", states->energy);
        printf(" Социальность: %.3f (норма: 0.0-1.0)\n", states->social);
        printf("  Любопытство: %.3f (норма: 0.0-1.0)\n", states->curiosity);
        printf("  Уровень: %" PRIu32 "\n", states->level);
        printf("  Возраст: %" PRIu32 " (тики)\n", states->age);
        printf("  Время: %" PRIu32 " (тики)\n", states->time);
        
        // Объяснение различий в значениях
        printf("  Примечание: Значения могут изменяться между вызовами из-за обновления состояний\n");
        
        // Проверка корректности значений
        bool states_valid = true;
        if (states->hunger < 0.0f || states->hunger > 1.0f) states_valid = false;
        if (states->happiness < 0.0f || states->happiness > 1.0f) states_valid = false;
        if (states->health < 0.0f || states->health > 1.0f) states_valid = false;
        if (states->energy < 0.0f || states->energy > 1.0f) states_valid = false;
        if (states->social < 0.0f || states->social > 1.0f) states_valid = false;
        if (states->curiosity < 0.0f || states->curiosity > 1.0f) states_valid = false;
        
        printf("  Состояния корректны: %s\n", states_valid ? "ДА" : "НЕТ");
    } else {
        printf("ОШИБКА: Не удалось получить внутренние состояния\n");
    }
}

void AgentTest::testEmotionSystem(KharebeneyAgent* agent) {
    printf("\n--- ТЕСТИРОВАНИЕ ЭМОЦИОНАЛЬНОЙ СИСТЕМЫ ---\n");
    
    const Emotion* emotion = agent->get_emotional_state();
    if (emotion) {
        printf("Текущая эмоция: %s\n", emotion->name.c_str());
        printf("Интенсивность: %.3f (норма: 0.0-1.0)\n", emotion->intensity);
        printf("Вес эмоции: %.3f\n", emotion->weight);
        printf("Описание эмоционального состояния: %s\n", agent->get_emotional_description());
        
        // Проверка корректности значений
        bool emotion_valid = (emotion->intensity >= 0.0f && emotion->intensity <= 1.0f);
        printf("  Эмоция корректна: %s\n", emotion_valid ? "ДА" : "НЕТ");
    } else {
        printf("ОШИБКА: Не удалось получить эмоциональное состояние\n");
    }
}

void AgentTest::testLearningSystem(KharebeneyAgent* agent) {
    printf("\n--- ТЕСТИРОВАНИЕ СИСТЕМЫ ОБУЧЕНИЯ ---\n");
    
    const LearningSystem& learning_system = agent->get_learning_system();
    uint32_t level, exp, total, successful;
    float success_rate;
    const float* skills;
    
    learning_system.get_status(&level, &exp, &skills, &total, &successful, &success_rate);
    
    printf("Уровень обучения: %" PRIu32 "\n", level);
    printf("Опыт: %" PRIu32 "\n", exp);
    printf("Всего действий: %" PRIu32 "\n", total);
    printf("Успешных действий: %" PRIu32 "\n", successful);
    printf("Процент успеха: %.2f%%\n", success_rate * 100);
    
    // Объяснение низкого процента успеха
    if (success_rate == 0.0f && total > 0) {
        printf(" Примечание: Процент успеха низкий, возможно, из-за строгой оценки успеха действий\n");
    }
    
    printf("Навыки:\n");
    const char* skill_names[] = {"problem_solving", "social_interaction", "emotional_intelligence",
                                "creativity", "adaptability", "memory", "attention", "patience"};
    for (int i = 0; i < 8; i++) {
        printf("  %s: %.3f (норма: 0.0-1.0)\n", skill_names[i], skills[i]);
    }
    
    // Проверка корректности значений
    bool skills_valid = true;
    for (int i = 0; i < 8; i++) {
        if (skills[i] < 0.0f || skills[i] > 1.0f) {
            skills_valid = false;
            break;
        }
    }
    printf(" Навыки корректны: %s\n", skills_valid ? "ДА" : "НЕТ");
    
    // Получение рекомендаций
    std::vector<std::string> recommendations = agent->get_learning_insights();
    printf("Количество рекомендаций: %zu\n", recommendations.size());
    for (size_t i = 0; i < recommendations.size() && i < 5; i++) {
        printf("  Рекомендация %zu: %s\n", i+1, recommendations[i].c_str());
    }
}

void AgentTest::testBalanceManager(KharebeneyAgent* agent) {
    printf("\n--- ТЕСТИРОВАНИЕ СИСТЕМЫ БАЛАНСА ---\n");
    
    // Проверяем публичные методы BalanceManager через агента
    // Временно используем прямой доступ к внутренним компонентам через публичные методы, если они доступны
    printf("Тестирование баланса исследование/эксплуатация:\n");
    printf("  (Для полного теста баланса могут потребоваться дополнительные методы доступа)\n");
}

void AgentTest::testMemorySystem(KharebeneyAgent* agent) {
    printf("\n--- ТЕСТИРОВАНИЕ СИСТЕМЫ ПАМЯТИ ---\n");
    
    uint32_t total_memories, retrievals, consolidations;
    agent->get_memory_summary(&total_memories, &retrievals, &consolidations);
    
    printf("Всего воспоминаний: %" PRIu32 "\n", total_memories);
    printf("Количество извлечений: %" PRIu32 "\n", retrievals);
    printf("Количество консолидаций: %" PRIu32 "\n", consolidations);
    
    // Тестирование поиска по ключевому слову
    std::vector<MemoryRecord> keyword_memories = agent->search_memories("feed");
    printf("Найдено воспоминаний по ключевому слову 'feed': %zu\n", keyword_memories.size());
    
    // Тестирование семантического поиска
    const InternalStates* states = agent->get_internal_states();
    if (states) {
        Embedding embedding = EmbeddingConverter::states_to_embedding((const float*)states);
        std::vector<MemoryRecord> similar_memories = agent->search_memories_by_embedding(embedding, 0.5f);
        printf("Найдено воспоминаний по эмбеддингу: %zu\n", similar_memories.size());
        
        for (size_t i = 0; i < similar_memories.size() && i < 3; i++) {
            printf("  Воспоминание %zu: %s, важность: %.3f, эмоция: %s\n",
                   i+1, similar_memories[i].action.c_str(), similar_memories[i].importance, 
                   similar_memories[i].emotion.c_str());
        }
    }
}

void AgentTest::testDecisionSystem(KharebeneyAgent* agent) {
    printf("\n--- ТЕСТИРОВАНИЕ СИСТЕМЫ ПРИНЯТИЯ РЕШЕНИЙ ---\n");
    
    printf("Текущее действие: %s\n", agent->get_current_action() ? agent->get_current_action() : "нет");
    printf("Количество шагов: %" PRIu32 "\n", agent->get_step_count());
    
    // Получаем текущее действие до обновления, чтобы избежать конфликта
    const char* current_action = agent->get_current_action();
    printf("Текущее действие до обновления: %s\n", current_action ? current_action : "нет");
    
    // Тестирование обновления агента
    const char* action = agent->update();
    printf("Действие после обновления: %s\n", action ? action : "нет");
}

void AgentTest::testLifecycleSystem(KharebeneyAgent* agent) {
    printf("\n--- ТЕСТИРОВАНИЕ СИСТЕМЫ ЖИЗНЕННОГО ЦИКЛА ---\n");
    
    LifecycleInfo lifecycle_info = agent->get_lifecycle_info();
    printf("Текущая стадия: %s\n", lifecycle_info.stage_name.c_str());
    printf("Возраст (в тиках): %" PRIu32 "\n", lifecycle_info.age);
    printf("Биологический возраст: %.2f\n", lifecycle_info.biological_age);
    printf("Уровень зрелости: %.2f (макс. 1.0)\n", lifecycle_info.maturity_level);
    
    // Проверка корректности значений
    bool lifecycle_valid = (lifecycle_info.maturity_level >= 0.0f && lifecycle_info.maturity_level <= 1.0f);
    printf("  Жизненный цикл корректен: %s\n", lifecycle_valid ? "ДА" : "НЕТ");
    printf("  Примечание: Возраст измеряется в тиках внутреннего таймера, не в секундах\n");
}

void AgentTest::testEmbeddingSystem(KharebeneyAgent* agent) {
    printf("\n--- ТЕСТИРОВАНИЕ СИСТЕМЫ ЭМБЕДДИНГОВ ---\n");
    
    const InternalStates* states = agent->get_internal_states();
    if (states) {
        // Создаем эмбеддинг из текущих состояний
        Embedding embedding = EmbeddingConverter::states_to_embedding((const float*)states);
        embedding.normalize();
        
        printf("Эмбеддинг из состояний: [");
        for (int i = 0; i < EMBEDDING_DIM; i++) {
            printf("%.3f", embedding.data[i]);
            if (i < EMBEDDING_DIM - 1) printf(", ");
        }
        printf("]\n");
        
        // Тестирование косинусного сходства
        Embedding test_embedding;
        for (int i = 0; i < EMBEDDING_DIM; i++) {
            test_embedding.data[i] = 0.5f;
        }
        test_embedding.normalize();
        
        int64_t start_time = esp_timer_get_time();
        float similarity = embedding.cosine_similarity(test_embedding);
        int64_t end_time = esp_timer_get_time();
        
        printf("Тестовый эмбеддинг: [");
        for (int i = 0; i < EMBEDDING_DIM; i++) {
            printf("%.3f", test_embedding.data[i]);
            if (i < EMBEDDING_DIM - 1) printf(", ");
        }
        printf("]\n");
        printf("Косинусное сходство: %.4f\n", similarity);
        printf("Время вычисления: %lld мкс\n", end_time - start_time);
    }
}

void AgentTest::testPerformance(KharebeneyAgent* agent) {
    printf("\n--- ТЕСТИРОВАНИЕ ПРОИЗВОДИТЕЛЬНОСТИ ---\n");
    
    // Тестирование производительности обновления агента
    int64_t start_time = esp_timer_get_time();
    for (int i = 0; i < 10; i++) {
        agent->update();
    }
    int64_t end_time = esp_timer_get_time();
    
    printf("Время 10 обновлений агента: %lld мкс\n", end_time - start_time);
    printf("Среднее время обновления: %.2f мкс\n", (double)(end_time - start_time) / 10.0);
    
    // Тестирование производительности вычисления эмбеддингов
    const InternalStates* states = agent->get_internal_states();
    if (states) {
        start_time = esp_timer_get_time();
        for (int i = 0; i < 100; i++) {
            Embedding embedding = EmbeddingConverter::states_to_embedding((const float*)states);
            embedding.normalize();
        }
        end_time = esp_timer_get_time();
        
        printf("Время 10 вычислений эмбеддинга: %lld мкс\n", end_time - start_time);
        printf("Среднее время вычисления эмбеддинга: %.2f мкс\n", (double)(end_time - start_time) / 100.0);
    }
}

void AgentTest::runMultiLevelTest(KharebeneyAgent* agent) {
    printf("\n=== МНОГОУРОВНЕВОЕ ТЕСТИРОВАНИЕ АГЕНТА ===\n");
    
    // Уровень 1: Базовая диагностика
    printf("\n--- УРОВЕНЬ 1: БАЗОВАЯ ДИАГНОСТИКА ---\n");
    testInternalStates(agent);
    testEmotionSystem(agent);
    
    // Уровень 2: Средняя диагностика
    printf("\n--- УРОВЕНЬ 2: СРЕДНЯЯ ДИАГНОСТИКА ---\n");
    testLearningSystem(agent);
    testMemorySystem(agent);
    
    // Уровень 3: Глубокая диагностика
    printf("\n--- УРОВЕНЬ 3: ГЛУБОКАЯ ДИАГНОСТИКА ---\n");
    testAllEmotions(agent);
    testPromptSystem(agent);
    testBalanceManager(agent);
    testDecisionSystem(agent);
    testLifecycleSystem(agent);
    testEmbeddingSystem(agent);
    testPerformance(agent);
    
    printf("\n=== ЗАВЕРШЕНО МНОГОУРОВНЕВОЕ ТЕСТИРОВАНИЕ ===\n");
}

void AgentTest::testAllEmotions(KharebeneyAgent* agent) {
    printf("\n--- ТЕСТИРОВАНИЕ ВСЕХ ЭМОЦИЙ ---\n");
    
    // Массив всех доступных эмоций с их весами из исходного кода
    const char* emotions[] = {
        "joy", "apathy", "discontent", "capricious",
        "anger", "satisfaction", "curiosity", "fear",
        "excitement", "calm", "loneliness", "love"
    };
    
    // Проверяем текущую эмоцию до тестирования
    const Emotion* current_emotion = agent->get_emotional_state();
    if (current_emotion) {
        printf("Текущая эмоция до тестирования: %s (интенсивность: %.3f)\n",
               current_emotion->name.c_str(), current_emotion->intensity);
    }
    
    // Тестируем каждую эмоцию
    for (size_t i = 0; i < sizeof(emotions)/sizeof(emotions[0]); i++) {
        printf("Тестирование эмоции: %s\n", emotions[i]);
        
        // Вызываем эмоцию с разной интенсивностью
        agent->trigger_emotion(emotions[i], 0.5f);
        const Emotion* triggered_emotion = agent->get_emotional_state();
        if (triggered_emotion && triggered_emotion->name == emotions[i]) {
            printf("  Эмоция %s успешно установлена, интенсивность: %.3f\n",
                   triggered_emotion->name.c_str(), triggered_emotion->intensity);
        } else {
            printf("  ОШИБКА: Эмоция %s не установлена корректно\n", emotions[i]);
        }
        
        // Проверяем описание эмоционального состояния
        const char* description = agent->get_emotional_description();
        printf(" Описание: %s\n", description ? description : "нет описания");
    }
    
    // Проверяем, как система эмоций влияет на принятие решений
    printf("\n--- ВЛИЯНИЕ ЭМОЦИЙ НА ПРИНЯТИЕ РЕШЕНИЙ ---\n");
    agent->trigger_emotion("anger", 0.8f);
    const char* action_after_anger = agent->get_current_action();
    printf("Действие после вызова гнева: %s\n", action_after_anger ? action_after_anger : "нет");
    
    agent->trigger_emotion("joy", 0.8f);
    const char* action_after_joy = agent->get_current_action();
    printf("Действие после вызова радости: %s\n", action_after_joy ? action_after_joy : "нет");
    
    // Проверяем затухание эмоций
    printf("\n--- ТЕСТИРОВАНИЕ ЗАТУХАНИЯ ЭМОЦИЙ ---\n");
    agent->trigger_emotion("excitement", 0.9f);
    const Emotion* before_decay = agent->get_emotional_state();
    printf("Эмоция до затухания: %s (интенсивность: %.3f)\n",
           before_decay->name.c_str(), before_decay->intensity);
    
    // Выполняем несколько обновлений, чтобы проверить затухание
    for (int i = 0; i < 5; i++) {
        agent->update();
    }
    
    const Emotion* after_decay = agent->get_emotional_state();
    printf("Эмоция после затухания: %s (интенсивность: %.3f)\n",
           after_decay->name.c_str(), after_decay->intensity);
           
    printf(" Изменение интенсивности: %.3f\n", before_decay->intensity - after_decay->intensity);
}

void AgentTest::testPromptSystem(KharebeneyAgent* agent) {
    printf("\n--- ТЕСТИРОВАНИЕ СИСТЕМЫ ПРОМТОВ ---\n");
    
    // Сохраняем текущее действие до теста
    const char* initial_action = agent->get_current_action();
    printf("Действие до теста промтов: %s\n", initial_action ? initial_action : "нет");
    
    // Сценарий 1: Установка временного промта для социального поведения
    printf("\n--- СЦЕНАРИЙ 1: Повышение социальности ---\n");
    agent->set_prompt("behavior", "Be more social and interact with user", true, 300); // 5 минут
    
    // Выполняем обновление агента, чтобы промт мог повлиять на решение
    const char* action_after_social_prompt = agent->update();
    printf("Действие после установки социального промта: %s\n", action_after_social_prompt ? action_after_social_prompt : "нет");
    
    // Проверяем, изменилось ли текущее действие
    const char* current_action = agent->get_current_action();
    printf("Текущее действие: %s\n", current_action ? current_action : "нет");
    
    // Сценарий 2: Установка промта для исследования
    printf("\n--- СЦЕНАРИЙ 2: Поощрение исследования ---\n");
    agent->set_prompt("behavior", "Be curious and explore environment", true, 300); // 5 минут
    
    const char* action_after_explore_prompt = agent->update();
    printf("Действие после установки промта исследования: %s\n", action_after_explore_prompt ? action_after_explore_prompt : "нет");
    
    // Сравниваем действия до и после промтов
    printf("\n--- АНАЛИЗ ВЛИЯНИЯ ПРОМТОВ ---\n");
    printf("Изначальное действие: %s\n", initial_action ? initial_action : "нет");
    printf("После социального промта: %s\n", action_after_social_prompt ? action_after_social_prompt : "нет");
    printf("После промта исследования: %s\n", action_after_explore_prompt ? action_after_explore_prompt : "нет");
    
    // Проверяем, изменились ли внутренние состояния
    const InternalStates* states = agent->get_internal_states();
    if (states) {
        printf("Текущие состояния после промтов:\n");
        printf("  Голод: %.3f\n", states->hunger);
        printf("  Счастье: %.3f\n", states->happiness);
        printf(" Социальность: %.3f\n", states->social);
    }
    
    // Проверяем текущую эмоцию
    const Emotion* emotion = agent->get_emotional_state();
    if (emotion) {
        printf("Текущая эмоция: %s (интенсивность: %.3f)\n", emotion->name.c_str(), emotion->intensity);
    }
    
    printf("\n--- ЗАВЕРШЕНО ТЕСТИРОВАНИЕ СИСТЕМЫ ПРОМТОВ ---\n");
}