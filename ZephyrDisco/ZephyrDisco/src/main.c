/*
 * Copyright (c) 2018 Jan Van Winkel <jan.van_winkel@dxplore.eu>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/display.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>
#include <lvgl.h>
#include <stdio.h>
#include <string.h>
#include <zephyr/kernel.h>
#include <lvgl_input_device.h>

void lv_example_get_started_1(void);

#define LOG_LEVEL CONFIG_LOG_DEFAULT_LEVEL
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(app);

static uint32_t count;

#ifdef CONFIG_GPIO
static struct gpio_dt_spec button_gpio = GPIO_DT_SPEC_GET_OR(
		DT_ALIAS(sw0), gpios, {0});
static struct gpio_callback button_callback;

static void button_isr_callback(const struct device *port,
				struct gpio_callback *cb,
				uint32_t pins)
{
	ARG_UNUSED(port);
	ARG_UNUSED(cb);
	ARG_UNUSED(pins);

	count = 0;
}
#endif /* CONFIG_GPIO */




#ifdef CONFIG_LV_Z_ENCODER_INPUT
static const struct device *lvgl_encoder =
	DEVICE_DT_GET(DT_COMPAT_GET_ANY_STATUS_OKAY(zephyr_lvgl_encoder_input));
#endif /* CONFIG_LV_Z_ENCODER_INPUT */




#ifdef CONFIG_LV_Z_KEYPAD_INPUT
static const struct device *lvgl_keypad =
	DEVICE_DT_GET(DT_COMPAT_GET_ANY_STATUS_OKAY(zephyr_lvgl_keypad_input));
#endif /* CONFIG_LV_Z_KEYPAD_INPUT */



static void lv_btn_click_callback(lv_event_t *e)
{
	ARG_UNUSED(e);

	count = 0;
}

int main(void)
{
    const struct device * dht22 = DEVICE_DT_GET_ONE(aosong_dht);
    struct sensor_value temperature;
    struct sensor_value humidity;
    if (!device_is_ready(dht22)) {
        printf("Device %s is not ready\n", dht22->name);
        return 1;
    }

	char count_str[11] = {0};
	const struct device *display_dev;
	lv_obj_t *hello_world_label;
	lv_obj_t *count_label;

	display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
	if (!device_is_ready(display_dev)) {
		LOG_ERR("Device not ready, aborting test");
		return 0;
	}

#ifdef CONFIG_GPIO
	if (gpio_is_ready_dt(&button_gpio)) {
		int err;

		err = gpio_pin_configure_dt(&button_gpio, GPIO_INPUT);
		if (err) {
			LOG_ERR("failed to configure button gpio: %d", err);
			return 0;
		}

		gpio_init_callback(&button_callback, button_isr_callback,
				   BIT(button_gpio.pin));

		err = gpio_add_callback(button_gpio.port, &button_callback);
		if (err) {
			LOG_ERR("failed to add button callback: %d", err);
			return 0;
		}

		err = gpio_pin_interrupt_configure_dt(&button_gpio,
						      GPIO_INT_EDGE_TO_ACTIVE);
		if (err) {
			LOG_ERR("failed to enable button callback: %d", err);
			return 0;
		}
	}
#endif /* CONFIG_GPIO */


	if (IS_ENABLED(CONFIG_LV_Z_POINTER_INPUT)) {
		lv_obj_t *hello_world_button;

		hello_world_button = lv_btn_create(lv_scr_act());
		lv_obj_align(hello_world_button, LV_ALIGN_CENTER, 180, -100);
		lv_obj_add_event_cb(hello_world_button, lv_btn_click_callback, LV_EVENT_CLICKED,
				    NULL);
		hello_world_label = lv_label_create(hello_world_button); 
        
	} else {
		hello_world_label = lv_label_create(lv_scr_act());
       
	}

	lv_label_set_text(hello_world_label, "RESET");
	lv_obj_align(hello_world_label, LV_ALIGN_CENTER, 0, 0);

	lv_obj_t * label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, "Time: ");
    lv_obj_set_style_text_color(lv_scr_act(), lv_color_hex(0x000000), LV_PART_MAIN);
    lv_obj_align(label, LV_ALIGN_CENTER, 43, -100);


	count_label = lv_label_create(lv_scr_act());
    lv_obj_set_style_text_color(count_label, lv_color_hex(0x000000), LV_PART_MAIN);
	lv_obj_align(count_label, LV_ALIGN_BOTTOM_MID, 105, -228);

	 /*Create a chart*/
    lv_obj_t * chart;
    chart = lv_chart_create(lv_scr_act());
    lv_obj_set_size(chart, 200, 150);
    lv_obj_align(chart, LV_ALIGN_CENTER, 120, 25);
    lv_chart_set_type(chart, LV_CHART_TYPE_LINE);   /*Show lines and points too*/

    /*Add two data series*/
    lv_chart_series_t * ser1 = lv_chart_add_series(chart, lv_color_hex(0x0000ff), LV_CHART_AXIS_PRIMARY_Y);
    lv_chart_series_t * ser2 = lv_chart_add_series(chart, lv_color_hex(0xff0000), LV_CHART_AXIS_SECONDARY_Y);

    lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, 0, 100);
    lv_chart_set_range(chart, LV_CHART_AXIS_SECONDARY_Y, 0, 40);

    
    lv_chart_refresh(chart); /*Required after direct set*/

    lv_obj_t * label2 = lv_label_create(lv_scr_act());
    lv_label_set_text(label2, "Temperatur: ");
    lv_obj_set_style_text_color(label2, lv_color_hex(0xFF0000), LV_PART_MAIN);
    lv_obj_align(label2, LV_ALIGN_CENTER, 68, -42);

    lv_obj_t * label3 = lv_label_create(lv_scr_act());
    lv_label_set_text(label3, "Luftfeuchtigkeit: ");
    lv_obj_set_style_text_color(label3, lv_color_hex(0x0000ff), LV_PART_MAIN);
    lv_obj_align(label3, LV_ALIGN_CENTER, 83, 92);

    lv_obj_t * label5 = lv_label_create(lv_scr_act());
    lv_label_set_text(label5, "40°C \n\n\n 30°C \n\n\n 20°C \n\n\n 10°C \n\n\n 0°C ");
    lv_obj_set_style_text_color(label5, lv_color_hex(0x000000), LV_PART_MAIN);
    lv_obj_align(label5, LV_ALIGN_CENTER, -190, 0);

    lv_obj_t * label6 = lv_label_create(lv_scr_act());
    lv_label_set_text(label6, "100% \n\n\n 75% \n\n\n 50% \n\n\n 25% \n\n\n 0%");
    lv_obj_set_style_text_color(label6, lv_color_hex(0x000000), LV_PART_MAIN);
    lv_obj_align(label6, LV_ALIGN_CENTER, -70, 0);


    lv_example_get_started_1();

    static lv_style_t style_hum;
    lv_style_init(&style_hum);
    lv_style_set_bg_opa(&style_hum, LV_OPA_COVER);
    lv_style_set_bg_color(&style_hum, lv_palette_main(LV_PALETTE_BLUE));
    lv_style_set_bg_grad_color(&style_hum, lv_color_white());
    lv_style_set_bg_grad_dir(&style_hum, LV_GRAD_DIR_VER);

    lv_obj_t * humidity_bar = lv_bar_create(lv_scr_act());
    lv_obj_add_style(humidity_bar, &style_hum, LV_PART_INDICATOR);
    lv_obj_set_size(humidity_bar, 20, 200);
    lv_obj_align(humidity_bar, LV_ALIGN_CENTER, -40, 0);
    lv_bar_set_range(humidity_bar, 0, 100);

    static lv_style_t style_temp;
    lv_style_init(&style_temp);
    lv_style_set_bg_opa(&style_temp, LV_OPA_COVER);
    lv_style_set_bg_color(&style_temp, lv_palette_main(LV_PALETTE_RED));
    lv_style_set_bg_grad_color(&style_temp, lv_palette_main(LV_PALETTE_BLUE));
    lv_style_set_bg_grad_dir(&style_temp, LV_GRAD_DIR_VER);

    lv_obj_t * temperature_bar = lv_bar_create(lv_scr_act());
    lv_obj_add_style(temperature_bar, &style_temp, LV_PART_INDICATOR);
    lv_obj_set_size(temperature_bar, 20, 200);
    lv_obj_align(temperature_bar, LV_ALIGN_CENTER, -160, 0);
    lv_bar_set_range(temperature_bar, 0, 40);


	/////////////////////////////////////////////////////////////////////
	lv_task_handler();
	display_blanking_off(display_dev);

	while (1) {     
        int rc = sensor_sample_fetch(dht22);
        if (rc == 0) {
            rc = sensor_channel_get(dht22, SENSOR_CHAN_AMBIENT_TEMP, &temperature);
            if (rc == 0) {
                rc = sensor_channel_get(dht22, SENSOR_CHAN_HUMIDITY, &humidity);
            }
        }

        lv_bar_set_value(humidity_bar, sensor_value_to_float(&humidity), LV_ANIM_OFF);
        lv_bar_set_value(temperature_bar, sensor_value_to_float(&temperature), LV_ANIM_OFF);

        // true every second
		if ((count % 100) == 0U) {
            lv_chart_set_next_value(chart, ser2, sensor_value_to_float(&temperature));
            lv_chart_set_next_value(chart, ser1, sensor_value_to_float(&humidity));

			sprintf(count_str, "%d", count/100U);
			lv_label_set_text(count_label, count_str);
			lv_obj_set_style_text_color(lv_scr_act(), lv_color_hex(0x000000), LV_PART_MAIN);
		}
		lv_task_handler();
		++count;
		k_sleep(K_MSEC(10));
	}
}


void lv_example_get_started_1(void)
{
    /*Change the active screen's background color*/
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0xffffff), LV_PART_MAIN);
    
    
}
