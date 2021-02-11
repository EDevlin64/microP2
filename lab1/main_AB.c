/*
Project:        Lab1 Part A and B
Name:           Edward Devlin
Class :         MicroP 2
PI Name:        Haoqi Shan

Description:    Computes the Fletcher 16 algorithm sum for an array of data called
                magic. Uses a provided C algorithm and an Assembly to output the
                result to a serial terminal. Also sends the result to the LED
                daughter board
*/

#include "msp.h"
#include <driverlib.h>
#include <RGBLeds.h>
#include <uart.h>

uint16_t Modulus255(uint16_t data) {
    return data % 255;
}


uint16_t Fletcher16( uint8_t * data, int count)
{
    uint16_t sum1 = 0;
    uint16_t sum2 = 0;
    int index;

    for (index = 0; index < count; ++index)
    {
        sum1 = (sum1 + data[index]) % 255;
        sum2 = (sum2 +sum1) % 255;
    }

    return (sum2 << 8) | sum1;
}

// the assembly fletcher algorithm
extern uint16_t fletcher16(uint8_t *data, int count);

/*
 * MAIN PROGRAM
 */

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;        // stop watchdog timer

    uartInit();

    // create magic box and pointer
    uint16_t result_c = 0;
    uint16_t result_asm = 0;
    int count = 16;
    uint8_t magic[16] = {1, 15, 14, 4, 10, 11, 8 ,5, 7, 6, 9, 12, 16, 2, 3, 13};
    uint8_t * magic_p = &magic;

    // run the Fletcher algorithm in C and Assembly
    result_c = Fletcher16(magic_p, count);
    result_asm = fletcher16(magic_p, count);

    // send string out via UART
    char str[255];
    snprintf(str, 255, "The C checksum = %d and the ASM checksum = %d \r \n", result_c, result_asm);
    uartTransmitString(str);

    // testing I2C functionality
    init_RGBLEDS();

    // Fletcher algorithm on LEDs
    uint16_t UNIT_OFF = 0x0000;
    while(1)
    {
        LP3943_LedModeSet(BLUE, result_c);
        LP3943_LedModeSet(GREEN, result_c);
        LP3943_LedModeSet(RED, result_c);
        delay(1);        // $$$ change later to use systick
        LP3943_LedModeSet(BLUE, UNIT_OFF);
        LP3943_LedModeSet(GREEN, UNIT_OFF);
        LP3943_LedModeSet(RED, UNIT_OFF);
        delay(1);
    }

    // never reached - just a fancy pattern
    while(1)
    {
        pattern1();
        pattern2();
    }
}


/*
 * these functions below are used for the fancy pattern, not the fletcher algorithm
 */
int j;
void delay(int delayFactor)
{
    for (j = 0 ; j < delayFactor*50000 ; j++){}
}
int k;
uint16_t LED;
void pattern1(void)
{
    LED = 0x0001;

    for (k = 0 ; k < 16 ; k++)
    {
       LP3943_LedModeSet(RED, LED);
       delay(1);
       LED <<= 1;
    }
    LED = 0x000F;
    LP3943_LedModeSet(RED, LED);
    for (k = 0 ; k < 4 ; k++)
    {
       LP3943_LedModeSet(RED, LED);
       delay(2);
       LED <<= 4;
    }
    LP3943_LedModeSet(RED, 0xFF00);
    delay(2);
    LP3943_LedModeSet(RED, 0xFFF0);
    delay(2);
    LP3943_LedModeSet(RED, 0xFFFF);
    delay(2);
    LED = 0xFFFF;
    for (k = 0 ; k < 16 ; k++)
    {
       LP3943_LedModeSet(RED, LED);
       delay(1);
       LED >>= 1;
    }
    LP3943_LedModeSet(GREEN, 0x0000);
    LP3943_LedModeSet(BLUE, 0x0000);
    LP3943_LedModeSet(RED, 0x0000);
}

void pattern2()
{

    LED = 0x0000;
    LP3943_LedModeSet(RED, LED);
    LED = 0xAAAA;
    for (k = 0 ; k < 16 ; k++)
    {
        LP3943_LedModeSet(BLUE, LED);
        LP3943_LedModeSet(RED, LED);
        delay(1);
        LED <<= 1;
    }
    LED = 0xAAAA;
    for (k = 0 ; k < 16 ; k++)
    {
        LP3943_LedModeSet(GREEN, LED);
        LP3943_LedModeSet(BLUE, LED);
        delay(1);
        LED >>= 1;
    }
    for (k = 0 ; k < 32 ; k++)
    {
        LP3943_LedModeSet(GREEN, 0x1F87); //0b0001 1111 1000 0111);
        LP3943_LedModeSet(BLUE, 0x01FF);  //0b0000 0001 1111 1111);
        LP3943_LedModeSet(RED, 0xF81F);   //0b1111 1000 0001 1111);
        // red yellow green blue cyan purple white
        delay(4);
        LP3943_LedModeSet(GREEN, 0x0000);
        LP3943_LedModeSet(BLUE, 0x0000);
        LP3943_LedModeSet(RED, 0x0000);
        delay(4);
    }
    LP3943_LedModeSet(GREEN, 0x0000);
    LP3943_LedModeSet(BLUE, 0x0000);
    LP3943_LedModeSet(RED, 0x0000);
}
