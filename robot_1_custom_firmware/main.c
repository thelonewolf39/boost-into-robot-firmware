#include <stdint.h>
#include <stdbool.h>
#include "nrf_ble.h"
#include "motor.h"
#include "sensors.h"
#include "utils.h"

#define DEVICE_NAME "Zeus II"
#define COMMAND_CHAR_UUID 0xABCD
#define SENSOR_CHAR_UUID  0xDCBA

uint8_t device_mac[6];
uint8_t command_data[5];
uint8_t sensor_data[5];

// Function prototypes
void ble_init(void);
void process_command(uint8_t *data);

int main(void) {
    // 1️⃣ Generate MAC on first boot
    generate_mac(device_mac);

    // 2️⃣ Initialize BLE stack
    ble_init();
    ble_set_device_name(DEVICE_NAME);
    ble_set_mac_address(device_mac);

    // 3️⃣ Create characteristics
    ble_add_characteristic(COMMAND_CHAR_UUID, BLE_CHAR_WRITE, command_data, sizeof(command_data));
    ble_add_characteristic(SENSOR_CHAR_UUID,  BLE_CHAR_READ_NOTIFY, sensor_data, sizeof(sensor_data));

    // 4️⃣ Start advertising
    ble_start_advertising();

    while (1) {
        // Poll commands
        if (ble_char_written(COMMAND_CHAR_UUID)) {
            ble_read_char(COMMAND_CHAR_UUID, command_data, sizeof(command_data));
            process_command(command_data);
        }

        // Read sensors
        read_sensors(sensor_data);

        // Autonomous reactions
        // Example: tilt > threshold stops motors
        if (sensor_data[1] > 100 || sensor_data[2] > 100) {
            motor_stop_all();
        }

        // Example: color-based location logic
        if (sensor_data[0] == BLUE_COLOR_CODE) {
            // do something for downstairs
        } else if (sensor_data[0] == YELLOW_COLOR_CODE) {
            // do something for upstairs
        }

        // Notify Pi
        ble_notify(SENSOR_CHAR_UUID, sensor_data, sizeof(sensor_data));

        // Delay for loop timing
        nrf_delay_ms(50);
    }
}

// Command handling
void process_command(uint8_t *data) {
    uint8_t type = data[0];
    uint8_t target = data[1];
    uint8_t value = data[2];
    uint16_t duration = data[3] | (data[4] << 8);

    switch (type) {
        case 0x00: // Motor
            switch (target) {
                case 0x00: motor_run(MOTOR_A, value, duration); break;
                case 0x01: motor_run(MOTOR_B, value, duration); break;
                case 0x02: motor_run(MOTOR_A, value, duration);
                           motor_run(MOTOR_B, value, duration); break;
            }
            break;

        case 0x01: // System commands
            if (target == 0x00) motor_stop_all();
            break;

        case 0x02: // Patrol commands
            if (target == 0x00) { // forward patrol
                motor_run(MOTOR_A, 50, 1000);
                motor_run(MOTOR_B, 50, 1000);
            } else if (target == 0x01) { // turn
                motor_run(MOTOR_A, 50, 500);
                motor_run(MOTOR_B, -50, 500);
            }
            break;
    }
}
