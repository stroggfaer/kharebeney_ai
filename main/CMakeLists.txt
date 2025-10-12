idf_component_register(
    SRCS "main.cpp"
         "core/internal_state.cpp"
         "core/balance_manager.cpp"
         "core/emotion_system.cpp"
         "core/simple_systems.cpp"
         "core/learning_system.cpp"
         "core/kharebeney_agent.cpp"
    INCLUDE_DIRS "." "core"
    REQUIRES i2cdev ssd1306 esp_timer
)
