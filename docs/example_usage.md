# Пример работы агента Kharebeney

## Инициализация агента

```python
from core.micropython_agent import MicroPythonKharebeneyAgent

# Создание агента
agent = MicroPythonKharebeneyAgent()

# Загрузка сохраненного состояния (если существует)
agent.load_state()

# Установка промтов
agent.set_prompt('personality', 'You are a smart AI agent for ESP32-S3.')
agent.set_prompt('behavior', 'You are optimized for microcontroller operation.')
```

## Основной цикл работы

```python
# Обновление агента
action = agent.update(sensor_data={})

# Получение статуса агента
status = agent.get_status()
print(f"Action: {status.get('current_action', 'N/A')}")
print(f"Level: {status['internal_states'].get('level', 'N/A')}")
print(f"Happiness: {status['internal_states'].get('happiness', 'N/A'):.2f}")
print(f"Energy: {status['internal_states'].get('energy', 'N/A'):.2f}")
print(f"Emotion: {status['emotional_state'].get('dominant_emotion', 'N/A')}")
```

## Взаимодействие с агентом

```python
# Вызов эмоции (например, при нажатии кнопки)
agent.trigger_emotion('joy', 0.7)

# Получение эмоционального описания
emotion_desc = agent.get_emotional_description()
print(f"Emotional state: {emotion_desc}")

# Поиск воспоминаний
memories = agent.search_memories("play")
print(f"Found {len(memories)} memories related to play")
```

## Сохранение состояния

```python
# Сохранение состояния перед выключением
agent.save_state()
```

## Пример вывода на дисплей

```python
def display_message(msg: str):
    """Выводит сообщение на дисплей и в консоль."""
    print(msg)  # Выводим в консоль для дебага
    oled.fill(0)
    # Разбиваем длинные сообщения на несколько строк
    if len(msg) > 16:
        oled.text(msg[:16], 0, 15)
        if len(msg) > 16:
            oled.text(msg[16:32], 0, 30)
        if len(msg) > 32:
            oled.text(msg[32:48], 0, 45)
    else:
        oled.text(msg, 10, 25)
    oled.show()
```

## Пример симуляции работы

```
=== Kharebeney AI Agent Debug Mode ===
Agent created
State loaded
Prompts set
AI Ready
Step 1 - Action: play
Step 2 - Action: explore
Step 3 - Action: rest
=== DEBUG STEP 5 ===
Action: play
Level: 1
Happiness: 0.62
Energy: 0.58
Emotion: joy
========================
Step 5 - Action: socialize
Button A pressed!
Joy emotion triggered
Step 6 (button) - Action: play
Step 7 (button) - Action: heal
Step 8 (button) - Action: rest
State saved
Demo complete
```

## Структура состояния агента

```json
{
  "step_count": 8,
  "current_action": "rest",
  "internal_states": {
    "hunger": 0.52,
    "happiness": 0.65,
    "health": 0.82,
    "energy": 0.71,
    "social": 0.48,
    "curiosity": 0.63,
    "level": 1,
    "age": 120.5,
    "time": 120.5
  },
  "emotional_state": {
    "dominant_emotion": "joy",
    "intensity": 0.7,
    "emotions": {
      "joy": 0.7,
      "apathy": 0.0,
      "discontent": 0.0,
      "capricious": 0.0,
      "anger": 0.0,
      "satisfaction": 0.0,
      "curiosity": 0.3,
      "fear": 0.0,
      "excitement": 0.0,
      "calm": 0.2,
      "loneliness": 0.0,
      "love": 0.1
    },
    "emotional_weight": 1.2
  },
  "learning_status": {
    "learning_level": 2,
    "experience_points": 150.5,
    "total_actions": 50,
    "successful_actions": 35,
    "success_rate": 0.7
  },
  "lifecycle_info": {
    "stage_name": "child",
    "age": 120.5,
    "biological_age": 12.05,
    "maturity_level": 0.12
  },
  "memory_summary": {
    "total_memories": 15,
    "retrievals": 3,
    "consolidations": 2
  },
  "prompt_statistics": {
    "total_prompts": 2,
    "average_effectiveness": 0.7,
    "most_effective_type": "personality"
  },
  "memory_usage": {
    "allocated": 12000,
    "free": 80000,
    "total": 92000,
    "usage_percent": 13.0
  }
}