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
 * Wait Semaphore
 *  - Check if the desired resource is available.
 *  - If it is not, mark the currently running thread as blocked and start a context switch.
 *  - Param "s": Pointer to semaphore to wait on
 *  - THIS IS A CRITICAL SECTION
 */
void G8RTOS_WaitSemaphore(semaphore_t *s)
{
    uint32_t i_state = StartCriticalSection();

    /* Lock the resource/semaphore*/
    (*s)--;

    if ((*s) < 0)
    {
        CurrentlyRunningThread->blocked = s;

        /* Triggers context switch to let other threads go instead */
        G8RTOS_Yield();
    }

    EndCriticalSection(i_state);
}

/*
 * Signal Semaphore
 *  - Signals the completion of the usage of a resource.
 *  - Unblock 1 thread blocked by this resource if there are any.
 *  - Param "s": Pointer to semaphore to be signaled
 *  - THIS IS A CRITICAL SECTION
 */
void G8RTOS_SignalSemaphore(semaphore_t *s)
{
	uint32_t i_state = StartCriticalSection();

	/* Release the resource/semaphore */
	(*s)++;

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
