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
*/

#include <G8RTOS/G8RTOS.h>
#include "msp.h"
#include "BSP.h"
#include "Threads.h"
#include "ThreadsQuiz.h"
#include "gpio.h"

void main(void)
{
    /* stop watchdog timer */
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;

    /* initialize G8RTOS, disable interrupts */
    G8RTOS_Init();

    //Set P2.0-2 as outputs
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2);

	/* init semaphores*/
    G8RTOS_InitSemaphore(&sensorI2cSemaphore, 1);
    G8RTOS_InitSemaphore(&ledI2cSemaphore, 1);

    //Initialize the fifos
    G8RTOS_InitFIFO(FIFO_INDEX_JOYSTICK);
    G8RTOS_InitFIFO(FIFO_INDEX_TEMP);
    G8RTOS_InitFIFO(FIFO_INDEX_LIGHT);

    /* add initial threads*/
    G8RTOS_AddThread(&task0);
    G8RTOS_AddThread(&task1);
    G8RTOS_AddThread(&task2);
    G8RTOS_AddThread(&task3);
    G8RTOS_AddThread(&task4);
    G8RTOS_AddThread(&idleThread);
    G8RTOS_AddPeriodicThread(&ptask0, 100);
    G8RTOS_AddPeriodicThread(&ptask1, 1000);


    /* QUIZ */
/*    G8RTOS_AddThread(&player_position);
    G8RTOS_AddPeriodicThread(&score_thread, 1000);
    G8RTOS_AddPeriodicThread(&player_joystick, 100);
    G8RTOS_AddPeriodicThread(&coins, 5000);
*/

    /* launch the OS */
    G8RTOS_Launch();

    /* never executes */
    while(1);
}
