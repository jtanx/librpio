/**
 * @file rpio.c
 * @brief Contains the implementation for all GPIO functions.
 * @author Jeremy Tan
 * @author Ben Croston
 * @copyright MIT License. See LICENSE for more information.
 */
 
#include <assert.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <memory.h>
#include <sys/mman.h>
#include <unistd.h>
#include "rpio.h"
#include "rpio-private.h"

const int pin_to_gpio_rev1[41] = {
     -1, -1, -1,  0, -1,  1, -1,  4, 14, -1,
     15, 17, 18, 21, -1, 22, 23, -1, 24, 10,
     -1,  9, 25, 11,  8, -1,  7, -1, -1, -1,
     -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};

const int pin_to_gpio_rev2[41] = {
     -1, -1, -1,  2, -1,  3, -1,  4, 14, -1,
     15, 17, 18, 27, -1, 22, 23, -1, 24, 10,
     -1,  9, 25, 11,  8, -1,  7, -1, -1, -1,
     -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};

const int pin_to_gpio_rev3[41] = {
     -1, -1, -1,  2, -1,  3, -1,  4, 14, -1,
     15, 17, 18, 27, -1, 22, 23, -1, 24, 10,
     -1,  9, 25, 11,  8, -1,  7, -1, -1,  5,
     -1,  6, 12, 13, -1, 19, 16, 26, 20, -1, 21
};

static volatile uint32_t *g_gpio_map = NULL;
static int g_pinmap = PINMAP_UNKNOWN;
static int g_revision = -1;
static const int *g_pin_to_gpio = NULL;
static int g_setup = 0;
static int g_pin_direction[54];

static void short_wait(void)
{
    int i;

    for (i=0; i<150; i++) { // wait 150 cycles
        __asm volatile("nop");
    }
}

static int get_gpio_number(int channel, int *gpio)
{
    assert(g_setup);
    assert(gpio);

    // Check channel number is in range
    if ((g_pinmap == PINMAP_BCM && (channel < 0 || channel > 53)) || 
        (g_pinmap == PINMAP_BOARD && (channel < 1 || channel > 26) && g_revision != 3) ||
        (g_pinmap == PINMAP_BOARD && (channel < 1 || channel > 40) && g_revision == 3))
    {
        return RPIO_INVALID_CHANNEL;
    }

    // convert channel to gpio
    if (g_pinmap == PINMAP_BOARD) {
        if (g_pin_to_gpio[channel] == -1) {
            return RPIO_INVALID_CHANNEL;
        } else {
            *gpio = g_pin_to_gpio[channel];
        }
    } else { // gpio_mode == BCM
        *gpio = channel;
    }

    return RPIO_OK;
}

static void set_pullupdn(int gpio, int pud)
{
    int clk_offset = PULLUPDNCLK_OFFSET + (gpio/32);
    int shift = (gpio%32);
    
    assert(g_setup);

    if (pud == PUD_DOWN)
        g_gpio_map[PULLUPDN_OFFSET] = (g_gpio_map[PULLUPDN_OFFSET] & ~3) | PUD_DOWN;
    else if (pud == PUD_UP)
        g_gpio_map[PULLUPDN_OFFSET] = (g_gpio_map[PULLUPDN_OFFSET] & ~3) | PUD_UP;
    else  // pud == PUD_OFF
        g_gpio_map[PULLUPDN_OFFSET] &= ~3;

    short_wait();
    g_gpio_map[clk_offset] = 1 << shift;
    short_wait();
    g_gpio_map[PULLUPDN_OFFSET] &= ~3;
    g_gpio_map[clk_offset] = 0;
}

static void set_direction(int gpio, int direction)
{
    int offset = FSEL_OFFSET + (gpio/10);
    int shift = (gpio%10)*3;
    
    assert(g_setup);
    
    if (direction == PIN_OUTPUT) {
        g_gpio_map[offset] = (g_gpio_map[offset] & ~(7<<shift)) | (1<<shift);
    } else {  // direction == PIN_INPUT
        g_gpio_map[offset] = (g_gpio_map[offset] & ~(7<<shift));
    }
    
    g_pin_direction[gpio] = direction;
}

int rpio_setup(int pinmap)
{
    if (pinmap != PINMAP_BOARD && pinmap != PINMAP_BCM) {
        return RPIO_INVALID_ARG;
    }
    
    if (g_revision < 0 && !g_setup) {
        g_revision = rpio_get_rpi_revision();
    }
    
    if (g_revision < 0) {
        return RPIO_UNKNOWN_BOARD_REVISION;
    } else if (g_revision == 0 && pinmap == PINMAP_BOARD) {
        return RPIO_INVALID_ARG; //Not applicable to the compute module.
    } else switch(g_revision) {
        case 1: g_pin_to_gpio = pin_to_gpio_rev1; break;
        case 2: g_pin_to_gpio = pin_to_gpio_rev2; break;
        default: g_pin_to_gpio = pin_to_gpio_rev3; break;
    }
    
    // Set the pin mapping mode.
    g_pinmap = pinmap;
    
    if (!g_setup) {
        memset(g_pin_direction, -1, sizeof(g_pin_direction));
    }
    
    if (g_gpio_map == NULL) {
        int mem_fd;

        if ((mem_fd = open("/dev/mem", O_RDWR|O_SYNC)) < 0) {
            return RPIO_FD_FAIL;
        }

        g_gpio_map = mmap(0, BLOCK_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED,
                          mem_fd, GPIO_BASE);
        close(mem_fd);
        
        if (g_gpio_map == MAP_FAILED) {
            return RPIO_MMAP_FAIL;
        }
    }
    
    g_setup = 1;
    return RPIO_OK;
}

void rpio_cleanup(void)
{
    if (g_gpio_map != NULL) {
        int i;
        for (i = 0; i < 54; i++) {
            if (g_pin_direction[i] != -1) {
                set_pullupdn(i, PUD_OFF);
                set_direction(i, PIN_INPUT);
                g_pin_direction[i] = -1;
            }
        }
        munmap((void*)g_gpio_map, BLOCK_SIZE);
        g_gpio_map = NULL;
    }
    
    g_setup = 0;
    g_pinmap = PINMAP_UNKNOWN;
    g_revision = -1;
    g_pin_to_gpio = NULL;
}

int rpio_set_pinmap(int pinmap)
{
    if (!g_setup) {
        return RPIO_NOT_INITIALISED;
    } else if (pinmap != PINMAP_BOARD && pinmap != PINMAP_BCM) {
        return RPIO_INVALID_ARG;
    } else if (g_revision == 0 && pinmap == PINMAP_BOARD) {
        return RPIO_INVALID_ARG;
    }
    
    g_pinmap = pinmap;
    return RPIO_OK;
}

int rpio_pin_function(int channel, int *retval)
{
    int gpio, value, ret;
    
    if (!g_setup) {
        if (retval) {
            *retval = RPIO_NOT_INITIALISED;
        }
        return 0;
    } else if ((ret = get_gpio_number(channel, &gpio)) != RPIO_OK) {
        if (retval) {
            *retval = ret;
        }
        return 0;
    } else {
        int offset = FSEL_OFFSET + (gpio/10);
        int shift = (gpio%10)*3;
        value = (g_gpio_map[offset] >> shift) & 7;
        
        if (retval) {
            *retval = ret;
        }
        
        return value; // 0=input, 1=output, 4=alt0
    }
}

int rpio_pin_setup(int channel, int direction, int pud)
{
    int ret, gpio;
    
    if (!g_setup) {
        return RPIO_NOT_INITIALISED;
    } else if ((ret = get_gpio_number(channel, &gpio)) != RPIO_OK) {
        return ret;
    }
    
    set_pullupdn(gpio, pud);
    set_direction(gpio, direction);
    
    return RPIO_OK;
}

int rpio_output(int channel, int value)
{
    int gpio, ret, offset, shift;
    
    if (!g_setup) {
        return RPIO_NOT_INITIALISED;
    } else if ((ret = get_gpio_number(channel, &gpio)) != RPIO_OK) {
        return ret;
    } else if (g_pin_direction[gpio] == -1) {
        return RPIO_INVALID_DIRECTION;
    }

    if (value) { // value == PIN_HIGH
        offset = SET_OFFSET + (gpio/32);
    } else {     // value == PIN_LOW
        offset = CLR_OFFSET + (gpio/32);
    }
    shift = (gpio%32);
    
    g_gpio_map[offset] = 1 << shift;
    return RPIO_OK;
}

int rpio_input(int channel, int *retval)
{
    int gpio, ret, offset, value, mask;
    
    if (!g_setup) {
        if (retval) {
            *retval = RPIO_NOT_INITIALISED;
        }
        return PIN_LOW;
    } else if ((ret = get_gpio_number(channel, &gpio)) != RPIO_OK) {
        if (retval) {
            *retval = ret;
        }
        return PIN_LOW;
    } else if (g_pin_direction[gpio] == -1) {
        if (retval) {
            *retval = RPIO_INVALID_DIRECTION;
        }
        return PIN_LOW;
    }

    offset = PINLEVEL_OFFSET + (gpio/32);
    mask = (1 << gpio%32);
    value = g_gpio_map[offset] & mask;

    if (retval) {
        *retval = RPIO_OK;
    }
    
    return value;
}