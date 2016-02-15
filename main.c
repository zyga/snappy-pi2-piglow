#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "piglow.h"

int main(int argc, char **argv) {
    struct piglow glow;
    bool led[18];
    uint8_t pwm[18];
    int err;
    // TODO: learn about this through the snappy i2c skill.
    if ((err = piglow_open(&glow, "/dev/i2c-1"))) {
        fprintf(stderr, "Cannot open piglow: %s\n", strerror(err));
        goto fail;
    }
    // Enable piglow
    printf("Powering piglow on\n");
    if ((err = piglow_reset(&glow))) {
        fprintf(stderr, "Cannot reset piglow: %s\n", strerror(err));
        goto fail;
    }
    if ((err = piglow_powerctl(&glow, true))) {
        fprintf(stderr, "Cannot enable piglow: %s\n", strerror(err));
        goto fail;
    }
    // Cycle through each LEDs at medium brigtness.
    printf("Cycling through each LED\n");
    for (int i=0; i<18; ++i) {
        led[i] = false;
        pwm[i] = 127;
    }
    if ((err = piglow_pwmctl(&glow, pwm))) {
        fprintf(stderr, "Cannot set PWM levels: %s\n", strerror(err));
        goto fail;
    }
    for (int i=0; i<18; ++i) {
        led[i] = true;
        if ((err = piglow_ledctl(&glow, led))) {
            fprintf(stderr, "Cannot set LED control flags: %s\n", strerror(err));
            goto fail;
        }
        led[i] = false;
        if ((err = piglow_update(&glow))) {
            fprintf(stderr, "Cannot update piglow: %s\n", strerror(err));
            goto fail;
        }
        usleep(200 * 1000);
    }
    for (int i=0; i<18; ++i) {
        led[i] = false;
    }
    if ((err = piglow_ledctl(&glow, led))) {
        fprintf(stderr, "Cannot set LED control flags: %s\n", strerror(err));
        goto fail;
    }
    if ((err = piglow_pwmctl(&glow, pwm))) {
        fprintf(stderr, "Cannot set PWM levels: %s\n", strerror(err));
        goto fail;
    }
    if ((err = piglow_update(&glow))) {
        fprintf(stderr, "Cannot update piglow: %s\n", strerror(err));
        goto fail;
    }
    // Fade all LEDs together
    usleep(1 * 1000 * 1000);
    printf("Breathing...\n");
    for (int loop=0; loop<6; ++loop) {
        for (int i=0; i<18; ++i)
            led[i] = i % 6 == loop;
        if ((err = piglow_ledctl(&glow, led))) {
            fprintf(stderr, "Cannot set LED control flags: %s\n", strerror(err));
            goto fail;
        }
        for (int br=0; br<=255; ++br) {
            for (int i=0; i<18; ++i)
                pwm[i] = br;
            // Cycle through each LEDs at maximum brightness.
            if ((err = piglow_pwmctl(&glow, pwm))) {
                fprintf(stderr, "Cannot set PWM levels: %s\n", strerror(err));
                goto fail;
            }
            if ((err = piglow_update(&glow))) {
                fprintf(stderr, "Cannot update piglow: %s\n", strerror(err));
                goto fail;
            }
            usleep(2000);
        }
        usleep(1000 * 1000);
        for (int br=255; br>=0; --br) {
            for (int i=0; i<18; ++i)
                pwm[i] = br;
            // Cycle through each LEDs at maximum brightness.
            if ((err = piglow_pwmctl(&glow, pwm))) {
                fprintf(stderr, "Cannot set PWM levels: %s\n", strerror(err));
                goto fail;
            }
            if ((err = piglow_update(&glow))) {
                fprintf(stderr, "Cannot update piglow: %s\n", strerror(err));
                goto fail;
            }
            usleep(2000);
        }
        usleep(1000 * 1000);
    }
    // Disable piglow
    printf("Powering piglow off\n");
    if ((err = piglow_powerctl(&glow, false))) {
        fprintf(stderr, "Cannot disable piglow: %s\n", strerror(err));
        goto fail;
    }
    if ((err = piglow_close(&glow))) {
        fprintf(stderr, "Cannot close piglow: %s\n", strerror(err));
        goto fail;
    }
    return 0;
fail:
    piglow_close(&glow); // ignore errors
    return 1;
}
