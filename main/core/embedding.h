#ifndef EMBEDDING_H
#define EMBEDDING_H

#include <vector>
#include <string>
#include <cstring>
#include <cmath>

// Определение размерности эмбеддинга
#define EMBEDDING_DIM 8

struct Embedding {
    float data[EMBEDDING_DIM];
    
    Embedding();
    Embedding(const float* values);
    
    // Инициализация нулями
    void zero();
    
    // Нормализация вектора
    void normalize();
    
    // Косинусное сходство с другим эмбеддингом
    float cosine_similarity(const Embedding& other) const;
    
    // Евклидово расстояние до другого эмбеддинга
    float euclidean_distance(const Embedding& other) const;
    
    // Сложение эмбеддингов
    Embedding operator+(const Embedding& other) const;
    
    // Умножение на скаляр
    Embedding operator*(float scalar) const;
};

// Класс для преобразования внутренних состояний в эмбеддинги
class EmbeddingConverter {
private:
    static const int INPUT_DIM = 6; // hunger, happiness, health, energy, social, curiosity
    
public:
    // Преобразование внутренних состояний в эмбеддинг
    static Embedding states_to_embedding(const float* states);
    
    // Преобразование эмбеддинга во внутренние состояния (обратное преобразование)
    static void embedding_to_states(const Embedding& embedding, float* states);
    
    // Создание эмбеддинга на основе эмоционального состояния
    static Embedding emotion_to_embedding(const std::string& emotion, float intensity);
    
    // Создание эмбеддинга на основе действия
    static Embedding action_to_embedding(const std::string& action);
    
    // Создание эмбеддинга на основе комбинации состояний
    static Embedding combined_embedding(const float* states, const std::string& emotion = "", 
                                      const std::string& action = "");
};

#endif // EMBEDDING_H