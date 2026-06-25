/*
 * aht10.c
 *
 *  Created on: 10 Jun 2026
 *      Author: hilla
 */
#include "stm32f4xx.h"
#include "rcc.h"
#include "tim.h"
#include "i2c.h"
#include "aht10.h"

#define AHT10_ADDR 0x38
#define AHT10_STATUS_BUSY (1UL << 7)
#define AHT10_STATUS_CALIB (1UL << 3)

void aht10_init(){
	delay_ms(40);

	// Soft reset
	uint8_t rst_cmd = 0xBA;
	i2c1_start();
	i2c1_addr(AHT10_ADDR);
	i2c1_write(&rst_cmd, sizeof(rst_cmd));
	i2c1_stop();
	delay_ms(20);

	// Calibration

	// Check if device is ready for measurement
	uint8_t status;
	do {
		delay_ms(10);
		i2c1_start();
		i2c1_read(AHT10_ADDR, &status, 1);

		if(!(status & AHT10_STATUS_CALIB)){
			uint8_t cal_cmds[] = {0xE1, 0x08, 0x00};
			i2c1_start();
			i2c1_addr(AHT10_ADDR);
			i2c1_write(cal_cmds, sizeof(cal_cmds));
			i2c1_stop();
			delay_ms(10);
		}
	} while (status & AHT10_STATUS_BUSY);

}

ErrorStatus aht10_read(AHT10Data_t *data){
	uint8_t buf[6];

	uint8_t cmds[] = {0xAC, 0x33, 0x00};
	i2c1_start();
	i2c1_addr(AHT10_ADDR);
	i2c1_write(cmds, sizeof(cmds));
	i2c1_stop();
	delay_ms(80);

	i2c1_start();
	i2c1_read(AHT10_ADDR, buf, sizeof(buf));

	if(buf[0] & AHT10_STATUS_BUSY) return ERROR;

    uint32_t raw_hum  = ((uint32_t)buf[1] << 12)
                      | ((uint32_t)buf[2] <<  4)
                      | ((uint32_t)buf[3] >>  4);

    uint32_t raw_temp = ((uint32_t)(buf[3] & 0x0F) << 16)
                      | ((uint32_t)buf[4] <<  8)
                      |  (uint32_t)buf[5];
    data->status = buf[0];
    // Integer alternative — avoids FPU dependency
    data->temperature = ((int64_t)raw_temp * 2000 / 1048576) - 500;
    // temp_x10 = 234 means 23.4°C

    data->humidity = (uint64_t)raw_hum * 1000 / 1048576;
    // hum_x10 = 612 means 61.2%

    return SUCCESS;
}
