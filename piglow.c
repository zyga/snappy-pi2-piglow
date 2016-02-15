#include "piglow.h"

#include "sn3218.h"

#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

static const uint8_t piglow_to_sn3218[18] = {
    6, 7, 8, 5, 4, 9, // first arm
    17, 16, 15, 13, 11, 10, // second arm
    0, 1, 2, 3, 14, 12 // third arm
};

int piglow_open(struct piglow *glow, const char *device_path) {
    glow->fd = open(device_path, O_RDWR);
    if (glow->fd < 0) {
        return errno;
    }
    return 0;
}

int piglow_close(struct piglow *glow) {
    if (close(glow->fd) < 0) {
        return errno;
    }
    glow->fd = -1;
    return 0;
}

int piglow_powerctl(struct piglow *glow, bool active) {
    return sn3218_powerctl(glow->fd, active);
}

int piglow_pwmctl(struct piglow *glow, const uint8_t pwm[18]) {
    uint8_t remapped_pwm[18];
    for (int i=0; i<18; ++i)
        remapped_pwm[piglow_to_sn3218[i]] = pwm[i];
    return sn3218_pwmctl(glow->fd, remapped_pwm);
}

int piglow_ledctl(struct piglow *glow, const bool active[18]) {
    bool remapped_active[18];
    for (int i=0; i<18; ++i)
        remapped_active[piglow_to_sn3218[i]] = active[i];
    return sn3218_ledctl(glow->fd, remapped_active);
}

int piglow_update(struct piglow *glow) {
    return sn3218_update(glow->fd);
}

int piglow_reset(struct piglow *glow) {
    return sn3218_reset(glow->fd);
}
