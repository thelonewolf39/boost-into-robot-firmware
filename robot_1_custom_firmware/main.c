#include <stdint.h>
#include <stdbool.h>
#include "nrf_ble.h"      // Your BLE stack header
#include "motor.h"        // Motor control functions
#include "sensors.h"      // Sensor read functions
#include "utils.h"        // MAC generation, helpers

#define DEVICE_NAME "Robot_1"
#define COMMAND_CHAR_UUID 0xABCD
#define SENSOR_CHAR_UUID  0xDCBA

// Global BLE device MAC
uint8_t device_mac[6];

// BLE characteristic data
uint8_t command_data[5];
uint8_t sensor_data[5];

// Function prototypes
void ble_init(void);
void generate_mac(uint8_t *mac);
void process_command(uint8_t *data);
void send_sensor_data(void);

int main(void) {
    // 1️⃣ Generate MAC on first boot
    generate_mac(device_mac);

    // 2️⃣ Initialize BLE stack with generated MAC
    ble_init();
    ble_set_device_name(DEVICE_NAME);
    ble_set_mac_address(device_mac);

    // 3️⃣ Create characteristics
    ble_add_characteristic(COMMAND_CHAR_UUID, BLE_CHAR_WRITE, command_data, sizeof(command_data));
    ble_add_characteristic(SENSOR_CHAR_UUID,  BLE_CHAR_READ_NOTIFY, sensor_data, sizeof(sensor_data));

    // 4️⃣ Start advertising
    ble_start_advertising();

    while (1) {
        // Poll command characteristic
        if (ble_char_written(COMMAND_CHAR_UUID)) {
            ble_read_char(COMMAND_CHAR_UUID, command_data, sizeof(command_data));
            process_command(command_data);
        }

        // Read sensors and notify Pi
        read_sensors(sensor_data);  // Fill sensor_data array
        ble_notify(SENSOR_CHAR_UUID, sensor_data, sizeof(sensor_data));

        // Optional: simple delay / low-power sleep
        nrf_delay_ms(50);
    }
}

// ------------------------
// Command processing
// ------------------------
void process_command(uint8_t *data) {
    uint8_t type = data[0];
    uint8_t target = data[1];
    uint8_t value = data[2];
    uint16_t duration = data[3] | (data[4] << 8);

    if (type == 0x00) { // Motor command
        switch (target) {
            case 0x00: motor_run(MOTOR_A, value, duration); break;
            case 0x01: motor_run(MOTOR_B, value, duration); break;
            case 0x02: motor_run(MOTOR_A, value, duration);
                       motor_run(MOTOR_B, value, duration); break;
        }
    } else if (type == 0x01) { // Sensor request
        // Could handle on-demand sensor reads if needed
    } else if (type == 0x02) { // System command
        // e.g., stop all motors, LED, buzzer
        if (target == 0x00) { motor_stop_all(); }
    }
}

// ------------------------
// MAC generation (simple random static)
// ------------------------
void generate_mac(uint8_t *mac) {
    mac[0] = 0x02; // Locally administered, unicast
    for (int i = 1; i < 6; i++) {
        mac[i] = rand() & 0xFF;
    }
}
