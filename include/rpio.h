/**
 * @file rpio.h
 * @brief librpio header. A simple library to control the GPIO pins on the RPi.
 * Based on `raspberry-gpio-python` (https://sf.net/p/raspberry-gpio-python)
 * @section LICENSE
 * The MIT License (MIT)
 * 
 * Copyright (c) 2015 Jeremy Tan
 * Copyright (c) 2013-2015 Ben Croston
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef _LIBRPIO_PUBLIC_H
#define _LIBRPIO_PUBLIC_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#define PINMAP_UNKNOWN  -1
#define PINMAP_BOARD    10
#define PINMAP_BCM      11

#define PIN_INPUT  1 // is really 0 for control register!
#define PIN_OUTPUT 0 // is really 1 for control register!

#define PIN_HIGH 1
#define PIN_LOW  0

#define PUD_OFF  0 //default
#define PUD_DOWN 1
#define PUD_UP   2

enum RPIO_STATUS {
    RPIO_OK,
    RPIO_INVALID_ARG,
    RPIO_UNKNOWN_BOARD_REVISION,
    RPIO_FD_FAIL,
    RPIO_MMAP_FAIL,
    RPIO_NOT_INITIALISED,
    RPIO_INVALID_CHANNEL,
    RPIO_INVALID_DIRECTION,
};

extern int rpio_setup(int setmap);
extern void rpio_cleanup(void);
extern int rpio_set_pinmap(int setmap);
extern int rpio_pin_function(int channel, int *retval);
extern int rpio_pin_setup(int channel, int direction, int pud);
extern int rpio_input(int channel, int *retval);
extern int rpio_output(int channel, int value);

extern int rpio_get_rpi_revision(void);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _LIBRPIO_PUBLIC_H