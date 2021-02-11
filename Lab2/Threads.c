#include <BSP.h>
#include "Threads.h"

/*
 * Reads Accelerometer Sensor Data (x-axis)
 */
void Task0(void)
{
    int16_t acceleromterX;
    G8RTOS_WaitSemaphore(s)
    while(bmi160_read_accel_x(acceleromterX));
    while(1)
    {
        counter0++;
    }
}

/*
 * Reads Light Sensor Data
 */
void Task1(void)
{
    int counter1 = 0;
    while(1)
    {
        counter1++;
    }
}

/*
 * Reads Gyro Sensor Data (z-axis)
 */
void Task2(void)
{
    int counter2 = 0;
    while(1)
    {
        counter2++;
    }
}




/*
void Task0(void)
{
    int counter0 = 0;
    while(1)
    {
        counter0++;
    }
}


void Task1(void)
{
    int counter1 = 0;
    while(1)
    {
        counter1++;
    }
}

void Task2(void)
{
    int counter2 = 0;
    while(1)
    {
        counter2++;
    }
}
 */
