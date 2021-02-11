#include "msp.h"
#include <driverlib.h>
#include <uart.h>

struct date
    {
        uint8_t hour;   // = 4;
        uint8_t minute; // = 5;
        uint8_t day;    // = 28;
        uint8_t month;  // = 1;
    }; //current, newYear;

extern void date(uint8_t *current, uint8_t *newYear, uint8_t *result);

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;        // stop watchdog timer

    uartInit();

    struct date current;
    struct date newYear;
    struct date result;

    current.hour = 19;
    current.minute = 54;
    current.day = 2;
    current.month = 11;
    uint8_t * current_p = &current;

    newYear.hour = 23;
    newYear.minute = 59;
    newYear.day = 30;
    newYear.month = 12;
    uint8_t * new_p = &newYear;

    uint8_t * result_p = &result;

    date(current_p, new_p, result_p);

    char str[255];
    snprintf(str, 255, "There are %d months, %d days, %d hours, and %d minutes until New Years \r \n", result.month, result.day, result.hour, result.minute);
    uartTransmitString(str);

}
