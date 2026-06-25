/*
 * save_data.c
 *
 *  Created on: 24 Jun 2026
 *      Author: hilla
 */
#include <stdint.h>
#include "save_data.h"
#include "at24cxx.h"

SaveData_t *get_save_data(void){

	static SaveData_t instance = {0};

	at_read_data((uint8_t *)&instance, sizeof(instance));

	return &instance;
}
