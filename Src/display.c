// display.c
#include <string.h>
#include <stdlib.h>

#include "FreeRTOS.h"
#include "task.h"

#include "gpio.h"
#include "spi.h"
#include "display.h"
#include "st7735.h"
#include "fonts.h"

uint16_t framebuffer[DISPLAY_HEIGHT][DISPLAY_WIDTH];
TaskHandle_t xDisplayTaskHandle = NULL;

void display_flush(void){
	xDisplayTaskHandle = xTaskGetCurrentTaskHandle();

	ST7735_Select();
	ST7735_SetAddressWindow(0, 0, DISPLAY_WIDTH - 1, DISPLAY_HEIGHT - 1);
	disp_dc_high();

	spi_dma_transmit((uint8_t *)framebuffer, sizeof(framebuffer));

	ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

	ST7735_Unselect();
}

void display_fill(uint16_t color) {
    if (color == 0x0000 || color == 0xFFFF) {
        memset(framebuffer, color & 0xFF, sizeof(framebuffer));
    } else {
        uint32_t i;
        uint16_t *p = (uint16_t *)framebuffer;
        for (i = 0; i < DISPLAY_WIDTH * DISPLAY_HEIGHT; i++)
            *p++ = color;
    }

}

void display_fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
    if (x >= DISPLAY_WIDTH || y >= DISPLAY_HEIGHT) return;
    if (x + w > DISPLAY_WIDTH)  w = DISPLAY_WIDTH  - x;
    if (y + h > DISPLAY_HEIGHT) h = DISPLAY_HEIGHT - y;

    for (uint16_t row = y; row < y + h; row++) {
        for (uint16_t col = x; col < x + w; col++) {
            framebuffer[row][col] = (color << 8) | (color >> 8);
        }
    }
}

void display_draw_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
    display_fill_rect(x,         y,         w, 1, color); // top
    display_fill_rect(x,         y + h - 1, w, 1, color); // bottom
    display_fill_rect(x,         y,         1, h, color); // left
    display_fill_rect(x + w - 1, y,         1, h, color); // right
}

/* Bresenham line */
void display_draw_line(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {
    int16_t dx =  abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int16_t dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int16_t err = dx + dy, e2;

    while (1) {
        display_set_pixel(x0, y0, color);
        if (x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

void display_write_string(uint16_t x, uint16_t y, const char *str, FontDef font,
                          uint16_t color, uint16_t bgcolor) {
    while (*str) {
        if (x + font.width > DISPLAY_WIDTH) {
            x = 0;
            y += font.height;
            if (y + font.height > DISPLAY_HEIGHT) break;
            if (*str == ' ') { str++; continue; }
        }
        /* Render glyph directly into framebuffer */
        for (uint16_t i = 0; i < font.height; i++) {
            uint16_t b = font.data[(*str - 32) * font.height + i];
            for (uint16_t j = 0; j < font.width; j++) {
                display_set_pixel(x + j, y + i,
                    (b << j) & 0x8000 ? color : bgcolor);
            }
        }
        x += font.width;
        str++;
    }
}

void display_draw_image(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t *image) {
    if (x >= DISPLAY_WIDTH || y >= DISPLAY_HEIGHT) return;
    if (x + w > DISPLAY_WIDTH)  w = DISPLAY_WIDTH  - x;
    if (y + h > DISPLAY_HEIGHT) h = DISPLAY_HEIGHT - y;

    for (uint16_t row = y; row < y + h; row++) {
        for (uint16_t col = x; col < x + w; col++) {
            framebuffer[row][col] = *image++;
        }
    }
}

void display_draw_circle(uint8_t x_c, uint8_t y_c, uint8_t r, uint16_t color){
	if (x_c >= DISPLAY_WIDTH || y_c >= DISPLAY_HEIGHT) return;

	uint8_t point_in_circle = 0;

	for(uint8_t y = y_c - r; y < y_c + r; y++){
		for(uint8_t x = x_c - r; x < x_c + r; x++){
			/* check if point in circle - (x-x_C)^2 + (y-y_c)^2 < r */
			point_in_circle = (((x - x_c) * (x - x_c)) + ((y - y_c) * (y - y_c))) < (r * r);
			if(point_in_circle) display_set_pixel(x, y, color);
		}
	}
}
