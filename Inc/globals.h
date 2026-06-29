#ifndef GLOBALS_H
#define GLOBALS_H

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"
#include "nunchuck.h"
#include "display.h"

// Event flag bits
#define OK_PRESSED       (1UL << 0)
#define EXIT_PRESSED     (1UL << 1)
#define HOME_SCREEN      (1UL << 2)
#define MENU_SCREEN      (1UL << 3)
#define PING_PONG_SCREEN (1UL << 4)
#define GAME_OVER        (1UL << 6)
#define AIR_RACE_SCREEN  (1UL << 7)
#define SETTINGS_SCREEN  (1UL << 8)
#define SET_DATETIME_SCREEN (1UL << 9)

// Shared objects — defined in main.c
extern QueueHandle_t     xDisplayQueue;
extern SemaphoreHandle_t xNunchuckDataMutex;
extern EventGroupHandle_t xSystemFlags;
extern NunchuckData_t    g_nun_data;

#define ARRAY_LEN(arr) (sizeof(arr) / sizeof((arr)[0]))
#endif
