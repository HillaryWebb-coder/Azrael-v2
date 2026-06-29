/*
 * settings.c
 *
 *  Created on: 24 Jun 2026
 *      Author: hilla
 */
#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "main.h"
#include "gpio.h"
#include "tasks.h"
#include "st7735.h"
#include "globals.h"

void vSettingsTask(void *pvParameters){
	displayData_t menu_item;
	menu_item.data_type = TEXT;
	menu_item.text.font = &Font_7x10;
	menu_item.text.color = ST7735_WHITE;
	menu_item.text.bgColor = ST7735_BLUE;

	displayData_t title;
	title.data_type = TEXT;

	displayData_t clear_rect;
	clear_rect.data_type = RECT;
	clear_rect.x = 0;
	clear_rect.y = 0;
	clear_rect.rect.w = ST7735_WIDTH;
	clear_rect.rect.h = ST7735_HEIGHT;
	clear_rect.rect.color = ST7735_BLACK;

	displayData_t des_rect;
	des_rect.data_type = RECT;
	des_rect.x = 5;
	des_rect.y = 40;
	des_rect.data_type = RECT;
	des_rect.rect.color = ST7735_BLUE;
	des_rect.rect.w = 118;
	des_rect.rect.h = 100;

	displayData_t highlighter;
	highlighter.data_type = RECT;
	highlighter.x = 5;
	highlighter.rect.w = 2;
	highlighter.rect.h = 12;
	highlighter.rect.color = ST7735_WHITE;

	char menu_items_arr[][30] = {"set date & time", "Set Alarm", "About"};

	NunchuckData_t nun_data;

	uint8_t menu_pos = 0, joy_data_dir = 0, prev_joy_data_dir = 0;

	while(1){
		xEventGroupWaitBits(xSystemFlags, SETTINGS_SCREEN, pdFALSE, pdTRUE, portMAX_DELAY);

		xQueueSend(xDisplayQueue, &clear_rect, portMAX_DELAY);

		title.y = 5;
		title.x = 15;
		title.text.color = ST7735_WHITE;
		title.text.bgColor = ST7735_BLACK;
		title.text.font = &Font_11x18;
		snprintf(title.text.str, sizeof(title.text.str), "SETTINGS");
		xQueueSend(xDisplayQueue, &title, portMAX_DELAY);


		xQueueSend(xDisplayQueue, &des_rect, portMAX_DELAY);

		for(uint8_t i = 0; i < ARRAY_LEN(menu_items_arr); i++){
			menu_item.y = 45 + (i * 13);
			menu_item.x = 10;
			snprintf(menu_item.text.str, sizeof(menu_item.text.str), "%s", menu_items_arr[i]);
			xQueueSend(xDisplayQueue, &menu_item, portMAX_DELAY);
		}

		xSemaphoreTake(xNunchuckDataMutex, portMAX_DELAY);
		nun_data = g_nun_data;
		xSemaphoreGive(xNunchuckDataMutex);

		if(menu_pos > 2) menu_pos = 0;

		if(nun_data.joy_y < 30){
			joy_data_dir = 1;
		} else if(nun_data.joy_y > 225){
			joy_data_dir = 2;
		} else {
			joy_data_dir = 0;
		}


		if(prev_joy_data_dir != joy_data_dir){
			if(joy_data_dir == 1){
				menu_pos++;
			}else if(joy_data_dir == 2){
				menu_pos--;
			}
			prev_joy_data_dir = joy_data_dir;
		}



		highlighter.y = 45 + (menu_pos * 13);
		xQueueSend(xDisplayQueue, &highlighter, portMAX_DELAY);

//		led_on(LED_BLUE_PIN, LED_BLUE_GPIO);
//		vTaskDelay(pdMS_TO_TICKS(50));
//		led_off(LED_BLUE_PIN, LED_BLUE_GPIO);
//		vTaskDelay(pdMS_TO_TICKS(50));
		EventBits_t uxbits = xEventGroupWaitBits(xSystemFlags,
		                         OK_PRESSED | EXIT_PRESSED, pdTRUE, pdFALSE, pdMS_TO_TICKS(50));

		if (uxbits & OK_PRESSED) {
		    switch (menu_pos) {
		        case 0:
		            // transition to set date & time screen
		             xEventGroupClearBits(xSystemFlags, SETTINGS_SCREEN);
		             xEventGroupSetBits(xSystemFlags, SET_DATETIME_SCREEN);
		            break;
		        case 1:
		            // transition to alarm screen
		            break;
		        case 2:
		            // transition to about screen
		            break;
		    }
		} else if (uxbits & EXIT_PRESSED) {
		    xEventGroupClearBits(xSystemFlags, SETTINGS_SCREEN);
		    xEventGroupSetBits(xSystemFlags, MENU_SCREEN);
		}

		vTaskDelay(pdMS_TO_TICKS(30));
	}
}
