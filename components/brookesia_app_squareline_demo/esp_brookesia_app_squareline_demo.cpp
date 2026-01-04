/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "lvgl.h"
#include "esp_brookesia.hpp"
#ifdef ESP_UTILS_LOG_TAG
#   undef ESP_UTILS_LOG_TAG
#endif
#define ESP_UTILS_LOG_TAG "BS:Squareline"
#include "esp_lib_utils.h"
#include "ui/ui.h"
#include "esp_brookesia_app_squareline_demo.hpp"
#include "driver/i2c_master.h"

extern "C" i2c_master_bus_handle_t bsp_i2c_get_handle(void);

#define APP_NAME "Squareline"

using namespace std;
using namespace esp_brookesia::gui;
using namespace esp_brookesia::systems;

LV_IMG_DECLARE(esp_brookesia_app_icon_launcher_squareline_112_112);

namespace esp_brookesia::apps {

SquarelineDemo *SquarelineDemo::_instance = nullptr;

SquarelineDemo *SquarelineDemo::requestInstance(bool use_status_bar, bool use_navigation_bar)
{
    if (_instance == nullptr) {
        _instance = new SquarelineDemo(use_status_bar, use_navigation_bar);
    }
    return _instance;
}

SquarelineDemo::SquarelineDemo(bool use_status_bar, bool use_navigation_bar):
    App(APP_NAME, &esp_brookesia_app_icon_launcher_squareline_112_112, false, use_status_bar, use_navigation_bar),
    clock_update_timer(nullptr),
    rtc_initialized(false),
    button_gpio(GPIO_NUM_0),
    display_sleeping(false)
{
}

SquarelineDemo::~SquarelineDemo()
{
   if (clock_update_timer) {
        lv_timer_del(clock_update_timer);
    }
}

bool SquarelineDemo::run(void)
{
    ESP_UTILS_LOGD("Run");
    
    // Initialize RTC
    i2c_master_bus_handle_t i2c_handle = bsp_i2c_get_handle();
    if (rtc.begin(i2c_handle)) {
        rtc_initialized = true;
        
        // Set time to 00:05
//        struct tm timeinfo;
  //      timeinfo.tm_year = 2025 - 1900;
 //       timeinfo.tm_mon = 12 - 1;
 //       timeinfo.tm_mday = 12;
 //       timeinfo.tm_hour = 0;
 //       timeinfo.tm_min = 58;
 //       timeinfo.tm_sec = 0;
 //       rtc.setDateTime(timeinfo);
    }
    
    // Create all UI resources here
    phone_app_squareline_ui_init();
    
    // Create timer to update clock
    if (rtc_initialized) {
        clock_update_timer = lv_timer_create(update_clock_callback, 1000, this);
        updateClockHands(); // Update immediately
    }
    // Setup button for sleep/wake
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_NEGEDGE;
    io_conf.pin_bit_mask = (1ULL << button_gpio);
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config(&io_conf);
//    gpio_install_isr_service(0);
    gpio_isr_handler_add(button_gpio, button_isr_handler, (void*)this);
    return true;
}

bool SquarelineDemo::back(void)
{
    ESP_UTILS_LOGD("Back");
        // Delete timer
    if (clock_update_timer) {
        lv_timer_del(clock_update_timer);
        clock_update_timer = nullptr;
    }
    // If the app needs to exit, call notifyCoreClosed() to notify the core to close the app
    ESP_UTILS_CHECK_FALSE_RETURN(notifyCoreClosed(), false, "Notify core closed failed");

    return true;
}

void SquarelineDemo::update_clock_callback(lv_timer_t *timer)
{
    SquarelineDemo *app = (SquarelineDemo *)timer->user_data;
    if (app && app->rtc_initialized) {
        app->updateClockHands();
    }
}

void SquarelineDemo::updateClockHands()
{
    RTC_DateTime dt = rtc.getDateTime();
    int h = dt.getHour();
    int m = dt.getMinute();
    int s = dt.getSecond();
    
    // Update analog hands (LVGL uses tenths of degrees)
    lv_img_set_angle(ui_clock_image_hour, ((h % 12) * 300) + (m * 5));
    lv_img_set_angle(ui_clock_image_min, m * 60);
    lv_img_set_angle(ui_clock_image_sec, s * 60);
    
    // Update digital time
    char buf[6];
    snprintf(buf, sizeof(buf), "%02d:%02d", h, m);
    lv_label_set_text(ui_clock_label_clock_number, buf);
    // Update date label
    const char* days[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    const char* months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", 
                            "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    char date_buf[20];
    snprintf(date_buf, sizeof(date_buf), "%s %02d %s", 
             days[dt.getWeek()], dt.getDay(), months[dt.getMonth() - 1]);
    lv_label_set_text(ui_clock_small_label_date, date_buf);
}

void IRAM_ATTR SquarelineDemo::button_isr_handler(void* arg)
{
    SquarelineDemo* app = (SquarelineDemo*)arg;
    // Toggle display sleep state
    app->display_sleeping = !app->display_sleeping;
    
    if (app->display_sleeping) {
        // Turn off display backlight
        lv_display_set_default(NULL);
    } else {
        // Wake up display
        lv_display_set_default(lv_display_get_next(NULL));
    }
}
// bool SquarelineDemo::close(void)
// {
//     ESP_UTILS_LOGD("Close");

//     /* Do some operations here if needed */

//     return true;
// }

// bool SquarelineDemo::init()
// {
//     ESP_UTILS_LOGD("Init");

//     /* Do some initialization here if needed */

//     return true;
// }

// bool SquarelineDemo::deinit()
// {
//     ESP_UTILS_LOGD("Deinit");

//     /* Do some deinitialization here if needed */

//     return true;
// }

// bool SquarelineDemo::pause()
// {
//     ESP_UTILS_LOGD("Pause");

//     /* Do some operations here if needed */

//     return true;
// }

// bool SquarelineDemo::resume()
// {
//     ESP_UTILS_LOGD("Resume");

//     /* Do some operations here if needed */

//     return true;
// }

// bool SquarelineDemo::cleanResource()
// {
//     ESP_UTILS_LOGD("Clean resource");

//     /* Do some cleanup here if needed */

//     return true;
// }

extern "C" {

    /**
     * The following functions are generated by Squareline and records resources before and after creating animations,
     * allowing for automatic cleanup of animation resources when the app exits. This prevents errors that may occur when
     * animations call UI elements that have already been cleaned up.
     *
     */
    void upanim_Animation(lv_obj_t *TargetObject, int delay)
    {
        ui_anim_user_data_t *PropertyAnimation_0_user_data = (ui_anim_user_data_t *)lv_malloc(sizeof(ui_anim_user_data_t));
        PropertyAnimation_0_user_data->target = TargetObject;
        PropertyAnimation_0_user_data->val = -1;
        lv_anim_t PropertyAnimation_0;
        lv_anim_init(&PropertyAnimation_0);
        lv_anim_set_time(&PropertyAnimation_0, 200);
        lv_anim_set_user_data(&PropertyAnimation_0, PropertyAnimation_0_user_data);
        lv_anim_set_custom_exec_cb(&PropertyAnimation_0, _ui_anim_callback_set_y);
        lv_anim_set_values(&PropertyAnimation_0, -30, 0);
        lv_anim_set_path_cb(&PropertyAnimation_0, lv_anim_path_ease_out);
        lv_anim_set_delay(&PropertyAnimation_0, delay + 0);
        lv_anim_set_deleted_cb(&PropertyAnimation_0, _ui_anim_callback_free_user_data);
        lv_anim_set_playback_time(&PropertyAnimation_0, 0);
        lv_anim_set_playback_delay(&PropertyAnimation_0, 0);
        lv_anim_set_repeat_count(&PropertyAnimation_0, 0);
        lv_anim_set_repeat_delay(&PropertyAnimation_0, 0);
        lv_anim_set_early_apply(&PropertyAnimation_0, false);
        lv_anim_set_get_value_cb(&PropertyAnimation_0, &_ui_anim_callback_get_y);

        ESP_UTILS_CHECK_FALSE_EXIT(
            SquarelineDemo::requestInstance()->startRecordResource(), "Start record resource failed"
        );
        lv_anim_start(&PropertyAnimation_0);
        ESP_UTILS_CHECK_FALSE_EXIT(
            SquarelineDemo::requestInstance()->endRecordResource(), "End record resource failed"
        );

        ui_anim_user_data_t *PropertyAnimation_1_user_data = (ui_anim_user_data_t *)lv_malloc(sizeof(ui_anim_user_data_t));
        PropertyAnimation_1_user_data->target = TargetObject;
        PropertyAnimation_1_user_data->val = -1;
        lv_anim_t PropertyAnimation_1;
        lv_anim_init(&PropertyAnimation_1);
        lv_anim_set_time(&PropertyAnimation_1, 100);
        lv_anim_set_user_data(&PropertyAnimation_1, PropertyAnimation_1_user_data);
        lv_anim_set_custom_exec_cb(&PropertyAnimation_1, _ui_anim_callback_set_opacity);
        lv_anim_set_values(&PropertyAnimation_1, 0, 255);
        lv_anim_set_path_cb(&PropertyAnimation_1, lv_anim_path_linear);
        lv_anim_set_delay(&PropertyAnimation_1, delay + 0);
        lv_anim_set_deleted_cb(&PropertyAnimation_1, _ui_anim_callback_free_user_data);
        lv_anim_set_playback_time(&PropertyAnimation_1, 0);
        lv_anim_set_playback_delay(&PropertyAnimation_1, 0);
        lv_anim_set_repeat_count(&PropertyAnimation_1, 0);
        lv_anim_set_repeat_delay(&PropertyAnimation_1, 0);
        lv_anim_set_early_apply(&PropertyAnimation_1, true);

        ESP_UTILS_CHECK_FALSE_EXIT(
            SquarelineDemo::requestInstance()->startRecordResource(), "Start record resource failed"
        );
        lv_anim_start(&PropertyAnimation_1);
        ESP_UTILS_CHECK_FALSE_EXIT(
            SquarelineDemo::requestInstance()->endRecordResource(), "End record resource failed"
        );

    }
    void hour_Animation(lv_obj_t *TargetObject, int delay)
    {
        ui_anim_user_data_t *PropertyAnimation_0_user_data = (ui_anim_user_data_t *)lv_malloc(sizeof(ui_anim_user_data_t));
        PropertyAnimation_0_user_data->target = TargetObject;
        PropertyAnimation_0_user_data->val = -1;
        lv_anim_t PropertyAnimation_0;
        lv_anim_init(&PropertyAnimation_0);
        lv_anim_set_time(&PropertyAnimation_0, 1000);
        lv_anim_set_user_data(&PropertyAnimation_0, PropertyAnimation_0_user_data);
        lv_anim_set_custom_exec_cb(&PropertyAnimation_0, _ui_anim_callback_set_image_angle);
        lv_anim_set_values(&PropertyAnimation_0, 0, 2800);
        lv_anim_set_path_cb(&PropertyAnimation_0, lv_anim_path_ease_out);
        lv_anim_set_delay(&PropertyAnimation_0, delay + 0);
        lv_anim_set_deleted_cb(&PropertyAnimation_0, _ui_anim_callback_free_user_data);
        lv_anim_set_playback_time(&PropertyAnimation_0, 0);
        lv_anim_set_playback_delay(&PropertyAnimation_0, 0);
        lv_anim_set_repeat_count(&PropertyAnimation_0, 0);
        lv_anim_set_repeat_delay(&PropertyAnimation_0, 0);
        lv_anim_set_early_apply(&PropertyAnimation_0, false);

        ESP_UTILS_CHECK_FALSE_EXIT(
            SquarelineDemo::requestInstance()->startRecordResource(), "Start record resource failed"
        );
        lv_anim_start(&PropertyAnimation_0);
        ESP_UTILS_CHECK_FALSE_EXIT(
            SquarelineDemo::requestInstance()->endRecordResource(), "End record resource failed"
        );

        ui_anim_user_data_t *PropertyAnimation_1_user_data = (ui_anim_user_data_t *)lv_malloc(sizeof(ui_anim_user_data_t));
        PropertyAnimation_1_user_data->target = TargetObject;
        PropertyAnimation_1_user_data->val = -1;
        lv_anim_t PropertyAnimation_1;
        lv_anim_init(&PropertyAnimation_1);
        lv_anim_set_time(&PropertyAnimation_1, 300);
        lv_anim_set_user_data(&PropertyAnimation_1, PropertyAnimation_1_user_data);
        lv_anim_set_custom_exec_cb(&PropertyAnimation_1, _ui_anim_callback_set_opacity);
        lv_anim_set_values(&PropertyAnimation_1, 0, 255);
        lv_anim_set_path_cb(&PropertyAnimation_1, lv_anim_path_linear);
        lv_anim_set_delay(&PropertyAnimation_1, delay + 0);
        lv_anim_set_deleted_cb(&PropertyAnimation_1, _ui_anim_callback_free_user_data);
        lv_anim_set_playback_time(&PropertyAnimation_1, 0);
        lv_anim_set_playback_delay(&PropertyAnimation_1, 0);
        lv_anim_set_repeat_count(&PropertyAnimation_1, 0);
        lv_anim_set_repeat_delay(&PropertyAnimation_1, 0);
        lv_anim_set_early_apply(&PropertyAnimation_1, true);

        ESP_UTILS_CHECK_FALSE_EXIT(
            SquarelineDemo::requestInstance()->startRecordResource(), "Start record resource failed"
        );
        lv_anim_start(&PropertyAnimation_1);
        ESP_UTILS_CHECK_FALSE_EXIT(
            SquarelineDemo::requestInstance()->endRecordResource(), "End record resource failed"
        );

    }
    void min_Animation(lv_obj_t *TargetObject, int delay)
    {
        ui_anim_user_data_t *PropertyAnimation_0_user_data = (ui_anim_user_data_t *)lv_malloc(sizeof(ui_anim_user_data_t));
        PropertyAnimation_0_user_data->target = TargetObject;
        PropertyAnimation_0_user_data->val = -1;
        lv_anim_t PropertyAnimation_0;
        lv_anim_init(&PropertyAnimation_0);
        lv_anim_set_time(&PropertyAnimation_0, 1000);
        lv_anim_set_user_data(&PropertyAnimation_0, PropertyAnimation_0_user_data);
        lv_anim_set_custom_exec_cb(&PropertyAnimation_0, _ui_anim_callback_set_image_angle);
        lv_anim_set_values(&PropertyAnimation_0, 0, 2100);
        lv_anim_set_path_cb(&PropertyAnimation_0, lv_anim_path_ease_out);
        lv_anim_set_delay(&PropertyAnimation_0, delay + 0);
        lv_anim_set_deleted_cb(&PropertyAnimation_0, _ui_anim_callback_free_user_data);
        lv_anim_set_playback_time(&PropertyAnimation_0, 0);
        lv_anim_set_playback_delay(&PropertyAnimation_0, 0);
        lv_anim_set_repeat_count(&PropertyAnimation_0, 0);
        lv_anim_set_repeat_delay(&PropertyAnimation_0, 0);
        lv_anim_set_early_apply(&PropertyAnimation_0, false);

        ESP_UTILS_CHECK_FALSE_EXIT(
            SquarelineDemo::requestInstance()->startRecordResource(), "Start record resource failed"
        );
        lv_anim_start(&PropertyAnimation_0);
        ESP_UTILS_CHECK_FALSE_EXIT(
            SquarelineDemo::requestInstance()->endRecordResource(), "End record resource failed"
        );

        ui_anim_user_data_t *PropertyAnimation_1_user_data = (ui_anim_user_data_t *)lv_malloc(sizeof(ui_anim_user_data_t));
        PropertyAnimation_1_user_data->target = TargetObject;
        PropertyAnimation_1_user_data->val = -1;
        lv_anim_t PropertyAnimation_1;
        lv_anim_init(&PropertyAnimation_1);
        lv_anim_set_time(&PropertyAnimation_1, 200);
        lv_anim_set_user_data(&PropertyAnimation_1, PropertyAnimation_1_user_data);
        lv_anim_set_custom_exec_cb(&PropertyAnimation_1, _ui_anim_callback_set_opacity);
        lv_anim_set_values(&PropertyAnimation_1, 0, 255);
        lv_anim_set_path_cb(&PropertyAnimation_1, lv_anim_path_linear);
        lv_anim_set_delay(&PropertyAnimation_1, delay + 0);
        lv_anim_set_deleted_cb(&PropertyAnimation_1, _ui_anim_callback_free_user_data);
        lv_anim_set_playback_time(&PropertyAnimation_1, 0);
        lv_anim_set_playback_delay(&PropertyAnimation_1, 0);
        lv_anim_set_repeat_count(&PropertyAnimation_1, 0);
        lv_anim_set_repeat_delay(&PropertyAnimation_1, 0);
        lv_anim_set_early_apply(&PropertyAnimation_1, true);

        ESP_UTILS_CHECK_FALSE_EXIT(
            SquarelineDemo::requestInstance()->startRecordResource(), "Start record resource failed"
        );
        lv_anim_start(&PropertyAnimation_1);
        ESP_UTILS_CHECK_FALSE_EXIT(
            SquarelineDemo::requestInstance()->endRecordResource(), "End record resource failed"
        );

    }
    void sec_Animation(lv_obj_t *TargetObject, int delay)
    {
        ui_anim_user_data_t *PropertyAnimation_0_user_data = (ui_anim_user_data_t *)lv_malloc(sizeof(ui_anim_user_data_t));
        PropertyAnimation_0_user_data->target = TargetObject;
        PropertyAnimation_0_user_data->val = -1;
        lv_anim_t PropertyAnimation_0;
        lv_anim_init(&PropertyAnimation_0);
        lv_anim_set_time(&PropertyAnimation_0, 60000);
        lv_anim_set_user_data(&PropertyAnimation_0, PropertyAnimation_0_user_data);
        lv_anim_set_custom_exec_cb(&PropertyAnimation_0, _ui_anim_callback_set_image_angle);
        lv_anim_set_values(&PropertyAnimation_0, 0, 3600);
        lv_anim_set_path_cb(&PropertyAnimation_0, lv_anim_path_linear);
        lv_anim_set_delay(&PropertyAnimation_0, delay + 0);
        lv_anim_set_deleted_cb(&PropertyAnimation_0, _ui_anim_callback_free_user_data);
        lv_anim_set_playback_time(&PropertyAnimation_0, 0);
        lv_anim_set_playback_delay(&PropertyAnimation_0, 0);
        lv_anim_set_repeat_count(&PropertyAnimation_0, LV_ANIM_REPEAT_INFINITE);
        lv_anim_set_repeat_delay(&PropertyAnimation_0, 0);
        lv_anim_set_early_apply(&PropertyAnimation_0, false);

        ESP_UTILS_CHECK_FALSE_EXIT(
            SquarelineDemo::requestInstance()->startRecordResource(), "Start record resource failed"
        );
        lv_anim_start(&PropertyAnimation_0);
        ESP_UTILS_CHECK_FALSE_EXIT(
            SquarelineDemo::requestInstance()->endRecordResource(), "End record resource failed"
        );

        ui_anim_user_data_t *PropertyAnimation_1_user_data = (ui_anim_user_data_t *)lv_malloc(sizeof(ui_anim_user_data_t));
        PropertyAnimation_1_user_data->target = TargetObject;
        PropertyAnimation_1_user_data->val = -1;
        lv_anim_t PropertyAnimation_1;
        lv_anim_init(&PropertyAnimation_1);
        lv_anim_set_time(&PropertyAnimation_1, 1000);
        lv_anim_set_user_data(&PropertyAnimation_1, PropertyAnimation_1_user_data);
        lv_anim_set_custom_exec_cb(&PropertyAnimation_1, _ui_anim_callback_set_opacity);
        lv_anim_set_values(&PropertyAnimation_1, 0, 255);
        lv_anim_set_path_cb(&PropertyAnimation_1, lv_anim_path_linear);
        lv_anim_set_delay(&PropertyAnimation_1, delay + 0);
        lv_anim_set_deleted_cb(&PropertyAnimation_1, _ui_anim_callback_free_user_data);
        lv_anim_set_playback_time(&PropertyAnimation_1, 0);
        lv_anim_set_playback_delay(&PropertyAnimation_1, 0);
        lv_anim_set_repeat_count(&PropertyAnimation_1, 0);
        lv_anim_set_repeat_delay(&PropertyAnimation_1, 0);
        lv_anim_set_early_apply(&PropertyAnimation_1, true);

        ESP_UTILS_CHECK_FALSE_EXIT(
            SquarelineDemo::requestInstance()->startRecordResource(), "Start record resource failed"
        );
        lv_anim_start(&PropertyAnimation_1);
        ESP_UTILS_CHECK_FALSE_EXIT(
            SquarelineDemo::requestInstance()->endRecordResource(), "End record resource failed"
        );

    }
    void scrolldot_Animation(lv_obj_t *TargetObject, int delay)
    {
        ui_anim_user_data_t *PropertyAnimation_0_user_data = (ui_anim_user_data_t *)lv_malloc(sizeof(ui_anim_user_data_t));
        PropertyAnimation_0_user_data->target = TargetObject;
        PropertyAnimation_0_user_data->val = -1;
        lv_anim_t PropertyAnimation_0;
        lv_anim_init(&PropertyAnimation_0);
        lv_anim_set_time(&PropertyAnimation_0, 300);
        lv_anim_set_user_data(&PropertyAnimation_0, PropertyAnimation_0_user_data);
        lv_anim_set_custom_exec_cb(&PropertyAnimation_0, _ui_anim_callback_set_y);
        lv_anim_set_values(&PropertyAnimation_0, 30, -8);
        lv_anim_set_path_cb(&PropertyAnimation_0, lv_anim_path_ease_out);
        lv_anim_set_delay(&PropertyAnimation_0, delay + 0);
        lv_anim_set_deleted_cb(&PropertyAnimation_0, _ui_anim_callback_free_user_data);
        lv_anim_set_playback_time(&PropertyAnimation_0, 0);
        lv_anim_set_playback_delay(&PropertyAnimation_0, 0);
        lv_anim_set_repeat_count(&PropertyAnimation_0, 0);
        lv_anim_set_repeat_delay(&PropertyAnimation_0, 0);
        lv_anim_set_early_apply(&PropertyAnimation_0, true);

        ESP_UTILS_CHECK_FALSE_EXIT(
            SquarelineDemo::requestInstance()->startRecordResource(), "Start record resource failed"
        );
        lv_anim_start(&PropertyAnimation_0);
        ESP_UTILS_CHECK_FALSE_EXIT(
            SquarelineDemo::requestInstance()->endRecordResource(), "End record resource failed"
        );
    }

} // extern "C"

ESP_UTILS_REGISTER_PLUGIN_WITH_CONSTRUCTOR(systems::base::App, SquarelineDemo, APP_NAME, []()
{
    return std::shared_ptr<SquarelineDemo>(SquarelineDemo::requestInstance(), [](SquarelineDemo * p) {});
})

} // namespace esp_brookesia::apps
