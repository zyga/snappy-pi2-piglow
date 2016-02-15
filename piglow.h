#pragma once
#include <stdbool.h>
#include <stdint.h>

struct piglow {
    int fd;
};

// Open the piglow i2c channel.
int piglow_open(struct piglow *glow, const char *device_path);
// Close the piglow i2c channel.
int piglow_close(struct piglow *glow);
// Control the power of the SN3218 support chip.
int piglow_powerctl(struct piglow *glow, bool active);
// Control the PWM duty cycle of each of the 18 LEDs.
int piglow_pwmctl(struct piglow *glow, const uint8_t pwm[18]);
// Control the enable flag of each of the 18 LEDs.
int piglow_ledctl(struct piglow *glow, const bool active[18]);
// Commit values of PWM duty cycle and LED control registers.
int piglow_update(struct piglow *glow);
// Reset all state to initial values.
int piglow_reset(struct piglow *glow);
