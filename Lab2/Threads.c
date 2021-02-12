#include <BSP.h>
#include "Threads.h"

/*
 * Reads Accelerometer Sensor Data (x-axis)
 */
void Task0(void)
{
    int16_t accelerometerX;
    while(1)
    {
        G8RTOS_WaitSemaphore(&sensor_I2C_semaphore);
        while(bmi160_read_accel_x(&accelerometerX));
        G8RTOS_SignalSemaphore(&sensor_I2C_semaphore);

        G8RTOS_WaitSemaphore(&led_I2C_semaphore);

        // write to the LEDs
        if (accelerometerX >= 14000)
        {
            LP3943_LedModeSet(RED, 0xFF00);
        }
        else if (accelerometerX < 14000 && accelerometerX >= 12000)
        {
            LP3943_LedModeSet(RED, 0x7F00);
        }
        else if (accelerometerX < 12000 && accelerometerX >= 10000)
        {
            LP3943_LedModeSet(RED, 0x3F00);
        }
        else if (accelerometerX < 10000 && accelerometerX >= 8000)
        {
            LP3943_LedModeSet(RED, 0x1F00);
        }
        else if (accelerometerX < 8000 && accelerometerX >= 6000)
        {
            LP3943_LedModeSet(RED, 0x0F00);
        }
        else if (accelerometerX < 6000 && accelerometerX >= 4000)
        {
            LP3943_LedModeSet(RED, 0x0700);
        }
        else if (accelerometerX < 4000 && accelerometerX >= 2000)
        {
            LP3943_LedModeSet(RED, 0x0300);
        }
        else if (accelerometerX < 2000 && accelerometerX >= 0)
        {
            LP3943_LedModeSet(RED, 0x0100);
        }
        else if (accelerometerX < 0 && accelerometerX >= -2000)
        {
            LP3943_LedModeSet(RED, 0x0080);
        }
        else if (accelerometerX < -2000 && accelerometerX >= -4000)
        {
            LP3943_LedModeSet(RED, 0x00C0);
        }
        else if (accelerometerX < -4000 && accelerometerX >= -6000)
        {
            LP3943_LedModeSet(RED, 0x00E0);
        }
        else if (accelerometerX < -6000 && accelerometerX >= -8000)
        {
            LP3943_LedModeSet(RED, 0x00F0);
        }
        else if (accelerometerX < -8000 && accelerometerX >= -10000)
        {
            LP3943_LedModeSet(RED, 0x00F8);
        }
        else if (accelerometerX < -10000 && accelerometerX >= -12000)
        {
            LP3943_LedModeSet(RED, 0x00FC);
        }
        else if (accelerometerX < -12000 && accelerometerX >= -14000)
        {
            LP3943_LedModeSet(RED, 0x00FE);
        }
        else if (accelerometerX < -14000)
        {
            LP3943_LedModeSet(RED, 0x00FF);
        }
        else
        {
            LP3943_LedModeSet(RED, 0x0000);
        }

        G8RTOS_SignalSemaphore(&led_I2C_semaphore);

        /* added bc of naive semaphores - works without them though*/
        int i;
        for (i = 0 ; i < 48000 ; i++);
    }

}

/*
 * Reads Light Sensor Data
 */
void Task1(void)
{
    int16_t lightData;

    while(1)
    {
        G8RTOS_WaitSemaphore(&sensor_I2C_semaphore);
        while(!sensorOpt3001Read(&lightData));
        G8RTOS_SignalSemaphore(&sensor_I2C_semaphore);

        G8RTOS_WaitSemaphore(&led_I2C_semaphore);
        /* write to the LEDs */
        if (lightData >= 49000)
        {
            LP3943_LedModeSet(GREEN, 0xFFFF);
        }
        else if (lightData < 49000 && lightData >= 45500)
        {
            LP3943_LedModeSet(GREEN, 0xFFFE);
        }
        else if (lightData < 45500 && lightData >= 42000)
        {
            LP3943_LedModeSet(GREEN, 0xFFFC);
        }
        else if (lightData < 42000 && lightData >= 38500)
        {
            LP3943_LedModeSet(GREEN, 0xFFF8);
        }
        else if (lightData < 38500 && lightData >= 35500)
        {
            LP3943_LedModeSet(GREEN, 0xFFF0);
        }
        else if (lightData < 35500 && lightData >= 31500)
        {
            LP3943_LedModeSet(GREEN, 0xFFE0);
        }
        else if (lightData < 31500 && lightData >= 28000)
        {
            LP3943_LedModeSet(GREEN, 0xFFC0);
        }
        else if (lightData < 28000 && lightData >= 26000)
        {
            LP3943_LedModeSet(GREEN, 0xFF80);
        }
        else if (lightData < 26000 && lightData >= 24500)
        {
            LP3943_LedModeSet(GREEN, 0xFF00);
        }
        else if (lightData < 24500 && lightData >= 21000)
        {
            LP3943_LedModeSet(GREEN, 0xFE00);
        }
        else if (lightData < 21000 && lightData >= 17500)
        {
            LP3943_LedModeSet(GREEN, 0xFC00);
        }
        else if (lightData < 17500 && lightData >= 14000)
        {
            LP3943_LedModeSet(GREEN, 0xF800);
        }
        else if (lightData < 14000 && lightData >= 10500)
        {
            LP3943_LedModeSet(GREEN, 0xF000);
        }
        else if (lightData < 10500 && lightData >= 7000)
        {
            LP3943_LedModeSet(GREEN, 0xE000);
        }
        else if (lightData < 7000 && lightData >= 3500)
        {
            LP3943_LedModeSet(GREEN, 0xC000);
        }
        else if (lightData < 3500)
        {
            LP3943_LedModeSet(GREEN, 0x8000);
        }
        else
        {
            LP3943_LedModeSet(GREEN, 0x0000);
        }

        G8RTOS_SignalSemaphore(&led_I2C_semaphore);

        /* added bc of naive semaphores - works without them though*/
        int i;
        for (i = 0 ; i < 48000 ; i++);

    }

}

/*
 * Reads Gyro Sensor Data (z-axis)
 */
void Task2(void)
{
    int16_t gyroZ;
    while(1)
    {
        G8RTOS_WaitSemaphore(&sensor_I2C_semaphore);
        while(bmi160_read_gyro_z(&gyroZ));
        G8RTOS_SignalSemaphore(&sensor_I2C_semaphore);

        G8RTOS_WaitSemaphore(&led_I2C_semaphore);

        /* write to the LEDs */
        if (gyroZ >= 7000)
        {
            LP3943_LedModeSet(BLUE, 0xFF00);
        }
        else if (gyroZ < 7000 && gyroZ >= 6000)
        {
            LP3943_LedModeSet(BLUE, 0x7F00);
        }
        else if (gyroZ < 6000 && gyroZ >= 5000)
        {
            LP3943_LedModeSet(BLUE, 0x3F00);
        }
        else if (gyroZ < 5000 && gyroZ >= 4000)
        {
            LP3943_LedModeSet(BLUE, 0x1F00);
        }
        else if (gyroZ < 4000 && gyroZ >= 3000)
        {
            LP3943_LedModeSet(BLUE, 0x0F00);
        }
        else if (gyroZ < 3000 && gyroZ >= 2000)
        {
            LP3943_LedModeSet(BLUE, 0x0700);
        }
        else if (gyroZ < 2000 && gyroZ >= 1000)
        {
            LP3943_LedModeSet(BLUE, 0x0300);
        }
        else if (gyroZ < 1000 && gyroZ >= 0)
        {
            LP3943_LedModeSet(BLUE, 0x0100);
        }
        else if (gyroZ < 0 && gyroZ >= -1000)
        {
            LP3943_LedModeSet(BLUE, 0x0080);
        }
        else if (gyroZ < -1000 && gyroZ >= -2000)
        {
            LP3943_LedModeSet(BLUE, 0x00C0);
        }
        else if (gyroZ < -2000 && gyroZ >= -3000)
        {
            LP3943_LedModeSet(BLUE, 0x00E0);
        }
        else if (gyroZ < -3000 && gyroZ >= -4000)
        {
            LP3943_LedModeSet(BLUE, 0x00F0);
        }
        else if (gyroZ < -4000 && gyroZ >= -5000)
        {
            LP3943_LedModeSet(BLUE, 0x00F8);
        }
        else if (gyroZ < -5000 && gyroZ >= -6000)
        {
            LP3943_LedModeSet(BLUE, 0x00FC);
        }
        else if (gyroZ < -6000 && gyroZ >= -7000)
        {
            LP3943_LedModeSet(BLUE, 0x00FE);
        }
        else if (gyroZ < -7000)
        {
            LP3943_LedModeSet(BLUE, 0x00FF);
        }
        else
        {
            LP3943_LedModeSet(BLUE, 0x0000);
        }

        G8RTOS_SignalSemaphore(&led_I2C_semaphore);

        /* added bc of naive semaphores - works without them though*/
        int i;
        for (i = 0 ; i < 48000 ; i++);
    }
}
