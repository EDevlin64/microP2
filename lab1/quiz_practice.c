#include "msp.h"
#include <driverlib.h>
#include <uart.h>

extern void sort(uint8_t *data_in, int count, uint8_t *data_out);

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;        // stop watchdog timer

    uartInit();

    uint16_t result = 0;
    int count = 16;
    uint8_t in_table[16] = {1, 15, 14, 4, 10, 11, 8 ,5, 7, 6, 9, 12, 16, 2, 3, 13};
    uint8_t out_table[16];
    uint8_t * in_table_p = &in_table;
    uint8_t * out_table_p = &out_table;

    sort(in_table_p, count, out_table_p);

    char str[255];
    snprintf(str, 255, "The result is %d ", result);
    uartTransmitString(str);

}
