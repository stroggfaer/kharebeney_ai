#ifndef AGENT_TEST_H
#define AGENT_TEST_H

#include <stdint.h>
#include <inttypes.h>

// Предварительное объявление класса агента
class KharebeneyAgent;

class AgentTest {
private:
    static const char* TAG;

    // Приватные методы тестирования отдельных систем
    static void testInternalStates(KharebeneyAgent* agent);
    static void testEmotionSystem(KharebeneyAgent* agent);
    static void testLearningSystem(KharebeneyAgent* agent);
    static void testBalanceManager(KharebeneyAgent* agent);
    static void testMemorySystem(KharebeneyAgent* agent);
    static void testDecisionSystem(KharebeneyAgent* agent);
    static void testLifecycleSystem(KharebeneyAgent* agent);
    static void testEmbeddingSystem(KharebeneyAgent* agent);
    static void testPerformance(KharebeneyAgent* agent);
    static void testAllEmotions(KharebeneyAgent* agent);
    static void testPromptSystem(KharebeneyAgent* agent);

public:
    // Основной метод для запуска всех тестов
    static void runAllTests(KharebeneyAgent* agent);
    
    // Метод для многоуровневого тестирования
    static void runMultiLevelTest(KharebeneyAgent* agent);
};

#endif // AGENT_TEST_H