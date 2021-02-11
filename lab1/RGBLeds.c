#include "msp.h"
#include <driverlib.h>
#include <RGBLeds.h>

/*
 * Performs the necessary initializations for RGB LEDs
 */
void LP3943_LedModeSet(uint32_t unit, uint16_t LED_DATA)
{
    /*
     * LP3943_LedModeSet
     * This function will set each of the LEDs to the desired operating
     * mode. The operating modes are on, off
     *
     * The units that can be written to are:
     *   UNIT  |  0  | Red
     *   UNIT  |  1  | Blue
     *   UNIT  |  2  | Green
     *
     *   The registers to be written are:
     *   ---------
     *   |  LS0  | LED0-3 Selector
     *   |  LS1  | LED4-7 Selector
     *   |  LS2  | LED8-11 Selector
     *   |  LS3  | LED12-15 Selector
     *   ---------
     *
     *   See 24.3.4.2.1 in MSP432 Tech Manual for Master Transmitter Mode
     */
    uint8_t LS0_DATA;
    uint8_t LS1_DATA;
    uint8_t LS2_DATA;
    uint8_t LS3_DATA;

    LS0_DATA = (LED_DATA & 0x000F) >> 0;
    LS1_DATA = (LED_DATA & 0x00F0) >> 4;
    LS2_DATA = (LED_DATA & 0x0F00) >> 8;
    LS3_DATA = (LED_DATA & 0xF000) >> 12;

    // determines which color LP3943 module to address
    uint8_t SLV_ADDR = 0x60 | unit;

    // address of the first LED module
    uint16_t LS0_ADDR = 0x06;

    UCB2I2CSA = SLV_ADDR;               // set slave address to correct color module (LSB = 0 to indicate WRITE)
    UCB2CTLW0 |= UCTXSTT;               // generate start condition - sends color module address to buffer??
    while(UCB2CTLW0 & UCTXSTT);         //
    while((UCB2IFG & UCTXIFG0) == 0);   // poll for buffer flag $$$ might be unnecessary

    UCB2TXBUF = LS0_ADDR | (1<<4);      // address the LS0 module and initialize the auto-increment function
    while((UCB2IFG & UCTXIFG0) == 0);   // poll for buffer flag being set

    UCB2TXBUF = (LS0_DATA & 1) << 0 | (LS0_DATA & 2) << 1 | (LS0_DATA & 4) << 2 | (LS0_DATA & 8) << 3;
    while((UCB2IFG & UCTXIFG0) == 0);
    UCB2TXBUF = (LS1_DATA & 1) << 0 | (LS1_DATA & 2) << 1 | (LS1_DATA & 4) << 2 | (LS1_DATA & 8) << 3;
    while((UCB2IFG & UCTXIFG0) == 0);
    UCB2TXBUF = (LS2_DATA & 1) << 0 | (LS2_DATA & 2) << 1 | (LS2_DATA & 4) << 2 | (LS2_DATA & 8) << 3;
    while((UCB2IFG & UCTXIFG0) == 0);
    UCB2TXBUF = (LS3_DATA & 1) << 0 | (LS3_DATA & 2) << 1 | (LS3_DATA & 4) << 2 | (LS3_DATA & 8) << 3;
    while((UCB2IFG & UCTXIFG0) == 0);

    UCB2CTLW0 |= UCTXSTP;               // generate stop condition

}

void init_RGBLEDS()
{
    uint16_t UNIT_OFF = 0x0000;

    // Software reset enable mode
    UCB2CTLW0 = UCSWRST;

    // Initialize the software reset
    // Set as master, I2C mode, Clock sync, SMCLK source, Transmitter
    UCB2CTLW0 |= UCMST | UCMODE_3 | UCSYNC | UCSSEL_2 | UCTR;             // UCSYNC? UCTXACK? ACK? NACK??


    // Set the Fclk as 400 kHz
    // Presumes that the SMCLK is selected as source and Fsmclk is 12 MHz
    UCB2BRW = 30;

    // In conjunction with the next line, this sets the pins as I2C mode
    // (Table found p160 of SLAS826E)
    // Set P3.6 as UCB2_SDA and 3.7 as UCB2_SLA
    P3SEL0 |= (1<<7) | (1<<6);      // set bit6 and bit7 to use default function (I2C SDA)
    P3SEL1 &= ~((1<<7) | (1<<6));                 // clear bit6 and bit7 to use default function (I2C SCL)        $$$

    // Bitwise anding all bits except UCSWRST - turns off software reset mode
    UCB2CTLW0 &= ~UCSWRST;

    LP3943_LedModeSet(RED, UNIT_OFF);
    LP3943_LedModeSet(GREEN, UNIT_OFF);
    LP3943_LedModeSet(BLUE, UNIT_OFF);

}


