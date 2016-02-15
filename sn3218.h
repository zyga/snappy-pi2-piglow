#pragma once
#include <stdint.h>
#include <stdbool.h>

// I2C address of the SN3218 chip.
#define SN3218_ADDR 0x54

// SN3218 shutdown register.
#define SN3218_REG_SHUTDOWN 0x00
// SN3218 PWM register base address (valid for + 0..17)
#define SN3218_REG_PWM_BASE 0x01
// SN32128 LED control register 1 (channel 1-6 enable bit).
#define SN3218_REG_LED_CTL1 0x13
// SN32128 LED control register 2 (channel 7-12 enable bit).
#define SN3218_REG_LED_CTL2 0x14
// SN32128 LED control register 3 (channel 13-18 enable bit).
#define SN3218_REG_LED_CTL3 0x15
// SN3218 update register.
#define SN3218_REG_UPDATE 0x16
// SN3218 state reset register.
#define SN3218_REG_RESET 0x17

// Argument for SN3218_REG_SHUTDOWN, puts the chip to soft-shutdown mode.
#define SN3218_MODE_SOFTWARE_SHUTDOWN 0x00
// Argument for SN3218_REG_SHUTDOWN, turns the chip on.
#define SN3218_MODE_NORMAL_OPERATION 0x01

// Control the power state of SN3218 chip.
int sn3218_powerctl(int fd, bool active);
// Control the PWM duty cycle of each of the 18 LEDs.
int sn3218_pwmctl(int fd, const uint8_t pwm[18]);
// Control the enable flag of each of the 18 LEDs.
int sn3218_ledctl(int fd, const bool active[18]);
// Commit values of PWM duty cycle and LED control registers.
int sn3218_update(int fd);
// Reset all state to initial values.
int sn3218_reset(int fd);
