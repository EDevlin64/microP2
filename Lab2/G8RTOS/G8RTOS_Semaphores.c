/*
 * G8RTOS_Semaphores.c
 */

/*********************************************** Dependencies and Externs *************************************************************/

#include <G8RTOS/G8RTOS_CriticalSection.h>
#include <G8RTOS/G8RTOS_Semaphores.h>
#include <stdint.h>
#include "msp.h"

/*********************************************** Dependencies and Externs *************************************************************/


/*********************************************** Public Functions *********************************************************************/

/*
 * Initializes a semaphore to a given value
 * Param "s": Pointer to semaphore
 * Param "value": Value to initialize semaphore to
 * THIS IS A CRITICAL SECTION
 */
void G8RTOS_InitSemaphore(semaphore_t *s, int32_t value)
{
    // disable interrupts
    uint32_t i_state;
    i_state = StartCriticalSection();

    // initialize the sempaphore to an on or off state
	*s = value;

	// reenable interrupts
	EndCriticalSection(i_state);

}

/*
 * Waits for a semaphore to be available (value greater than 0)
 * 	- Decrements semaphore when available
 * 	- Spinlocks to wait for semaphore
 * Param "s": Pointer to semaphore to wait on
 * THIS IS A CRITICAL SECTION
 */
void G8RTOS_WaitSemaphore(semaphore_t *s)
{
    uint32_t i_state = StartCriticalSection();  // disable interrupts
    int i;

    while (*s == 0)
    {
        for (i = 0 ; i < 100 ; i++);
        EndCriticalSection(i_state);            // enable interrupts
        for (i = 0 ; i < 100 ; i++);            // wait it out
        i_state = StartCriticalSection();       // disable interrupts
    }

    (*s)--;
    EndCriticalSection(i_state);

}

/*
 * Signals the completion of the usage of a semaphore
 * 	- Increments the semaphore value by 1
 * Param "s": Pointer to semaphore to be signalled
 * THIS IS A CRITICAL SECTION
 */
void G8RTOS_SignalSemaphore(semaphore_t *s)
{
	uint32_t i_state = StartCriticalSection();

	(*s)++;     // set the semaphore

	EndCriticalSection(i_state);
}

/*********************************************** Public Functions *********************************************************************/
