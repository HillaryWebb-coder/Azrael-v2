/*
 * task_pingpong.c
 *
 *  Created on: 16 Jun 2026
 *      Author: hilla
 */

#include "globals.h"
#include "tasks.h"
#include "nunchuck.h"
#include "display.h"
#include "st7735.h"

#include <stdio.h>
#include <stdlib.h>

void vPingPongTask(void *pvParameters) {

	NunchuckData_t nun_data;

	displayData_t ball;
	ball.data_type = CIRCLE;
	ball.x = 61;
	ball.y = 77;
	ball.circle.r = 5;
	ball.circle.color = ST7735_WHITE;

	displayData_t clear_rect;
	clear_rect.x = 0;
	clear_rect.y = 0;
	clear_rect.data_type = RECT;
	clear_rect.rect.color = ST7735_BLACK;
	clear_rect.rect.w = DISPLAY_WIDTH;
	clear_rect.rect.h = DISPLAY_HEIGHT;

	displayData_t bat_rect;
	bat_rect.x = 47;
	bat_rect.y = 150;
	bat_rect.data_type = RECT;
	bat_rect.rect.color = ST7735_RED;
	bat_rect.rect.w = 30;
	bat_rect.rect.h = 5;

	displayData_t data;
	data.data_type = TEXT;
	data.y = 75;
	data.x = 20;
	data.text.color = ST7735_WHITE;
	data.text.bgColor = ST7735_BLUE;
	data.text.font = &Font_11x18;

	displayData_t comp_rect = bat_rect;
	comp_rect.y = 10;
	comp_rect.rect.color = ST7735_CYAN;

	uint8_t bat_speed = 4, comp_rect_div = ball.x, comp_rect_speed = 4;
	uint8_t ball_x_dir = 0, ball_y_dir = 0;
	uint8_t ball_speed = 1, ball_x_offset = 2;
	uint8_t ball_x_speed = ball_speed + ball_x_offset;

	while (1) {
		EventBits_t uxBits = xEventGroupWaitBits(xSystemFlags,
				PING_PONG_SCREEN | GAME_OVER, pdFALSE, pdFALSE, portMAX_DELAY);

		if (uxBits & PING_PONG_SCREEN) {
			if (uxBits & GAME_OVER) {
				xQueueSend(xDisplayQueue, &clear_rect, portMAX_DELAY);

				snprintf(data.text.str, sizeof(data.text.str), "GAME OVER");

				xQueueSend(xDisplayQueue, &data, portMAX_DELAY);

				uxBits = xEventGroupWaitBits(xSystemFlags,
						OK_PRESSED | EXIT_PRESSED, pdTRUE, pdFALSE,
						portMAX_DELAY);

				if (uxBits & OK_PRESSED) {
					ball.x = rand() % (DISPLAY_WIDTH - 1);
					ball.y = 77;
					ball_x_dir = rand() % 2;   // random direction
					ball_y_dir = 1;         // always start moving toward player
					xEventGroupClearBits(xSystemFlags, GAME_OVER);
				} else if (uxBits & EXIT_PRESSED) {
					ball.x = rand() % (DISPLAY_WIDTH - 1);
					ball.y = 77;
					ball_x_dir = rand() % 2;   // random direction
					ball_y_dir = 1;         // always start moving toward player
					xEventGroupClearBits(xSystemFlags, GAME_OVER);
					xEventGroupClearBits(xSystemFlags, PING_PONG_SCREEN);
					xEventGroupSetBits(xSystemFlags, MENU_SCREEN);
				}
			} else {
				xSemaphoreTake(xNunchuckDataMutex, portMAX_DELAY);
				nun_data = g_nun_data;
				xSemaphoreGive(xNunchuckDataMutex);

				xQueueSend(xDisplayQueue, &clear_rect, portMAX_DELAY);

				if ((ball.x + ball.circle.r)
						>= (DISPLAY_WIDTH - ball_x_speed)) {
					ball_x_dir = 0;
				} else if ((ball.x - ball.circle.r) <= ball_x_speed) {
					ball_x_dir = 1;
				}

				if ((ball.y + ball.circle.r) >= bat_rect.y) {
					if ((ball.x + ball.circle.r) >= bat_rect.x
							&& (ball.x - ball.circle.r)
									<= (bat_rect.x + bat_rect.rect.w)) {
						ball_y_dir = 0;
					} else {
						xEventGroupSetBits(xSystemFlags, GAME_OVER);
					}
				} else if ((ball.y - ball.circle.r) <= comp_rect.y) {
					if ((comp_rect.x + comp_rect.circle.r) >= comp_rect.x
							&& (comp_rect.x - comp_rect.circle.r)
									<= (comp_rect.x + comp_rect.rect.w)) {
						ball_y_dir = 1;
					} else {
						xEventGroupSetBits(xSystemFlags, GAME_OVER);
					}
				}

				ball.x += ball_x_dir ? ball_x_speed : -ball_x_speed;
				ball.y += ball_y_dir ? ball_speed : -ball_speed;

				xQueueSend(xDisplayQueue, &ball, portMAX_DELAY);

				if (nun_data.joy_x > 138) {
					if ((bat_rect.x + bat_rect.rect.w)
							> DISPLAY_WIDTH - bat_speed) {
						bat_rect.x = DISPLAY_WIDTH - bat_rect.rect.w - 1;
					} else {
						bat_rect.x += bat_speed;
					}
				} else if (nun_data.joy_x < 118) {
					if (bat_rect.x <= bat_speed) {
						bat_rect.x = 1;
					} else {
						bat_rect.x -= bat_speed;
					}
				}

				if (comp_rect.x > comp_rect_div) {
					comp_rect.x -= comp_rect_speed;
				} else if (comp_rect.x < comp_rect_div) {
					comp_rect.x += comp_rect_speed;
				}

				if (abs(comp_rect.x - comp_rect_div) <= comp_rect_speed) {
					comp_rect_div = ball.x - ball.circle.r;
					if (comp_rect_div > DISPLAY_WIDTH)
						comp_rect_div = 0;
					if (comp_rect_div > (DISPLAY_WIDTH - comp_rect.rect.w))
						comp_rect_div = DISPLAY_WIDTH - comp_rect_div;
				}

				xQueueSend(xDisplayQueue, &bat_rect, portMAX_DELAY);
				xQueueSend(xDisplayQueue, &comp_rect, portMAX_DELAY);

				uxBits = xEventGroupWaitBits(xSystemFlags,
						OK_PRESSED | EXIT_PRESSED, pdTRUE, pdFALSE,
						pdMS_TO_TICKS(5));

				if (uxBits & OK_PRESSED) {
				} else if (uxBits & EXIT_PRESSED) {
					xEventGroupClearBits(xSystemFlags, PING_PONG_SCREEN);
					xEventGroupSetBits(xSystemFlags, MENU_SCREEN);
				}
			}
		}
		vTaskDelay(pdMS_TO_TICKS(16));
	}

}
