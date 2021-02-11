/*
Lab1 PartC
Name: Edward Devlin
Class : MicroP 2
PI Name: Haoqi Shan

Description:    Utilizes a button interrupt to change between the animations
                displayed on the LED daughter board (B0). Utilizes the SysTick
                interrupt to delay each frame of the animation
*/

#include "msp.h"
#include <driverlib.h>
#include <RGBLeds.h>

uint8_t LedFlag = 0;
void PORT4_IRQHandler(void)
{
    P4->IFG &= ~BIT4;   // clear int flag

    // invert flag to change between LED patterna
    LedFlag = ~LedFlag;
}

int index = 0;
int p2 = 0;
uint16_t pat1[16] = {0x0003, 0x000F, 0x003F, 0x00FF, 0x03FF, 0x0FFF, 0x3FFF , 0xFFFF, 0xAAAA, 0x5555, 0xAAAA, 0x5555, 0xAAAA, 0x5555, 0xAAAA, 0x5555};
uint16_t * pat1_p = &pat1;
void SysTick_Handler(void)
{

    if (index > 15)
    {
        index = 0;
    }
    if (LedFlag)
    {
        // clear the LEDs
        LP3943_LedModeSet(BLUE, 0x0000);
        LP3943_LedModeSet(GREEN, 0x0000);

        // purple pattern from table above
        LP3943_LedModeSet(RED, pat1_p[index]);
        LP3943_LedModeSet(BLUE, pat1_p[index]);
        index += 1;
    }
    else
    {
        // clear previous pattern
        LP3943_LedModeSet(RED, 0x0000);
        LP3943_LedModeSet(BLUE, 0x0000);
        // blue pattern
        p2 = ~p2;
        if (p2)
        {
            LP3943_LedModeSet(BLUE, 0xF0F0);
            LP3943_LedModeSet(GREEN, 0xF0F0);
        }
        else {
            LP3943_LedModeSet(BLUE, 0x0F0F);
            LP3943_LedModeSet(GREEN, 0x0F0F);
        }
    }
}

void init_IO(void)
{
    P4->DIR &= ~BIT4;                               // set P4.4 as input/output
    P4->IFG &= ~BIT4;                               // P4.4 IFG cleared
    P4->IE  |= BIT4;                                // Enable interrupt on P4.4
    P4->IES |= BIT4;                                // high-to-low transition
    P4->REN |= BIT4;                                // pull-up resistor
    P4->OUT |= BIT4;                                // set ers to pull-up
}

/*
 ******************** MAIN PROGRAM ********************
 */
void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    init_RGBLEDS();
    init_IO();
    SysTick_Config(3000000/4);                      // every 250 ms
    SysTick_enableInterrupt();

    NVIC_EnableIRQ(PORT4_IRQn);

    while(1)
    {
        PCM_gotoLPM0();
    }

}

