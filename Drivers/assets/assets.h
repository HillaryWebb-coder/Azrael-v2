#ifndef MEDIA_ASSETS_H_
#define MEDIA_ASSETS_H_

#include <stdint.h>

#define FIGHTER_HEIGHT 20
#define FIGHTER_WIDTH 20

#define OBSTACLE_WIDTH 25
#define OBSTACLE_HEIGHT 25

#define EXPLOSION_WIDTH 30
#define EXPLOSION_HEIGHT 30

#define HEART_HEIGHT 10
#define HEART_WIDTH 10

extern const uint16_t fighter_image[FIGHTER_HEIGHT][FIGHTER_WIDTH];
extern const uint16_t obstacle_image[OBSTACLE_HEIGHT][OBSTACLE_WIDTH];
extern const uint16_t explosion_image[EXPLOSION_HEIGHT][EXPLOSION_WIDTH];
extern const uint16_t heart_image[HEART_HEIGHT][HEART_WIDTH];

#endif /* MEDIA_ASSETS_H_ */
