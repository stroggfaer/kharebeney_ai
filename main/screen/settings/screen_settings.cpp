#include "screen_settings.h"
#include "../core/kharebeney_agent.h"
#include "../fonts/icons.h"
#include "ssd1306.h"
#include "../display.h"
#include "../lib/utlis.h"
#include "esp_timer.h"  // Для измерения времени
#include <cstdio>       // Для snprintf
#include <vector>       // Для работы с векторами
#include <string>       // Для работы со строками
#include <inttypes.h>   // Для PRI-макросов

// Объявляем extern переменную агента (предполагаем, что она определена в другом месте)

void draw_screen_settings(SSD1306_t* oled) {
    clear_screen_center(oled);

    // Выполняем расширенное тестирование агента
    if (agent) {
        printf("=== Тестирование агента Kharebeney ===\n");

        // Вызываем обновление агента для генерации действий и воспоминаний
        const char* action = agent->update();
        printf("Выполнено обновление агента, текущее действие: %s\n", action ? action : "нет");

        // Тестирование внутренних состояний
        const InternalStates* states = agent->get_internal_states();
        if (states) {
            printf("Текущие внутренние состояния:\n");
            printf("  Голод: %.3f\n", states->hunger);
            printf("  Счастье: %.3f\n", states->happiness);
            printf("  Здоровье: %.3f\n", states->health);
            printf(" Энергия: %.3f\n", states->energy);
            printf("  Социальность: %.3f\n", states->social);
            printf(" Любопытство: %.3f\n", states->curiosity);
            printf("  Уровень: %" PRIu32 "\n", states->level);
            printf("  Возраст: %" PRIu32 " сек\n", states->age);
            printf("  Время: %" PRIu32 " сек\n", states->time);
            
            // Создаем эмбеддинг из текущих состояний
            Embedding embedding = EmbeddingConverter::states_to_embedding((const float*)states);
            embedding.normalize(); // Нормализуем для лучшего сравнения
            
            // Создаем тестовый эмбеддинг для сравнения
            Embedding test_embedding;
            for (int i = 0; i < EMBEDDING_DIM; i++) {
                test_embedding.data[i] = 0.5f; // Просто тестовые значения
            }
            test_embedding.normalize();
            
            // Измеряем время для вычисления косинусного сходства
            int64_t start_time = esp_timer_get_time();
            float similarity = embedding.cosine_similarity(test_embedding);
            int64_t end_time = esp_timer_get_time();
            int64_t duration = end_time - start_time; // в микросекундах
            
            // Выводим информацию об эмбеддингах в консоль
            printf("=== Тестирование эмбеддингов ===\n");
            printf("Время вычисления косинусного сходства: %lld мкс\n", duration);
            printf("Сходство с тестовым вектором: %.4f\n", similarity);
            printf("Эмбеддинг: [");
            for (int i = 0; i < EMBEDDING_DIM; i++) {
                printf("%.3f", embedding.data[i]);
                if (i < EMBEDDING_DIM - 1) printf(", ");
            }
            printf("]\n");
            printf("Тестовый эмбеддинг: [");
            for (int i = 0; i < EMBEDDING_DIM; i++) {
                printf("%.3f", test_embedding.data[i]);
                if (i < EMBEDDING_DIM - 1) printf(", ");
            }
            printf("]\n");
            
            // Тестирование семантического поиска векторной базе
            std::vector<MemoryRecord> similar_memories = agent->search_memories_by_embedding(embedding, 0.5f);
            printf("Найдено %zu воспоминаний по эмбеддингу\n", similar_memories.size());
            for (size_t i = 0; i < similar_memories.size() && i < 3; i++) {
                printf("  Воспоминание %zu: %s, важность: %.3f, эмоция: %s\n",
                       i+1, similar_memories[i].action.c_str(), similar_memories[i].importance, similar_memories[i].emotion.c_str());
            }
            
            // Отображаем информацию на экране
            char buffer[32];
            snprintf(buffer, sizeof(buffer), "CosSim: %.3f", similarity);
            ssd1306_display_text(oled, 2, buffer, strlen(buffer), false);
            snprintf(buffer, sizeof(buffer), "Time: %lld us", duration);
            ssd1306_display_text(oled, 3, buffer, strlen(buffer), false);
            snprintf(buffer, sizeof(buffer), "Mem: %zu", similar_memories.size());
            ssd1306_display_text(oled, 4, buffer, strlen(buffer), false);
        } else {
            ssd1306_display_text(oled, 3, "No states", 9, false);
            printf("Ошибка: не удалось получить внутренние состояния агента\n");
        }
        
        // Тестирование эмоциональной системы
        const Emotion* emotion = agent->get_emotional_state();
        if (emotion) {
            printf("=== Тестирование эмоциональной системы ===\n");
            printf("Текущая эмоция: %s, интенсивность: %.3f\n", emotion->name.c_str(), emotion->intensity);
            printf("Описание эмоционального состояния: %s\n", agent->get_emotional_description());
        } else {
            printf("Не удалось получить эмоциональное состояние агента\n");
        }
        
        // Тестирование системы обучения
        std::vector<std::string> learning_insights = agent->get_learning_insights();
        printf("=== Тестирование системы обучения ===\n");
        printf("Количество рекомендаций обучения: %zu\n", learning_insights.size());
        for (size_t i = 0; i < learning_insights.size(); i++) {
            printf("  Рекомендация %zu: %s\n", i+1, learning_insights[i].c_str());
        }
        
        // Тестирование принятия решений
        printf("=== Тестирование логики принятия решений ===\n");
        printf("Текущее действие: %s\n", agent->get_current_action() ? agent->get_current_action() : "нет");
        printf("Количество шагов: %" PRIu32 "\n", agent->get_step_count());
        
        // Тестирование системы памяти
        printf("=== Тестирование системы памяти ===\n");
        uint32_t total_memories, retrievals, consolidations;
        agent->get_memory_summary(&total_memories, &retrievals, &consolidations);
        printf("Всего воспоминаний: %" PRIu32 "\n", total_memories);
        printf("Количество извлечений: %" PRIu32 "\n", retrievals);
        printf("Количество консолидаций: %" PRIu32 "\n", consolidations);
        
        // Поиск воспоминаний по ключевому слову
        std::vector<MemoryRecord> keyword_memories = agent->search_memories("feed");
        printf("Найдено %zu воспоминаний по ключевому слову 'feed'\n", keyword_memories.size());
        
        // Тестирование промтов и поведенческих состояний
        printf("=== Тестирование промтов и поведенческих состояний ===\n");
        printf("Применение различных поведенческих состояний для тестирования агента:\n");
        
        // Сохраняем текущие промты для последующего восстановления
        // Тест 1: Эмоциональное состояние
        agent->set_behavior_state(EMOTION_STATE, "joy", 0.8f);
        printf("Тест 1 - Эмоциональное состояние: 'joy' (интенсивность 0.8)\n");
        action = agent->update();
        const InternalStates* test_states = agent->get_internal_states();
        const Emotion* test_emotion = agent->get_emotional_state();
        printf(" Действие: %s, Голод: %.3f, Счастье: %.3f, Социальность: %.3f, Эмоция: %s, Интенсивность: %.3f\n",
               action ? action : "нет", test_states->hunger, test_states->happiness, test_states->social,
               test_emotion ? test_emotion->name.c_str() : "нет", test_emotion ? test_emotion->intensity : 0.0f);
                 
        // Тест 2: Поведенческое состояние
        agent->set_behavior_state(BEHAVIOR_STATE, "curious", 0.7f);
        printf("Тест 2 - Поведенческое состояние: 'curious' (интенсивность 0.7)\n");
        action = agent->update();
        test_states = agent->get_internal_states();
        test_emotion = agent->get_emotional_state();
        printf("  Действие: %s, Голод: %.3f, Счастье: %.3f, Любопытство: %.3f, Эмоция: %s, Интенсивность: %.3f\n",
               action ? action : "нет", test_states->hunger, test_states->happiness, test_states->curiosity,
               test_emotion ? test_emotion->name.c_str() : "нет", test_emotion ? test_emotion->intensity : 0.0f);
                 
        // Тест 3: Социальное состояние
        agent->set_behavior_state(SOCIAL_STATE, "friendly", 0.6f);
        printf("Тест 3 - Социальное состояние: 'friendly' (интенсивность 0.6)\n");
        action = agent->update();
        test_states = agent->get_internal_states();
        test_emotion = agent->get_emotional_state();
        printf(" Действие: %s, Голод: %.3f, Счастье: %.3f, Социальность: %.3f, Эмоция: %s, Интенсивность: %.3f\n",
               action ? action : "нет", test_states->hunger, test_states->happiness, test_states->social,
               test_emotion ? test_emotion->name.c_str() : "нет", test_emotion ? test_emotion->intensity : 0.0f);
                 
        // Тест 4: Энергетическое состояние
        agent->set_behavior_state(ENERGY_STATE, "energetic", 0.5f);
        printf("Тест 4 - Энергетическое состояние: 'energetic' (интенсивность 0.5)\n");
        action = agent->update();
        test_states = agent->get_internal_states();
        test_emotion = agent->get_emotional_state();
        printf("  Действие: %s, Энергия: %.3f, Счастье: %.3f, Здоровье: %.3f, Эмоция: %s, Интенсивность: %.3f\n",
               action ? action : "нет", test_states->energy, test_states->happiness, test_states->health,
               test_emotion ? test_emotion->name.c_str() : "нет", test_emotion ? test_emotion->intensity : 0.0f);
                 
        // Тест 5: Обучающееся состояние
        agent->set_behavior_state(LEARNING_STATE, "focused", 0.9f);
        printf("Тест 5 - Обучающееся состояние: 'focused' (интенсивность 0.9)\n");
        action = agent->update();
        test_states = agent->get_internal_states();
        test_emotion = agent->get_emotional_state();
        printf(" Действие: %s, Голод: %.3f, Счастье: %.3f, Любопытство: %.3f, Эмоция: %s, Интенсивность: %.3f\n",
               action ? action : "нет", test_states->hunger, test_states->happiness, test_states->curiosity,
               test_emotion ? test_emotion->name.c_str() : "нет", test_emotion ? test_emotion->intensity : 0.0f);
                 
        // Восстанавливаем стандартные промты
        agent->set_prompt("personality", "You are a smart AI agent for ESP32-S3.");
        agent->set_prompt("behavior", "You are optimized for microcontroller operation.");
                 
        printf("=== Конец тестирования агента ===\n");

        // Добавляем тесты для проверки корректности индексов состояний и успешности действий
        printf("=== Тестирование корректности индексов состояний и успешности действий ===\n");
        
        // Тест 1: Проверка корректности индексов состояний
        // Создаем новое имя переменной, чтобы избежать конфликта
        InternalStates test_states_local = {0.5f, 0.6f, 0.7f, 0.8f, 0.9f, 1.0f, 1, 0, 0};
        float* state_array = (float*)&test_states_local;
        
        printf("Тест индексов состояний:\n");
        printf("  state_array[0] (hunger): %.3f (ожидаем 0.5f)\n", state_array[0]);
        printf("  state_array[1] (happiness): %.3f (ожидаем 0.6f)\n", state_array[1]);
        printf("  state_array[2] (health): %.3f (ожидаем 0.7f)\n", state_array[2]);
        printf("  state_array[3] (energy): %.3f (ожидаем 0.8f)\n", state_array[3]);
        printf("  state_array[4] (social): %.3f (ожидаем 0.9f)\n", state_array[4]);
        printf("  state_array[5] (curiosity): %.3f (ожидаем 1.0f)\n", state_array[5]);
        
        bool indices_correct = 
            (state_array[0] == test_states_local.hunger) &&
            (state_array[1] == test_states_local.happiness) &&
            (state_array[2] == test_states_local.health) &&
            (state_array[3] == test_states_local.energy) &&
            (state_array[4] == test_states_local.social) &&
            (state_array[5] == test_states_local.curiosity);
        
        printf("  Индексы состояний корректны: %s\n", indices_correct ? "ДА" : "НЕТ");
        
        // Тест 2: Проверка успешности действий
        printf("Тест успешности действий:\n");
        LearningSystem test_learning_system;
        
        // Тест для действия feed: уменьшение голода
        float pre_states_feed[] = {0.8f, 0.5f, 0.6f, 0.7f, 0.4f, 0.3f};
        float post_states_feed[] = {0.2f, 0.5f, 0.6f, 0.7f, 0.4f, 0.3f}; // feed успешен
        bool feed_success = test_learning_system.analyze_success("feed", pre_states_feed, post_states_feed);
        printf("  Действие 'feed' успешно (голод уменьшился с 0.8 до 0.2): %s\n", feed_success ? "ДА" : "НЕТ");
        
        // Тест для действия heal: увеличение здоровья
        float pre_states_heal[] = {0.3f, 0.5f, 0.4f, 0.7f, 0.4f, 0.3f};
        float post_states_heal[] = {0.3f, 0.5f, 0.8f, 0.7f, 0.4f, 0.3f}; // heal успешен
        bool heal_success = test_learning_system.analyze_success("heal", pre_states_heal, post_states_heal);
        printf("  Действие 'heal' успешно (здоровье увеличилось с 0.4 до 0.8): %s\n", heal_success ? "ДА" : "НЕТ");
        
        // Тест для действия rest: увеличение энергии
        float pre_states_rest[] = {0.3f, 0.5f, 0.6f, 0.3f, 0.4f, 0.3f};
        float post_states_rest[] = {0.3f, 0.5f, 0.6f, 0.9f, 0.4f, 0.3f}; // rest успешен
        bool rest_success = test_learning_system.analyze_success("rest", pre_states_rest, post_states_rest);
        printf("  Действие 'rest' успешно (энергия увеличилась с 0.3 до 0.9): %s\n", rest_success ? "ДА" : "НЕТ");
        
        // Тест для действия play: увеличение счастья
        float pre_states_play[] = {0.3f, 0.4f, 0.6f, 0.7f, 0.4f, 0.3f};
        float post_states_play[] = {0.3f, 0.9f, 0.6f, 0.7f, 0.4f, 0.3f}; // play успешен
        bool play_success = test_learning_system.analyze_success("play", pre_states_play, post_states_play);
        printf("  Действие 'play' успешно (счастье увеличилось с 0.4 до 0.9): %s\n", play_success ? "ДА" : "НЕТ");
        
        // Тест для действия socialize: увеличение социальности
        float pre_states_social[] = {0.3f, 0.5f, 0.6f, 0.7f, 0.2f, 0.3f};
        float post_states_social[] = {0.3f, 0.5f, 0.6f, 0.7f, 0.8f, 0.3f}; // socialize успешен
        bool social_success = test_learning_system.analyze_success("socialize", pre_states_social, post_states_social);
        printf("  Действие 'socialize' успешно (социальность увеличилась с 0.2 до 0.8): %s\n", social_success ? "ДА" : "НЕТ");
        
        // Тест для действия explore: увеличение любопытства
        float pre_states_explore[] = {0.3f, 0.5f, 0.6f, 0.7f, 0.4f, 0.2f};
        float post_states_explore[] = {0.3f, 0.5f, 0.6f, 0.7f, 0.4f, 0.9f}; // explore успешен
        bool explore_success = test_learning_system.analyze_success("explore", pre_states_explore, post_states_explore);
        printf("  Действие 'explore' успешно (любопытство увеличилось с 0.2 до 0.9): %s\n", explore_success ? "ДА" : "НЕТ");
        
        printf("=== Конец тестирования корректности индексов и успешности действий ===\n");
        
        // Выводим результаты тестов на экран
        char test_result_buffer[32];
        snprintf(test_result_buffer, sizeof(test_result_buffer), "Indices: %s", indices_correct ? "OK" : "FAIL");
        ssd1306_display_text(oled, 6, test_result_buffer, strlen(test_result_buffer), false);
        
        snprintf(test_result_buffer, sizeof(test_result_buffer), "Feed: %s", feed_success ? "OK" : "FAIL");
        ssd1306_display_text(oled, 7, test_result_buffer, strlen(test_result_buffer), false);
        
        snprintf(test_result_buffer, sizeof(test_result_buffer), "Heal: %s", heal_success ? "OK" : "FAIL");
        ssd1306_display_text(oled, 8, test_result_buffer, strlen(test_result_buffer), false);
        
        snprintf(test_result_buffer, sizeof(test_result_buffer), "Rest: %s", rest_success ? "OK" : "FAIL");
        ssd1306_display_text(oled, 9, test_result_buffer, strlen(test_result_buffer), false);
    } else {
        ssd1306_display_text(oled, 3, "No agent", 8, false);
        printf("Ошибка: агент не инициализирован\n");
    }
    
    ssd1306_show_buffer(oled);
}

void handle_settings_interaction(int x, int y) {
    // Пока ничего не делаем
}