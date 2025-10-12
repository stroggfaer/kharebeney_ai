# Документация агента Kharebeney

## Общее описание
Kharebeney - это искусственный интеллект для микроконтроллера ESP32-S3, реализованный в форме виртуального питомца (tamagotchi). Агент обладает эмоциями, системой обучения, памятью и жизненным циклом.

## Архитектура

### Основные компоненты

#### 1. InternalState (Система внутренних состояний)
- hunger (голод): 0.0-1.0
- happiness (счастье): 0.0-1.0
- health (здоровье): 0.0-1.0
- energy (энергия): 0.0-1.0
- social (социальность): 0.0-1.0
- curiosity (любопытство): 0.0-1.0
- level (уровень): целое число
- age (возраст): число секунд

#### 2. EmotionSystem (Система эмоций)
- joy (радость)
- apathy (апатия)
- discontent (недовольство)
- capricious (капризность)
- anger (сердитость)
- satisfaction (довольство)
- curiosity (любопытство)
- fear (страх)
- excitement (возбуждение)
- calm (спокойствие)
- loneliness (одиночество)
- love (любовь)

#### 3. LearningSystem (Система обучения)
- problem_solving (решение проблем)
- social_interaction (социальное взаимодействие)
- emotional_intelligence (эмоциональный интеллект)
- creativity (креативность)
- adaptability (адаптивность)
- memory (память)
- attention (внимание)
- patience (терпение)

#### 4. DecisionEngine (Движок принятия решений)
- feed (покормить)
- play (поиграть)
- heal (вылечить)
- rest (отдых)
- socialize (общение)
- explore (исследовать)
- wait (ожидание)

#### 5. MemorySystem (Система памяти)
Хранит важные события и действия агента с контекстом и эмоциями

#### 6. LifecycleSystem (Система жизненного цикла)
Управляет стадиями развития: newborn, child, teenager, adult, elder

#### 7. BalanceManager (Менеджер баланса)
Управляет балансом между исследованием (exploration) и эксплуатацией (exploitation)

## Интерфейс

### Основные методы
- `update(sensor_data)` - обновление состояния агента
- `get_status()` - получение текущего статуса
- `trigger_emotion(emotion, intensity)` - вызвать эмоцию
- `set_prompt(prompt_type, content)` - установить промт
- `save_state(filename)` - сохранить состояние
- `load_state(filename)` - загрузить состояние

## Файловая структура
```
src/
├── config.py          # Конфигурация пинов и интерфейса
├── main.py           # Точка запуска
├── launcher.py       # Запуск основного скрипта
├── core/             # Основные модули агента
│   ├── micropython_agent.py
│   ├── micropython_internal_state.py
│   ├── micropython_emotion_system.py
│   ├── micropython_learning_system.py
│   ├── micropython_simple_systems.py
│   ├── micropython_balance_manager.py
│   └── utils/
│       └── micropython_logger.py
├── db/               # Файлы состояния
│   ├── agent_state.json
│   ├── balance_state.json
│   └── storage.json
├── lib/              # Библиотеки (ssd1306 и др.)
├── diagram.json      # Схема подключения для симулятора
└── wokwi.toml        # Конфигурация симулятора
```

## Оптимизации
- Ограниченное количество состояний и эмоций для экономии памяти
- Сборка мусора каждые 10 шагов
- Сохранение состояния каждые 30 шагов
- Ограниченная история действий и воспоминаний
- Простые алгоритмы для микроконтроллера