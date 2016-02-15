#include "sn3218.h"

#include <errno.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

int sn3218_powerctl(int fd, bool active) {

    struct i2c_rdwr_ioctl_data msgs = {
        .msgs = (struct i2c_msg[]){
            {
            .addr = SN3218_ADDR,
            .len = 2,
            .buf = (uint8_t[2]){
                SN3218_REG_SHUTDOWN,
                active
                    ? SN3218_MODE_NORMAL_OPERATION
                    : SN3218_MODE_SOFTWARE_SHUTDOWN},
            },
        },
        .nmsgs = 1,
    };
    if (ioctl(fd, I2C_RDWR, &msgs) < 0) {
        return errno;
    }
    return 0;
}

static const uint8_t gamma32[32] = {
    0, 1, 2, 4, 6, 10, 13, 18,
    22, 28, 33, 39, 46, 53, 61, 69,
    78, 86, 96, 106, 116, 126, 138, 149,
    161, 173, 186, 199, 212, 226, 240, 255,
};

static int min(int a, int b) {
    return a < b ? a : b;
}

static float interpolate(float v0, float v1, float t) {
      return (1 - t) * v0 + t * v1;
}

int sn3218_pwmctl(int fd, const uint8_t pwm[18]) {
    uint8_t adj_pwm[18];
    for (int i=0; i<18; ++i) {
        int level = pwm[i];
        int bin = level / 8;
        float t = level % 8 / 8.0;
        float v0 = gamma32[bin];
        float v1 = gamma32[min(31, bin + 1)];
        adj_pwm[i] = (uint8_t)interpolate(v0, v1, t);
    }
    struct i2c_rdwr_ioctl_data msgs = {
        .msgs = (struct i2c_msg[]){
            {
            .addr = SN3218_ADDR,
            .len = 1 + 18,
            .buf = (uint8_t[1 + 18]){
                SN3218_REG_PWM_BASE,
                adj_pwm[0], adj_pwm[1], adj_pwm[2],
                adj_pwm[3], adj_pwm[4], adj_pwm[5],
                adj_pwm[6], adj_pwm[7], adj_pwm[8],
                adj_pwm[9], adj_pwm[10], adj_pwm[11],
                adj_pwm[12], adj_pwm[13], adj_pwm[14],
                adj_pwm[15], adj_pwm[16], adj_pwm[17]},
            },
        },
        .nmsgs = 1,
    };
    if (ioctl(fd, I2C_RDWR, &msgs) < 0) {
        return errno;
    }
    return 0;
}

int sn3218_ledctl(int fd, const bool active[18]) {
    struct i2c_rdwr_ioctl_data msgs = {
        .msgs = (struct i2c_msg[]){
            {
            .addr = SN3218_ADDR,
            .len = 1 + 3,
            .buf = (uint8_t[1 + 3]){
                SN3218_REG_LED_CTL1,
                ( (active[0] ? (1 << 0) : 0) |
                  (active[1] ? (1 << 1) : 0) |
                  (active[2] ? (1 << 2) : 0) |
                  (active[3] ? (1 << 3) : 0) |
                  (active[4] ? (1 << 4) : 0) |
                  (active[5] ? (1 << 5) : 0)),
                ( (active[6] ? (1 << 0) : 0) |
                  (active[7] ? (1 << 1) : 0) |
                  (active[8] ? (1 << 2) : 0) |
                  (active[9] ? (1 << 3) : 0) |
                  (active[10] ? (1 << 4) : 0) |
                  (active[11] ? (1 << 5) : 0)),
                ( (active[12] ? (1 << 0) : 0) |
                  (active[13] ? (1 << 1) : 0) |
                  (active[14] ? (1 << 2) : 0) |
                  (active[15] ? (1 << 3) : 0) |
                  (active[16] ? (1 << 4) : 0) |
                  (active[17] ? (1 << 5) : 0))
                  },
            },
        },
        .nmsgs = 1,
    };
    if (ioctl(fd, I2C_RDWR, &msgs) < 0) {
        return errno;
    }
    return 0;
}

int sn3218_update(int fd) {
    struct i2c_rdwr_ioctl_data msgs = {
        .msgs = (struct i2c_msg[]){
            {
            .addr = SN3218_ADDR,
            .len = 2,
            // NOTE: 0xff is irrelevant
            .buf = (uint8_t[2]){SN3218_REG_UPDATE, 0xFF},
            },
        },
        .nmsgs = 1,
    };
    if (ioctl(fd, I2C_RDWR, &msgs) < 0) {
        return errno;
    }
    return 0;
}

int sn3218_reset(int fd) {
    struct i2c_rdwr_ioctl_data msgs = {
        .msgs = (struct i2c_msg[]){
            {
            .addr = SN3218_ADDR,
            .len = 2,
            // NOTE: 0xff is irrelevant
            .buf = (uint8_t[2]){SN3218_REG_RESET, 0xFF},
            },
        },
        .nmsgs = 1,
    };
    if (ioctl(fd, I2C_RDWR, &msgs) < 0) {
        return errno;
    }
    return 0;
}
