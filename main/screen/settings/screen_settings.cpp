#include "screen_settings.h"
#include "../core/kharebeney_agent.h"
#include "../core/agentTest.h"  // Подключаем файл диагностики
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

        // Запускаем диагностику агента
        AgentTest::runAllTests(agent);
        
        // Также запускаем многоуровневое тестирование
        AgentTest::runMultiLevelTest(agent);

        // Тестирование внутренних состояний
        const InternalStates* states = agent->get_internal_states();
        if (states) {
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
    } else {
        ssd1306_display_text(oled, 3, "No agent", 8, false);
        printf("Ошибка: агент не инициализирован\n");
    }
    
    ssd1306_show_buffer(oled);
}

void handle_settings_interaction(int x, int y) {
    // Пока ничего не делаем
}