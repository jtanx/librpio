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
    
    printf("Setting pin 8: %d\n", rpio_pin_setup(8, PIN_OUTPUT, PUD_DOWN));
    printf("Pin 8 function: %d\n", rpio_pin_function(8, &ret));
    
    rpio_cleanup();
    return 0;
}