#include "embedding.h"
#include <algorithm>
#include <random>
#include <chrono>

Embedding::Embedding() {
    zero();
}

Embedding::Embedding(const float* values) {
    for (int i = 0; i < EMBEDDING_DIM; i++) {
        data[i] = values ? values[i] : 0.0f;
    }
}

void Embedding::zero() {
    for (int i = 0; i < EMBEDDING_DIM; i++) {
        data[i] = 0.0f;
    }
}

void Embedding::normalize() {
    float norm = 0.0f;
    for (int i = 0; i < EMBEDDING_DIM; i++) {
        norm += data[i] * data[i];
    }
    norm = std::sqrt(norm);
    if (norm > 0.0f) {
        for (int i = 0; i < EMBEDDING_DIM; i++) {
            data[i] /= norm;
        }
    }
}

float Embedding::cosine_similarity(const Embedding& other) const {
    float dot_product = 0.0f;
    float norm_a = 0.0f;
    float norm_b = 0.0f;
    
    for (int i = 0; i < EMBEDDING_DIM; i++) {
        dot_product += data[i] * other.data[i];
        norm_a += data[i] * data[i];
        norm_b += other.data[i] * other.data[i];
    }
    
    norm_a = std::sqrt(norm_a);
    norm_b = std::sqrt(norm_b);
    
    if (norm_a == 0.0f || norm_b == 0.0f) {
        return 0.0f;
    }
    
    return dot_product / (norm_a * norm_b);
}

float Embedding::euclidean_distance(const Embedding& other) const {
    float sum = 0.0f;
    for (int i = 0; i < EMBEDDING_DIM; i++) {
        float diff = data[i] - other.data[i];
        sum += diff * diff;
    }
    return std::sqrt(sum);
}

Embedding Embedding::operator+(const Embedding& other) const {
    Embedding result;
    for (int i = 0; i < EMBEDDING_DIM; i++) {
        result.data[i] = data[i] + other.data[i];
    }
    return result;
}

Embedding Embedding::operator*(float scalar) const {
    Embedding result;
    for (int i = 0; i < EMBEDDING_DIM; i++) {
        result.data[i] = data[i] * scalar;
    }
    return result;
}

Embedding EmbeddingConverter::states_to_embedding(const float* states) {
    Embedding embedding;
    
    // Базовые состояния
    embedding.data[0] = states[0]; // hunger
    embedding.data[1] = states[1]; // happiness
    embedding.data[2] = states[2]; // health
    embedding.data[3] = states[3]; // energy
    embedding.data[4] = states[4]; // social
    embedding.data[5] = states[5]; // curiosity
    
    // Дополнительные признаки - комбинации базовых состояний
    embedding.data[6] = (states[0] + states[1]) / 2; // hunger + happiness
    embedding.data[7] = (states[2] + states[3]) / 2; // health + energy
    
    return embedding;
}

void EmbeddingConverter::embedding_to_states(const Embedding& embedding, float* states) {
    if (!states) return;
    
    // Восстанавливаем базовые состояния
    states[0] = embedding.data[0]; // hunger
    states[1] = embedding.data[1]; // happiness
    states[2] = embedding.data[2]; // health
    states[3] = embedding.data[3]; // energy
    states[4] = embedding.data[4]; // social
    states[5] = embedding.data[5]; // curiosity
}

Embedding EmbeddingConverter::emotion_to_embedding(const std::string& emotion, float intensity) {
    Embedding embedding;
    
    // Упрощенная схема: кодируем эмоции как индексы в пространстве
    std::hash<std::string> hasher;
    size_t hash = hasher(emotion);
    
    // Используем хэш для генерации вектора
    for (int i = 0; i < EMBEDDING_DIM; i++) {
        embedding.data[i] = (static_cast<float>((hash >> (i * 4)) & 0xF) / 15.0f) * intensity;
    }
    
    return embedding;
}

Embedding EmbeddingConverter::action_to_embedding(const std::string& action) {
    Embedding embedding;
    
    std::hash<std::string> hasher;
    size_t hash = hasher(action);
    
    // Используем хэш для генерации вектора
    for (int i = 0; i < EMBEDDING_DIM; i++) {
        embedding.data[i] = static_cast<float>((hash >> (i * 4)) & 0xF) / 15.0f;
    }
    
    return embedding;
}

Embedding EmbeddingConverter::combined_embedding(const float* states, const std::string& emotion, 
                                               const std::string& action) {
    Embedding base = states ? states_to_embedding(states) : Embedding();
    
    if (!emotion.empty()) {
        Embedding emotion_emb = emotion_to_embedding(emotion, 0.5f);
        base = base + emotion_emb;
    }
    
    if (!action.empty()) {
        Embedding action_emb = action_to_embedding(action);
        base = base + action_emb * 0.3f;
    }
    
    return base;
}