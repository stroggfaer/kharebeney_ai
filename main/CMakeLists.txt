idf_component_register(
    SRCS "main.cpp"
         "screen/menu.cpp"
    INCLUDE_DIRS "." "screen"
    REQUIRES i2cdev ssd1306 esp_timer
)
