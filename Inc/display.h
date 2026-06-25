// display.h

#ifndef DISPLAY_H
#define DISPLAY_H

#include "stm32f4xx.h"
#include <stdint.h>
#include "st7735.h"

#define DISPLAY_WIDTH   ST7735_WIDTH    // 128
#define DISPLAY_HEIGHT  ST7735_HEIGHT   // 160

#define DISPLAY_SPI SPI1

typedef enum {RECT, TEXT, IMAGE, CIRCLE} displayDataType_t;

typedef struct {
	displayDataType_t data_type;
	int16_t x;
	int16_t y;

	union {
		struct {
			uint16_t color;
			uint8_t w;
			uint8_t h;
		} rect;

		struct {
			uint16_t color;
			char str[30];
			FontDef *font;
			uint16_t bgColor;
		} text;

		struct {
			uint8_t w;
			uint8_t h;
			const uint16_t * image;
		} image;

		struct {
			uint8_t r;
			uint16_t color;
		} circle;
	};
} displayData_t;

extern uint16_t framebuffer[DISPLAY_HEIGHT][DISPLAY_WIDTH];
extern TaskHandle_t xDisplayTaskHandle;

void display_flush(void);

static inline void display_set_pixel(uint16_t x, uint16_t y, uint16_t color) {
    if (x < DISPLAY_WIDTH && y < DISPLAY_HEIGHT)
        framebuffer[y][x] = (color << 8) | (color >> 8);  // swap bytes
}

void init_display_dma(void);

/* Primitive draws — all write to buffer only */
void display_fill(uint16_t color);
void display_fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
void display_draw_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
void display_draw_line(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
void display_write_string(uint16_t x, uint16_t y, const char *str, FontDef font,
                          uint16_t color, uint16_t bgcolor);
void display_draw_image(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t * image);
void display_draw_circle(uint8_t x_c, uint8_t y_c, uint8_t r, uint16_t color);

#endif
