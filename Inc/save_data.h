/*
 * save_data.h
 *
 *  Created on: 24 Jun 2026
 *      Author: hilla
 */

#ifndef SAVE_DATA_H_
#define SAVE_DATA_H_

typedef struct{
	uint8_t airRace_highScore;
	uint8_t pingPong_highScore;
} SaveData_t;


SaveData_t *get_save_data(void);

#endif /* SAVE_DATA_H_ */
