#ifndef MOTOR_H
#define MOTOR_H
#include <stdint.h>

#define MOTOR_A 0x00
#define MOTOR_B 0x01

void motor_run(uint8_t motor, int8_t speed, uint16_t duration_ms);
void motor_stop(uint8_t motor);
void motor_stop_all(void);

#endif
