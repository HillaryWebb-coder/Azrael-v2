/*
 * aht10.h
 *
 *  Created on: 10 Jun 2026
 *      Author: hilla
 */

#ifndef AHT10_H_
#define AHT10_H_

typedef struct {
	uint8_t status;
	uint16_t humidity;
	uint16_t temperature;
} AHT10Data_t;

void aht10_init();
ErrorStatus aht10_read(AHT10Data_t *data);

#endif /* AHT10_H_ */
