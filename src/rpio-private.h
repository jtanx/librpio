/**
 * @file pigpio-private.h
 * @brief Contains internal definitions.
 * @author Jeremy Tan
 * @author Ben Croston
 * @copyright MIT License. See LICENSE for more information.
 */

#ifndef _LIBPIGPIO_PRIVATE_H
#define _LIBPIGPIO_PRIVATE_H

#define BCM2708_PERI_BASE   0x20000000
#define GPIO_BASE           (BCM2708_PERI_BASE + 0x200000)
#define FSEL_OFFSET         0   // 0x0000
#define SET_OFFSET          7   // 0x001c / 4
#define CLR_OFFSET          10  // 0x0028 / 4
#define PINLEVEL_OFFSET     13  // 0x0034 / 4
#define EVENT_DETECT_OFFSET 16  // 0x0040 / 4
#define RISING_ED_OFFSET    19  // 0x004c / 4
#define FALLING_ED_OFFSET   22  // 0x0058 / 4
#define HIGH_DETECT_OFFSET  25  // 0x0064 / 4
#define LOW_DETECT_OFFSET   28  // 0x0070 / 4
#define PULLUPDN_OFFSET     37  // 0x0094 / 4
#define PULLUPDNCLK_OFFSET  38  // 0x0098 / 4

#define BLOCK_SIZE (4*1024)

#endif // _LIBPIGPIO_PRIVATE_H