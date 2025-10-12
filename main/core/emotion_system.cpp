#include "emotion_system.h"
#include <esp_timer.h>
#include <algorithm>
#include <cstring>

const char* EmotionSystem::TAG = "EmotionSystem";

EmotionSystem::EmotionSystem() :
    last_update_time(esp_timer_get_time() / 1000),
    decay_rate(EMOTION_DECAY_RATE),
    max_active_emotions(MAX_ACTIVE_EMOTIONS)
{
    // Инициализация эмоций
    emotions = {
        {"joy", 0.5f, 1.2f},
        {"apathy", 0.0f, 0.8f},
        {"discontent", 0.0f, 0.9f},
        {"capricious", 0.0f, 1.1f},
        {"anger", 0.0f, 1.3f},
        {"satisfaction", 0.0f, 1.1f},
        {"curiosity", 0.3f, 1.4f},
        {"fear", 0.0f, 0.7f},
        {"excitement", 0.0f, 1.3f},
        {"calm", 0.2f, 1.0f},
        {"loneliness", 0.0f, 0.9f},
        {"love", 0.1f, 1.2f}
    };

    printf("EmotionSystem initialized\n");
}

void EmotionSystem::update() {
    uint32_t current_time = esp_timer_get_time() / 1000;
    uint32_t time_delta = current_time - last_update_time;

    if (time_delta > 0) {
        // Затухание эмоций
        for (auto& emotion : emotions) {
            if (emotion.intensity > 0) {
                emotion.intensity = std::max(0.0f, emotion.intensity - decay_rate * time_delta / 1000.0f);
            }
        }

        normalize_emotions();
        last_update_time = current_time;
    }
}

void EmotionSystem::trigger_emotion(const char* emotion_name, float intensity) {
    intensity = std::max(0.0f, std::min(1.0f, intensity));

    for (auto& emotion : emotions) {
        if (emotion.name == emotion_name) {
            emotion.intensity = intensity;
            printf("Emotion triggered: %s (%.2f)\n", emotion_name, intensity);
            break;
        }
    }

    normalize_emotions();
}

const Emotion* EmotionSystem::get_current_emotion() const {
    if (emotions.empty()) return nullptr;

    // Находим эмоцию с максимальной интенсивностью
    auto max_emotion = std::max_element(emotions.begin(), emotions.end(),
        [](const Emotion& a, const Emotion& b) {
            return a.intensity < b.intensity;
        });

    return &(*max_emotion);
}

const char* EmotionSystem::get_emotional_description() const {
    const Emotion* current = get_current_emotion();
    if (!current) return "Neutral state";

    static char buffer[128];
    const char* descriptions[] = {
        "Joyful and happy",
        "Apathetic and indifferent",
        "Discontent and upset",
        "Capricious and unpredictable",
        "Angry and irritated",
        "Satisfied and content",
        "Curious and interested",
        "Fearful and anxious",
        "Excited and energetic",
        "Calm and relaxed",
        "Lonely and sad",
        "Loving and tender"
    };

    const char* emotion_names[] = {
        "joy", "apathy", "discontent", "capricious", "anger",
        "satisfaction", "curiosity", "fear", "excitement", "calm",
        "loneliness", "love"
    };

    for (size_t i = 0; i < sizeof(emotion_names)/sizeof(emotion_names[0]); ++i) {
        if (current->name == emotion_names[i]) {
            snprintf(buffer, sizeof(buffer), "%s (intensity: %.2f)",
                    descriptions[i], current->intensity);
            return buffer;
        }
    }

    return "Unknown emotional state";
}

float EmotionSystem::get_emotional_weight(const char* emotion_name) const {
    for (const auto& emotion : emotions) {
        if (emotion.name == emotion_name) {
            return emotion.weight;
        }
    }
    return 1.0f;
}

void EmotionSystem::normalize_emotions() {
    // Ограничиваем количество активных эмоций
    std::vector<std::pair<std::string, float>> active_emotions;

    for (const auto& emotion : emotions) {
        if (emotion.intensity > 0.1f) {
            active_emotions.emplace_back(emotion.name, emotion.intensity);
        }
    }

    // Сортируем по интенсивности
    std::sort(active_emotions.begin(), active_emotions.end(),
        [](const auto& a, const auto& b) { return a.second > b.second; });

    // Оставляем только самые сильные эмоции
    if (active_emotions.size() > max_active_emotions) {
        for (size_t i = max_active_emotions; i < active_emotions.size(); ++i) {
            for (auto& emotion : emotions) {
                if (emotion.name == active_emotions[i].first) {
                    emotion.intensity = 0.0f;
                    break;
                }
            }
        }
    }

    // Нормализуем интенсивность
    float max_intensity = 0.0f;
    for (const auto& emotion : emotions) {
        max_intensity = std::max(max_intensity, emotion.intensity);
    }

    if (max_intensity > 1.0f) {
        for (auto& emotion : emotions) {
            emotion.intensity /= max_intensity;
        }
    }
}

size_t EmotionSystem::serialize(uint8_t* buffer, size_t buffer_size) const {
    size_t required_size = emotions.size() * (20 + sizeof(float) * 2); // name + intensity + weight

    if (buffer_size < required_size || !buffer) {
        return required_size;
    }

    uint8_t* ptr = buffer;

    for (const auto& emotion : emotions) {
        uint8_t name_len = std::min((size_t)19, emotion.name.length());
        memcpy(ptr, &name_len, sizeof(uint8_t));
        ptr += sizeof(uint8_t);

        memcpy(ptr, emotion.name.c_str(), name_len);
        ptr += name_len;

        memcpy(ptr, &emotion.intensity, sizeof(float));
        ptr += sizeof(float);

        memcpy(ptr, &emotion.weight, sizeof(float));
        ptr += sizeof(float);
    }

    return ptr - buffer;
}

bool EmotionSystem::deserialize(const uint8_t* buffer, size_t buffer_size) {
    if (buffer_size < emotions.size() * (1 + sizeof(float) * 2) || !buffer) {
        return false;
    }

    const uint8_t* ptr = buffer;

    for (auto& emotion : emotions) {
        uint8_t name_len;
        memcpy(&name_len, ptr, sizeof(uint8_t));
        ptr += sizeof(uint8_t);

        if (ptr - buffer + name_len + sizeof(float) * 2 > buffer_size) {
            return false;
        }

        char name_buf[20] = {0};
        memcpy(name_buf, ptr, name_len);
        ptr += name_len;
        emotion.name = name_buf;

        memcpy(&emotion.intensity, ptr, sizeof(float));
        ptr += sizeof(float);

        memcpy(&emotion.weight, ptr, sizeof(float));
        ptr += sizeof(float);
    }

    return true;
}