/*
 * i2c.h
 *
 *  Created on: 10 Jun 2026
 *      Author: hilla
 */

#ifndef I2C_H_
#define I2C_H_


void i2c1_init(void);
void i2c1_start();
void i2c1_write(uint8_t * data, uint32_t size);
ErrorStatus i2c1_addr(uint8_t addr);
void i2c1_read(uint8_t addr, uint8_t * data, uint32_t size);
void i2c1_stop(void);

#endif /* I2C_H_ */
