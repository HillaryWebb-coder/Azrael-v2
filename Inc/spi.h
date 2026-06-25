/*
 * spi.h
 *
 *  Created on: 9 Jun 2026
 *      Author: hilla
 */

#ifndef SPI_H_
#define SPI_H_

void spi1_init(void);
void spi1_transmit(uint8_t * data, uint32_t size);
void spi1_receive(uint8_t * data, uint32_t size);

void spi_dma_init(void);
void spi_dma_transmit(uint8_t * buff, uint16_t size);

#endif /* SPI_H_ */
