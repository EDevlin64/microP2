/*
 * G8RTOS_Structure.h
 *
 *  Created on: Jan 12, 2017
 *      Author: Raz Aloni
 */

#ifndef G8RTOS_STRUCTURES_H_
#define G8RTOS_STRUCTURES_H_

#include <G8RTOS/G8RTOS.h>
#include "stdbool.h"

/*********************************************** Data Structure Definitions ***********************************************************/

/*
 *  Thread Control Block:
 *      - Every thread has a Thread Control Block
 *      - The Thread Control Block holds information about the Thread Such as the Stack Pointer, Priority Level, and Blocked Status
 *      - For Lab 2 the TCB will only hold the Stack Pointer, next TCB and the previous TCB (for Round Robin Scheduling)
 */
typedef struct tcb_t
{
    int32_t * sp;           // stack pinter for this thread
    struct tcb_t * next;    // pointer to next tcb (RR scheduling)
    struct tcb_t * prev;    // pointer to previous tcb (RR scheduling)
    semaphore_t * blocked;  // blocking semaphore
    uint32_t sleepCount;    // keeps track of how long tcb has been sleep
    bool asleep;            // state where thread waits for a fixed amount of time before it enters active state

} tcb_t;

/*
 * Periodic Thread Control Block
 */
typedef struct ptcb_t
{
    void (*handler)(void);
    uint32_t period;
    uint32_t executeTime;
    uint32_t currentTime;
    struct pEvent_t * next;
    struct pEvent_t * prev;
} ptcb_t;

/*********************************************** Data Structure Definitions ***********************************************************/


/*********************************************** Public Variables *********************************************************************/

tcb_t * CurrentlyRunningThread;

/*********************************************** Public Variables *********************************************************************/

#endif /* G8RTOS_STRUCTURES_H_ */
