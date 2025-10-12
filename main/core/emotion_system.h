#ifndef EMOTION_SYSTEM_H
#define EMOTION_SYSTEM_H

#include <stdint.h>
#include <string.h>
#include <vector>
#include <string>
#include "../config.h"

// Структура эмоции
typedef struct {
    std::string name;
    float intensity; // 0.0 - 1.0
    float weight;    // вес для принятия решений
} Emotion;

class EmotionSystem {
private:
    std::vector<Emotion> emotions;
    uint32_t last_update_time;
    float decay_rate;
    uint32_t max_active_emotions;

    static const char* TAG;

    void normalize_emotions();

public:
    EmotionSystem();
    ~EmotionSystem() = default;

    void update();
    void trigger_emotion(const char* emotion_name, float intensity);
    const Emotion* get_current_emotion() const;
    const char* get_emotional_description() const;
    float get_emotional_weight(const char* emotion_name) const;

    // Сериализация
    size_t serialize(uint8_t* buffer, size_t buffer_size) const;
    bool deserialize(const uint8_t* buffer, size_t buffer_size);
};

#endif // EMOTION_SYSTEM_H