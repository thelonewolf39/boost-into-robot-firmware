#include "motor.h"
#include <stdint.h>
#include "nrf_delay.h"
#include "motor_driver_api.h" // Replace with actual motor driver headers

void motor_run(uint8_t motor, int8_t speed, uint16_t duration_ms) {
    set_motor_pwm(motor, speed); // Map to actual driver

    if (duration_ms > 0) {
        nrf_delay_ms(duration_ms);
        motor_stop(motor);
    }
}

void motor_stop(uint8_t motor) {
    set_motor_pwm(motor, 0);
}

void motor_stop_all(void) {
    motor_stop(MOTOR_A);
    motor_stop(MOTOR_B);
}
