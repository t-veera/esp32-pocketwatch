idf_component_register(
    SRCS "SensorRtcHelper.cpp"
         "SensorWireHelper.cpp"
         "SensorBHI260AP.cpp"
         "SensorBHI260AP_Klio.cpp"
         "SensorBMA423.cpp"
         "TouchDrvCSTXXX.cpp"
         "TouchDrvGT9895.cpp"
         "TouchDrvInterface.cpp"
    INCLUDE_DIRS "." "platform" "REG" "bosch" "touch"
    REQUIRES driver esp_timer
)

target_compile_definitions(${COMPONENT_LIB} PUBLIC 
    ESP_PLATFORM
    CONFIG_SENSORLIB_ESP_IDF_NEW_API
)
