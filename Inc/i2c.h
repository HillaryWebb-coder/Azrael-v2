/*
 * i2c.h
 *
 *  Created on: Apr 11, 2026
 *      Author: hillary
 */

#ifndef I2C_H_
#define I2C_H_

#include "stm32f4xx.h"

void i2c1_init(void);
void i2c1_byte_read(char saddr, char maddr, char *data);

#endif /* I2C_H_ */
