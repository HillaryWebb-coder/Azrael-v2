#include "stm32f4xx.h"
#include "i2c.h"

#include "at24cxx.h"

void at_page_write(uint16_t addr, uint8_t * data, uint16_t size){

	uint8_t addr_arr[] = {(addr >> 8), (addr & 0xFF)};
	i2c1_start();
	i2c1_addr(DEV_ADDR);
	i2c1_write(addr_arr, sizeof(addr_arr));
	i2c1_write(data, size);
	i2c1_stop();
}

void at_read_data(uint8_t *data_buf, uint16_t size){

	uint8_t reg_addr[] = {0x00, 0x00};

	i2c1_start();
	i2c1_addr(DEV_ADDR);
	i2c1_write(reg_addr, sizeof(reg_addr));
	i2c1_stop();
	i2c1_start();
	i2c1_read(DEV_ADDR, data_buf, size);
//	i2c1_stop();
}
