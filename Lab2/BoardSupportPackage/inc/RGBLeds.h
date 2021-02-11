#include <stdint.h>
#ifndef RGBLEDS_H_
#define RGBLEDS_H_

#define LS0 (0x06)
#define LS1 (0x07)
#define LS2 (0x08)
#define LS3 (0x09)


/* Enums for RGB LEDs */
typedef enum device
{
    BLUE = 0,
    GREEN = 1,
    RED = 2
};



/*
 * LP3943_ColorSet
 * This function will set the frequencies and PWM duty cycle
 * for each register of the specified unit.
 */
static void LP3943_ColorSet(uint32_t unit, uint32_t PWM_DATA);


/*
 * LP3943_LedModeSet
 * This function will set each of the LEDs to the desired operating
 * mode. The operating modes are on, off, PWM1 and PWM2.
 */
void LP3943_LedModeSet(uint32_t unit, uint16_t LED_DATA);


/*
 * Performs necessary initializations for RGB LEDs
 */
void init_RGBLEDS();

#endif
