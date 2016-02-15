#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <argp.h>
#include <string.h>

#include "piglow.h"

sig_atomic_t shutdown = 0;

void on_sigint(int signo) {
    shutdown = 1;
}

const char *argp_program_version = "piglow 0.1";

/* Program documentation. */
static char doc[] =
  "Argp example #3 -- a program with options and arguments using argp";

/* A description of the arguments we accept. */
static char args_doc[] = "ARG1 ARG2";

/* The options we understand. */
static struct argp_option options[] = {
  {"breathe",   'b',  0,  0, "Produce breathing pattern"},
  {"cycle",     'c',  0,  0, "Cycle through all LEDs"},
  { 0 }
};

/* Used by main to communicate with parse_opt. */
struct arguments
{
  const char *mode;
};

/* Parse a single option. */
static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
  /* Get the input argument from argp_parse, which we
     know is a pointer to our arguments structure. */
  struct arguments *arguments = state->input;

  switch (key)
    {
    case 'b':
      arguments->mode = "breathe";
      break;
    case 'c':
      arguments->mode = "cycle";
      break;
    case ARGP_KEY_ARG:
      if (state->arg_num >= 0)
        /* Too many arguments. */
        argp_usage (state);
      break;
    case ARGP_KEY_END:
      if (state->arg_num < 0)
        /* Not enough arguments. */
        argp_usage (state);
      break;
    default:
      return ARGP_ERR_UNKNOWN;
    }
  return 0;
}

/* Our argp parser. */
static struct argp argp = { options, parse_opt, args_doc, doc };

int main(int argc, char **argv) {
    struct piglow glow;
    bool led[18];
    uint8_t pwm[18];
    int err;
    struct arguments arguments = {
        .mode = "cycle,breathe"
    };

    argp_parse (&argp, argc, argv, 0, 0, &arguments);
    // TODO: learn about this through the snappy i2c skill.
    if ((err = piglow_open(&glow, "/dev/i2c-1"))) {
        fprintf(stderr, "Cannot open piglow: %s\n", strerror(err));
        goto fail;
    }
    signal(SIGINT, on_sigint);
    // Initialize piglow
    printf("Resetting piglow\n");
    if ((err = piglow_reset(&glow))) {
        fprintf(stderr, "Cannot reset piglow: %s\n", strerror(err));
        goto fail;
    }
    printf("Powering piglow on\n");
    if ((err = piglow_powerctl(&glow, true))) {
        fprintf(stderr, "Cannot enable piglow: %s\n", strerror(err));
        goto fail;
    }
    if (strstr(arguments.mode, "cycle") != NULL) {
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
            if (shutdown)
                goto out;
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
    }
    if (strstr(arguments.mode, "breathe") != NULL) {
        // Fade all LEDs together
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
                if (shutdown)
                    goto out;
            }
            usleep(1000 * 1000);
            if (shutdown)
                goto out;
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
                if (shutdown)
                    goto out;
            }
            usleep(1000 * 1000);
            if (shutdown)
                goto out;
        }
    }
out:
    // Disable piglow
    if (shutdown) {
        printf("\n");
    }
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
