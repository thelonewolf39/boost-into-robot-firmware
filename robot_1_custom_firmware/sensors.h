#ifndef SENSORS_H
#define SENSORS_H
#include <stdint.h>

#define BLUE_COLOR_CODE 0x01
#define YELLOW_COLOR_CODE 0x02

void read_sensors(uint8_t *sensor_data);

#endif
