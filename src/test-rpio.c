#include <stdio.h>
#include <stdlib.h>
#include "rpio.h"

int main() {
    int ret;
    
    printf("RPIO setup: %d\n", (ret = rpio_setup(PINMAP_BOARD)));
    if (ret != RPIO_OK) {
        return 1;
    }
    
    printf("Default pin modes:\n");
    for (int i = 1; i < 41; i++) {
        printf("Pin %d: %d\n", i, rpio_pin_function(i, &ret));
    }
    
    printf("PAUSED");
    getchar();
    printf("Setting pin 40: %d\n", rpio_pin_setup(40, PIN_OUTPUT, PUD_DOWN));
    printf("Pin 40 function: %d\n", rpio_pin_function(40, &ret));
    printf("Setting pin 40 to HIGH: %d\n", rpio_output(40, PIN_HIGH));
    printf("PAUSED\n");
    getchar();
    printf("Setting pin 40 to LOW: %d\n", rpio_output(40, PIN_LOW));
    printf("PAUSED\n");
    getchar();
    printf("Setting pin 40 to HIGH: %d\n", rpio_output(40, PIN_HIGH));
    printf("PAUSED\n");
    getchar();
    printf("CLEANUP\n");
    
    rpio_cleanup();
    return 0;
}