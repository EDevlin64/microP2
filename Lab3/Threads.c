#include <BSP.h>
#include "msp.h"
#include "G8RTOS.h"
#include "Threads.h"
#include <stdbool.h>

#define RMS_NVALUE 16
#define RMS_COUNT_MASK (RMS_NVALUE - 1)

semaphore_t ledI2cSemaphore;
semaphore_t sensorI2cSemaphore;

static bool rmsBelowThresh = false;
volatile int16_t joystickX = 0;
static int32_t currentTemperature = 0;
static int32_t decayedJoystickAvg = 0;

/*
 * First Period Task
 *      - Reads the X coordinate from Joystick
 *      - Write data to Joystick FIFO
 *      - Toggle GPIO pin
 *      - Period: 100 ms
 */
void ptask0(void)
{
    int16_t x_coord, y_coord;
    GetJoystickCoordinates(&x_coord, &y_coord);
    // use global variable instead of FIFO
    joystickX = x_coord;

    //G8RTOS_WriteFIFO(FIFO_INDEX_JOYSTICK, x_coord);
    BITBAND_PERI(P2->OUT, 0) = !BITBAND_PERI(P2->OUT, 0);
}

/*
 * Second Period Task
 *      - If global light variable is true,
 *      - Print temp in degrees F via UART and print decayed avg value of joystick x-coordinate
 *      - Otherwise do nothing
 *      - Period: 1 s
 */
void ptask1(void)
{
    if (rmsBelowThresh)
    {
        BackChannelPrintIntVariable("temperature", currentTemperature);
        BackChannelPrintIntVariable("decayedJoystickAvg", decayedJoystickAvg);
    }
}

/*
 * First Task
 *      - Read BME280 temp sensor
 *      - Send data to temp FIFO
 *      - Toggle an available GPIO pin
 *      - Sleep for 500 ms
 */
void task0(void)
{
    int32_t temperature;
    while(1)
    {
        // read uncompensated temp and then covert to compensated temp (C)
        G8RTOS_WaitSemaphore(&sensorI2cSemaphore);
        bme280_read_uncomp_temperature(&temperature);
        temperature = bme280_compensate_temperature_int32(temperature);
        G8RTOS_SignalSemaphore(&sensorI2cSemaphore);

        // Convert the temperature to Fahrenheit, send the data to the temp FIFO and toggle GPIO 2.1 (LED on msp)
        temperature = (temperature * 9) / 500 + 32;
        G8RTOS_WriteFIFO(FIFO_INDEX_TEMP, temperature);
        BITBAND_PERI(P2->OUT, 1) = !BITBAND_PERI(P2->OUT, 1);

        G8RTOS_Sleep(500);
    }
}

/*
 * Second Task
 *      - Read light sensor
 *      - Send data to light FIFO
 *      - Toggle an available pin
 *      - Sleep for 200 ms
 */
void task1(void)
{
    uint16_t lightData;
    while(1)
    {
        //Read light sensor, send to light FIFO then toggle GPIO 2.2 (LED on msp)
        G8RTOS_WaitSemaphore(&sensorI2cSemaphore);
        sensorOpt3001Read(&lightData);
        G8RTOS_SignalSemaphore(&sensorI2cSemaphore);
        G8RTOS_WriteFIFO(FIFO_INDEX_LIGHT, lightData);
        BITBAND_PERI(P2->OUT, 2) = !BITBAND_PERI(P2->OUT, 2);

        G8RTOS_Sleep(200);
    }
}

/*
 * Third Task
 *      - Read light FIFO
 *      - Calculate RMS task
 *      - Calculate square root
 *      - if RMS < 5000, set global variable true, otherwise keep it false
 */
void task2(void)
{
    uint16_t lightData[RMS_NVALUE] = {0};
    uint32_t runningSum = 0;
    uint32_t counter = 0;
    uint32_t rmsValue;
    while(1)
    {
        //Subtract old data from running sum
        runningSum -= (lightData[counter & RMS_COUNT_MASK] * lightData[counter & RMS_COUNT_MASK]) / RMS_NVALUE;
        //Get new data from light fifo
        lightData[counter & RMS_COUNT_MASK] = G8RTOS_ReadFIFO(FIFO_INDEX_LIGHT);
        //Add new data to running sum. Data is squared and divided by RMS_NVALUE for rms calc
        runningSum += (lightData[counter & RMS_COUNT_MASK] * lightData[counter & RMS_COUNT_MASK]) / RMS_NVALUE;
        //Increase counter
        counter++;
        //Take square root of running sum to find rms value
        rmsValue = squareRoot(runningSum);
        //Set/clear the global rms bool according to the threshold
        if(rmsValue < 5000) rmsBelowThresh = false;
        else rmsBelowThresh = true;
    }
}

/*
 * Fourth Task
 *      - Read temp FIFO
 *      - Output data to LEDs
 */
void task3(void)
{
    uint8_t ledBitMask = 0, oldLedBitMask = 0;
    while(1)
    {
        //Read temperature FIFO
        currentTemperature = G8RTOS_ReadFIFO(FIFO_INDEX_TEMP);

        //Set led bit mask corresponding to the temperature data
        if (currentTemperature > 84) ledBitMask = 0x00;
        else if (currentTemperature > 81) ledBitMask = 0x80;
        else if (currentTemperature > 78) ledBitMask = 0xC0;
        else if (currentTemperature > 75) ledBitMask = 0xE0;
        else if (currentTemperature > 72) ledBitMask = 0xF0;
        else if (currentTemperature > 69) ledBitMask = 0xF8;
        else if (currentTemperature > 66) ledBitMask = 0xFC;
        else if (currentTemperature > 63) ledBitMask = 0xFE;
        else ledBitMask = 0xFF;

        //Output data to LEDs
        if(ledBitMask != oldLedBitMask)
        {
            G8RTOS_WaitSemaphore(&ledI2cSemaphore);
            LP3943_LedModeSet(BLUE, ledBitMask & 0x00FF);
            LP3943_LedModeSet(RED, (~ledBitMask) & 0x00FF);
            G8RTOS_SignalSemaphore(&ledI2cSemaphore);
            oldLedBitMask = ledBitMask;
        }
    }
}

/*
 * Fifth Task
 *      - Read Joystick FIFO
 *      - Calculate decayed avg for x_coord
 *      - Output data to LEDs
 */
void task4(void)
{
    int16_t x_coord;
    uint8_t ledBitMask = 0, oldLedBitMask = 0;
    while(1)
    {
        //Read joystick FIFO
        x_coord = joystickX;
        //x_coord = G8RTOS_ReadFIFO(FIFO_INDEX_JOYSTICK);

        //Calculate decayed average for X-Coordinate
        //decayedJoystickAvg = (decayedJoystickAvg + x_coord) >> 1;
        decayedJoystickAvg = (decayedJoystickAvg + joystickX) >> 1;

        //Set led bit mask corresponding to the joystick data
        if(x_coord > 6000) ledBitMask = 0x0F;
        else if(x_coord > 4000) ledBitMask = 0x0E;
        else if(x_coord > 2000) ledBitMask = 0x0C;
        else if(x_coord > 500) ledBitMask = 0x08;
        else if(x_coord > -500) ledBitMask = 0x00;
        else if(x_coord > -2000) ledBitMask = 0x10;
        else if(x_coord > -4000) ledBitMask = 0x30;
        else if(x_coord > -6000) ledBitMask = 0x70;
        else ledBitMask = 0xF0;

        //Output data to LEDs only if ledBitMask has changed
        if(ledBitMask != oldLedBitMask)
        {
            G8RTOS_WaitSemaphore(&ledI2cSemaphore);
            LP3943_LedModeSet(GREEN, (ledBitMask << 8) & 0xFF00);
            G8RTOS_SignalSemaphore(&ledI2cSemaphore);
            oldLedBitMask = ledBitMask;
        }
    }
}

/*
 * Idle Task
 *      - Idle Thread
 */
void idleThread(void)
{
    while(1);
}

/*
 * Calculate the square root of a number using Newton's method
 * Param "value": The number to take the square root of
 * Returns the square root
 */
uint32_t squareRoot(uint32_t value)
{
    uint32_t xk = value;
    uint32_t xk1 = (xk + (value / xk)) / 2;
    while(abs(xk1 - xk) >= 1)
    {
        xk = xk1;
        xk1 = (xk + (value / xk)) / 2;
    }
    return xk1;
}


