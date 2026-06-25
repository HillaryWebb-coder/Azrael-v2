#include <stdio.h>
#include <stdlib.h>

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

#include "main.h"
#include "globals.h"
#include "display.h"
#include "st7735.h"
#include "assets.h"
#include "gpio.h"
#include "save_data.h"

#define OBSTACLES_NO 3
#define FIGHTER_Y 140

struct obstacle {
	displayData_t obstacle_data;
	struct obstacle *next_obstacle;
};

struct game_data {
	uint8_t high_score;
	uint8_t score;
	uint8_t lives;
	uint8_t fighter_speed;
	uint8_t obstacle_speed;
	uint8_t obstacles_passed;
	uint8_t invulnerable;
	displayData_t *obstacles;
	displayData_t *fighter;
};

TimerHandle_t xinvulnerability_timer;
uint8_t invulnerable = 0;

void vInvulnerabilityTimerCallback(TimerHandle_t xTimer){
	if(xEventGroupGetBits(xSystemFlags) & AIR_RACE_SCREEN){
		invulnerable = 0;
	}
}

static void init_game(struct game_data *g) {

	g->fighter_speed = 5;
	g->lives = 2;
	g->obstacles_passed = 0;
	g->score = 0;
	g->obstacle_speed = 1;
	g->high_score = get_save_data()->airRace_highScore;

	for (uint8_t i = 0; i < OBSTACLES_NO; i++) {
		g->obstacles[i].data_type = IMAGE;
		g->obstacles[i].x = (rand() % 4) * (DISPLAY_WIDTH / 4);
		g->obstacles[i].y = -((rand() % OBSTACLES_NO)
				* (DISPLAY_HEIGHT / OBSTACLES_NO));
		g->obstacles[i].image.w = OBSTACLE_WIDTH;
		g->obstacles[i].image.h = OBSTACLE_HEIGHT;
		g->obstacles[i].image.image = (const uint16_t*) obstacle_image;
	}
}

static void update_obstacles(struct game_data *g) {

	for (uint8_t i = 0; i < OBSTACLES_NO; i++) {
		g->obstacles[i].y += g->obstacle_speed;
		xQueueSend(xDisplayQueue, &g->obstacles[i], portMAX_DELAY);
	}
}

static void check_collision(struct game_data *g) {

	for (uint8_t i = 0; i < OBSTACLES_NO; i++) {
		if (g->obstacles[i].y + OBSTACLE_HEIGHT >= FIGHTER_Y
				&& g->obstacles[i].y <= FIGHTER_Y + FIGHTER_HEIGHT) {
			// y overlap confirmed — check x
			if ((g->obstacles[i].x + OBSTACLE_WIDTH) >= g->fighter->x
					&& g->obstacles[i].x
							<= (g->fighter->x + g->fighter->image.w)) {
				if(!invulnerable){
					if (g->lives) {
						g->lives--;
						invulnerable = 1;
						xTimerStart(xinvulnerability_timer, portMAX_DELAY);
					} else {
						xEventGroupSetBits(xSystemFlags, GAME_OVER);
					}
				}
			}
		}

		if (g->obstacles[i].y >= DISPLAY_HEIGHT) {
			g->obstacles[i].x = (rand() % 4) * (DISPLAY_WIDTH / 4);
			g->obstacles[i].y = 0;
			g->obstacles_passed++;
		}
	}

}

static void game_over_screen(struct game_data *g, displayData_t *clear_rect, displayData_t *text){

	if(g->score > g->high_score){
		get_save_data()->airRace_highScore = g->score;
	}

	xQueueSend(xDisplayQueue, clear_rect, portMAX_DELAY);

	text->text.color = ST7735_RED;
	text->text.bgColor = ST7735_BLACK;
	text->text.font = &Font_7x10;
	text->x = 10;
	text->y = 10;
	snprintf(text->text.str, sizeof(text->text.str), "High Score: %d", g->high_score);
	xQueueSend(xDisplayQueue, text, portMAX_DELAY);

	text->x = 16;
	text->y = 25;
	text->text.font = &Font_7x10;
	snprintf(text->text.str, sizeof(text->text.str), "Score: %d", g->score);
	xQueueSend(xDisplayQueue, text, portMAX_DELAY);

	text->x = 19;
	text->y = 120;
	snprintf(text->text.str, sizeof(text->text.str), "EXIT to quit");
	xQueueSend(xDisplayQueue, text, portMAX_DELAY);

	text->x = 16;
	text->y = 140;
	snprintf(text->text.str, sizeof(text->text.str), "OK to restart");
	xQueueSend(xDisplayQueue, text, portMAX_DELAY);

	text->x = 15;
	text->y = 75;
	text->text.font = &Font_11x18;
	text->text.color = ST7735_WHITE;
	snprintf(text->text.str, sizeof(text->text.str), "GAME OVER");
	xQueueSend(xDisplayQueue, text, portMAX_DELAY);

}

void vAirRace(void *pvParameters) {
	displayData_t clear_rect;
	clear_rect.x = 0;
	clear_rect.y = 0;
	clear_rect.data_type = RECT;
	clear_rect.rect.color = ST7735_BLACK;
	clear_rect.rect.w = DISPLAY_WIDTH;
	clear_rect.rect.h = DISPLAY_HEIGHT;

	displayData_t data;
	data.data_type = TEXT;

	displayData_t hud_txt;
	hud_txt.data_type = TEXT;
	hud_txt.y = 0;
	hud_txt.x = 0;
	hud_txt.text.color = ST7735_RED;
	hud_txt.text.bgColor = ST7735_BLACK;
	hud_txt.text.font = &Font_7x10;

	displayData_t fighter_img;
	fighter_img.data_type = IMAGE;
	fighter_img.x = DISPLAY_WIDTH / 2;
	fighter_img.y = FIGHTER_Y;
	fighter_img.image.h = FIGHTER_HEIGHT;
	fighter_img.image.w = FIGHTER_WIDTH;
	fighter_img.image.image = (const uint16_t*) fighter_image;

	displayData_t heart_img;
	heart_img.data_type = IMAGE;
	heart_img.x = 0;
	heart_img.y = 0;
	heart_img.image.h = HEART_HEIGHT;
	heart_img.image.w = HEART_WIDTH;
	heart_img.image.image = (const uint16_t*) heart_image;

	displayData_t obstacles[OBSTACLES_NO];

	struct game_data p_game_data;
	p_game_data.fighter = &fighter_img;
	p_game_data.obstacles = obstacles;

	init_game(&p_game_data);

	NunchuckData_t nun_data;

	xinvulnerability_timer = xTimerCreate("Invulnerability", pdMS_TO_TICKS(1000), pdFALSE, (void *)0, vInvulnerabilityTimerCallback);

	while (1) {
		EventBits_t uxBits = xEventGroupWaitBits(xSystemFlags,
		AIR_RACE_SCREEN | GAME_OVER, pdFALSE, pdFALSE, portMAX_DELAY);

		if (uxBits & AIR_RACE_SCREEN) {
			if (uxBits & GAME_OVER) {
				game_over_screen(&p_game_data, &clear_rect, &data);

				uxBits = xEventGroupWaitBits(xSystemFlags,
				OK_PRESSED | EXIT_PRESSED, pdTRUE, pdFALSE,
				portMAX_DELAY);
				init_game(&p_game_data);

				if (uxBits & OK_PRESSED) {
					xEventGroupClearBits(xSystemFlags, GAME_OVER);
				} else {
					xEventGroupClearBits(xSystemFlags, GAME_OVER);
					xEventGroupClearBits(xSystemFlags, AIR_RACE_SCREEN);
					xEventGroupSetBits(xSystemFlags, MENU_SCREEN);
				}

			} else {
//				led_on(LED_GREEN_PIN, LED_GREEN_GPIO);
//				vTaskDelay(10);
//				led_off(LED_GREEN_PIN, LED_GREEN_GPIO);
//				vTaskDelay(10);

				xSemaphoreTake(xNunchuckDataMutex, portMAX_DELAY);
				nun_data = g_nun_data;
				xSemaphoreGive(xNunchuckDataMutex);

				if (nun_data.joy_x > 138) {
					if ((p_game_data.fighter->x + p_game_data.fighter_speed
							+ FIGHTER_WIDTH) < (DISPLAY_WIDTH - 1)) {
						p_game_data.fighter->x += p_game_data.fighter_speed;
					} else {
						p_game_data.fighter->x = DISPLAY_WIDTH - FIGHTER_WIDTH
								- 1;
					}
				} else if (nun_data.joy_x < 118) {
					if (abs(p_game_data.fighter->x - p_game_data.fighter_speed)
							> p_game_data.fighter_speed) {
						p_game_data.fighter->x -= p_game_data.fighter_speed;
					} else {
						p_game_data.fighter->x = 0;
					}
				}

				xQueueSend(xDisplayQueue, &clear_rect, portMAX_DELAY);
				update_obstacles(&p_game_data);
//				if(invulnerable){
//					xQueueSend(xDisplayQueue, p_game_data.fighter, portMAX_DELAY);
//				}
				xQueueSend(xDisplayQueue, p_game_data.fighter, portMAX_DELAY);
				check_collision(&p_game_data);

				if (p_game_data.obstacles_passed == OBSTACLES_NO) {
					p_game_data.obstacles_passed = 0;
					p_game_data.obstacle_speed++;
					p_game_data.score++;
				}

				for (uint8_t i = 0; i <= p_game_data.lives; i++) {
					heart_img.x = i * HEART_WIDTH + 3;
					xQueueSend(xDisplayQueue, &heart_img, portMAX_DELAY);
				}

				snprintf(hud_txt.text.str, sizeof(hud_txt.text.str),
						"Score: %d", p_game_data.score);
				hud_txt.x = 70;
				xQueueSend(xDisplayQueue, &hud_txt, portMAX_DELAY);

				uxBits = xEventGroupWaitBits(xSystemFlags,
				EXIT_PRESSED, pdTRUE, pdTRUE, pdMS_TO_TICKS(1));

				if (uxBits & EXIT_PRESSED) {
					xEventGroupClearBits(xSystemFlags, GAME_OVER);
					xEventGroupClearBits(xSystemFlags, AIR_RACE_SCREEN);
					xEventGroupSetBits(xSystemFlags, MENU_SCREEN);
				}
			}
		}
		vTaskDelay(pdMS_TO_TICKS(16));
	}
}
