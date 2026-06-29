#include <stdio.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"
#include "queue.h"

#include "globals.h"
#include "display.h"
#include "nunchuck.h"
#include "rtc.h"

void vUpdateDateTimeTask(void *pvParameters){
	displayData_t clear_rect;
	clear_rect.data_type = RECT;
	clear_rect.x = 0;
	clear_rect.y = 0;
	clear_rect.rect.w = DISPLAY_WIDTH;
	clear_rect.rect.h = DISPLAY_HEIGHT;
	clear_rect.rect.color = ST7735_BLACK;

	displayData_t title;
	title.data_type = TEXT;
	title.x = 40;
	title.y = 20;
	title.text.bgColor = ST7735_BLACK;
	title.text.color = ST7735_WHITE;
	title.text.font = &Font_11x18;

	displayData_t body;
	body.data_type = TEXT;
	body.x = 10;
	body.y = 80;
	body.text.color = ST7735_WHITE;
	body.text.bgColor = ST7735_BLACK;
	body.text.font = &Font_11x18;

	displayData_t highlighter;
	highlighter.data_type = TEXT;
	highlighter.text.color = ST7735_RED;
	highlighter.text.bgColor = ST7735_BLACK;
	highlighter.text.font = &Font_7x10;
	highlighter.x = 55;
	highlighter.y = 65;


	uint8_t dateTime_vals[6] = {0,0,0,0,0,0};
	uint8_t dateTime_texts[][4] = {"hr", "min", "sec", "day", "mon", "yr"};
	uint8_t pos  = 0;
	char * page = "TIME";

	NunchuckData_t nun_data;

	while(1){

		xEventGroupWaitBits(xSystemFlags, SET_DATETIME_SCREEN, pdFALSE, pdTRUE, portMAX_DELAY);

		xQueueSend(xDisplayQueue, &clear_rect, portMAX_DELAY);

		page = (pos < 3) ? "TIME" : "DATE";
		snprintf(title.text.str, sizeof(title.text.str), "%s", page);
		xQueueSend(xDisplayQueue, &title, portMAX_DELAY);

		xSemaphoreTake(xNunchuckDataMutex, portMAX_DELAY);
		nun_data = g_nun_data;
		xSemaphoreGive(xNunchuckDataMutex);

		if(pos <= 5){
			if(nun_data.joy_y < 30){
				dateTime_vals[pos]++;
			} else if (nun_data.joy_y > 225){
				dateTime_vals[pos]--;
			}
		} else {

		}

		if(dateTime_vals[0] > 23) dateTime_vals[0] = 0;
		if(dateTime_vals[1] > 60) dateTime_vals[1] = 0;
		if(dateTime_vals[2] > 60) dateTime_vals[2] = 0;

		if(dateTime_vals[3] > 31) dateTime_vals[3] = 0;
		if(dateTime_vals[4] > 12) dateTime_vals[4] = 0;
		if(dateTime_vals[5] > 99) dateTime_vals[5] = 0;

		snprintf(highlighter.text.str, sizeof(highlighter.text.str), "%s", dateTime_texts[pos]);
		xQueueSend(xDisplayQueue, &highlighter, portMAX_DELAY);
		if(pos < 3){
			snprintf(body.text.str, sizeof(body.text.str), "%02d: %02d: %02d", dateTime_vals[0], dateTime_vals[1], dateTime_vals[2]);
			xQueueSend(xDisplayQueue, &body, portMAX_DELAY);
		} else {
			snprintf(body.text.str, sizeof(body.text.str), "%02d: %02d: %02d", dateTime_vals[3], dateTime_vals[4], dateTime_vals[5]);
			xQueueSend(xDisplayQueue, &body, portMAX_DELAY);
		}

		EventBits_t uxBits = xEventGroupWaitBits(xSystemFlags, OK_PRESSED | EXIT_PRESSED, pdTRUE, pdFALSE, pdMS_TO_TICKS(50));

		if(uxBits & OK_PRESSED){
			if(pos < 5) {
				pos++;
			} else {
				uint8_t time_period = dateTime_vals[0] / 12 ? 1 : 0;
				uint8_t hr_format = dateTime_vals[0] > 12 ? dateTime_vals[0] - 12 : dateTime_vals[0];
				rtc_set_time(time_period, hr_format, dateTime_vals[1], dateTime_vals[2]);
				rtc_set_date(dateTime_vals[3], dateTime_vals[4], dateTime_vals[5]);

				pos = 0;
				memset(dateTime_vals, 0, sizeof dateTime_vals);
				xEventGroupClearBits(xSystemFlags, SET_DATETIME_SCREEN);
				xEventGroupSetBits(xSystemFlags, SETTINGS_SCREEN);
			}
		}else if(uxBits & EXIT_PRESSED){
			pos = 0;
			memset(dateTime_vals, 0, sizeof dateTime_vals);
			xEventGroupClearBits(xSystemFlags, SET_DATETIME_SCREEN);
			xEventGroupSetBits(xSystemFlags, SETTINGS_SCREEN);
		}
		vTaskDelay(pdMS_TO_TICKS(16));
	}

}
