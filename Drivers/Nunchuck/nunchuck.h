/*
 * nunchuck.h
 *
 *  Created on: 11 Jun 2026
 *      Author: hilla
 */

#ifndef NUNCHUCK_H_
#define NUNCHUCK_H_

#include "stm32f4xx.h"

typedef struct {
	uint8_t joy_x;
	uint8_t joy_y;
	uint16_t accel_x;
	uint16_t accel_y;
	uint16_t accel_z;
	uint8_t btn_z;
	uint8_t btn_c;
} NunchuckData_t;

void nunchuck_init(void);
ErrorStatus nunchuck_read(NunchuckData_t *data);


#endif /* NUNCHUCK_H_ */
