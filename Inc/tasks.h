/*
 * tasks.h
 *
 *  Created on: 16 Jun 2026
 *      Author: hilla
 */

#ifndef TASKS_H
#define TASKS_H

#include "FreeRTOS.h"
#include "task.h"

void vHomeTask(void *pvParameters);
void vDisplayTask(void *pvParameters);
void vReadNunchuckTask(void *pvParameters);
void vDisplayRTCTask(void *pvParameters);
void vMenuTask(void *pvParameters);
void vPingPongTask(void *pvParameters);
void vAirRace(void * pvParameters);
void vSettingsTask(void * pvParameters);

#endif
