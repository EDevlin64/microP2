/*
 * G8RTOS_Semaphores.c
 */

/*********************************************** Dependencies and Externs *************************************************************/

#include <G8RTOS/G8RTOS_CriticalSection.h>
#include <G8RTOS/G8RTOS_Semaphores.h>
#include "G8RTOS_Scheduler.h"
#include "G8RTOS_Structures.h"
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

    (*s)--;

    if ((*s) < 0)
    {
        CurrentlyRunningThread->blocked = s;

        EndCriticalSection(i_state);            // enable interrupts

        G8RTOS_Yield();                         // triggers context switch to let other thread go instead
    }

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

	(*s)++;     // set the semaphore - resource

	if ((*s) <= 0)
	{
	    tcb_t *pt = CurrentlyRunningThread->next;
	    while(pt->blocked != s)
	    {
	        pt = pt->next;
	    }

	    pt->blocked = 0;
	}

	EndCriticalSection(i_state);
}

/*********************************************** Public Functions *********************************************************************/
