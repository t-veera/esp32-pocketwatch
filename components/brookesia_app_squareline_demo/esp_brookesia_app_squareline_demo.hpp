/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "systems/phone/esp_brookesia_phone_app.hpp"
#include "SensorPCF85063.hpp"

namespace esp_brookesia::apps {

/**
 * @brief Clock app with RTC integration
 */
class SquarelineDemo: public systems::phone::App {
public:
    static SquarelineDemo *requestInstance(bool use_status_bar = false, bool use_navigation_bar = false);
    ~SquarelineDemo();

    using systems::phone::App::startRecordResource;
    using systems::phone::App::endRecordResource;

protected:
    SquarelineDemo(bool use_status_bar, bool use_navigation_bar);
    
    bool run(void) override;
    bool back(void) override;

private:
    static SquarelineDemo *_instance;
    SensorPCF85063 rtc;
    lv_timer_t *clock_update_timer;
    bool rtc_initialized;
    
    static void update_clock_callback(lv_timer_t *timer);
    void updateClockHands();
    static void IRAM_ATTR button_isr_handler(void* arg);
    static void button_task(void* arg);
    gpio_num_t button_gpio;
    bool display_sleeping;
};

} // namespace esp_brookesia::apps
