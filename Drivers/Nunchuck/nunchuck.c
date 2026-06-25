/*
 * nunchuck.c
 *
 *  Created on: 11 Jun 2026
 *      Author: hilla
 */

#include "stm32f4xx.h"
#include "tim.h"
#include "rcc.h"
#include "i2c.h"
#include "nunchuck.h"

#define NUNCHUCK_ADDR 0x52

void nunchuck_init(void){

	uint8_t cmd1[2] =  {0xF0, 0x55};
	uint8_t cmd2[2] = {0xFB, 0x00};

    i2c1_start();
    i2c1_addr(NUNCHUCK_ADDR);
	i2c1_write(cmd1, sizeof(cmd1));
	i2c1_stop();
	i2c1_start();
	i2c1_addr(NUNCHUCK_ADDR);
	i2c1_write(cmd2, sizeof(cmd2));
	i2c1_stop();
	delay_ms(10);
}

ErrorStatus nunchuck_read(NunchuckData_t *data){
	uint8_t buff[6];
	uint8_t req = 0x00;

	i2c1_start();
	i2c1_addr(NUNCHUCK_ADDR);
	i2c1_write(&req, 1);
	i2c1_stop();
	delay_ms(1);

	i2c1_start();
	i2c1_read(NUNCHUCK_ADDR, buff, sizeof(buff));

	data->joy_x = buff[0];
	data->joy_y = buff[1];
	data->accel_x = ((uint16_t)buff[2] << 2) | ((buff[5] >> 2) & 0x03);
	data->accel_y = ((uint16_t)buff[3] << 2) | ((buff[5] >> 4) & 0x03);
	data->accel_z = ((uint16_t)buff[4] << 2) | ((buff[5] >> 6) & 0x03);
	data->btn_z = !((buff[5] >> 0) & 0x01);
	data->btn_c = !((buff[5] >> 1) & 0x01);

	return SUCCESS;
}
