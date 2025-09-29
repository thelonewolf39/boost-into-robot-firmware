#include "sensors.h"
#include <stdint.h>
#include "color_sensor.h"
#include "imu_sensor.h"
#include "button.h"

#define BLUE_COLOR_CODE 0x01
#define YELLOW_COLOR_CODE 0x02

void read_sensors(uint8_t *sensor_data) {
    sensor_data[0] = color_read();       // color sensor
    sensor_data[1] = imu_get_x();        // tilt X
    sensor_data[2] = imu_get_y();        // tilt Y
    sensor_data[3] = button_pressed() ? 1 : 0;
    sensor_data[4] = 0;                  // reserved
}
