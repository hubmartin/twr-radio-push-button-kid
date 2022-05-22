#include <application.h>

// Service mode interval defines how much time
#define BATTERY_UPDATE_INTERVAL (60 * 60 * 1000)

// LED instance
twr_led_t led;

// Button instance
twr_button_t button;

// Counters for button events
uint16_t button_click_count = 0;

// This function dispatches button events
void button_event_handler(twr_button_t *self, twr_button_event_t event, void *event_param)
{
    if (event == TWR_BUTTON_EVENT_PRESS)
    {
        // Pulse LED for 100 milliseconds
        twr_led_pulse(&led, 100);

        // Increment press count
        button_click_count++;

        twr_log_info("APP: Publish button press count = %u", button_click_count);

        // Publish button message on radio
        twr_radio_pub_push_button(&button_click_count);
    }
}

// This function dispatches battery events
void battery_event_handler(twr_module_battery_event_t event, void *event_param)
{
    // Update event?
    if (event == TWR_MODULE_BATTERY_EVENT_UPDATE)
    {
        float voltage;

        // Read battery voltage
        if (twr_module_battery_get_voltage(&voltage))
        {
            twr_log_info("APP: Battery voltage = %.2f", voltage);

            // Publish battery voltage
            twr_radio_pub_battery(&voltage);
        }
    }
}

void application_init(void)
{
    // Initialize log
    twr_log_init(TWR_LOG_LEVEL_INFO, TWR_LOG_TIMESTAMP_ABS);
    twr_log_info("APP: Reset");

    // Initialize LED
    twr_led_init(&led, TWR_GPIO_LED, false, false);
    twr_led_set_mode(&led, TWR_LED_MODE_OFF);

    // Initialize button
    twr_button_init(&button, TWR_GPIO_BUTTON, TWR_GPIO_PULL_DOWN, false);
    twr_button_set_event_handler(&button, button_event_handler, NULL);

    // Initialize battery
    twr_module_battery_init();
    twr_module_battery_set_event_handler(battery_event_handler, NULL);
    twr_module_battery_set_update_interval(BATTERY_UPDATE_INTERVAL);

    // Initialize radio
    twr_radio_init(TWR_RADIO_MODE_NODE_SLEEPING);

    // Send radio pairing request
    twr_radio_pairing_request("push-button-kid", VERSION);

    // Pulse LED
    twr_led_pulse(&led, 2000);
}
