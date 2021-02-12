/*
Lab2
Name:           Edward Devlin
Class:          uP2
Description:    Initializes and launches a real time operating system. Implements
                a round robin scheduler for the threads. Three active threads that
                read sensor data and display values to the daughter boards

Personal Note:  This implementation uses naive spinlock semaphores - the
                occasional random for loops are used to give the system time to
                unlock certain semaphores

#include <G8RTOS/G8RTOS.h>
#include "msp.h"
#include "BSP.h"
#include "Threads.h"

/**
 * main.c
 */
int main(void)
{
    G8RTOS_InitSemaphore(&sensor_I2C_semaphore, 1);
    G8RTOS_InitSemaphore(&led_I2C_semaphore, 1);

    /* initialize G8RTOS, disable interrupts */
	G8RTOS_Init();

	G8RTOS_AddThread(&Task0);
	G8RTOS_AddThread(&Task1);
	G8RTOS_AddThread(&Task2);

	G8RTOS_Launch();

	/* never executes */
	while(1);
}
