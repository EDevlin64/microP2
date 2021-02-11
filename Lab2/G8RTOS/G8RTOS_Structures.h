/*
 * G8RTOS_Structure.h
 *
 *  Created on: Jan 12, 2017
 *      Author: Raz Aloni
 */

#ifndef G8RTOS_STRUCTURES_H_
#define G8RTOS_STRUCTURES_H_

#include <G8RTOS/G8RTOS.h>

/*********************************************** Data Structure Definitions ***********************************************************/

/*
 *  Thread Control Block:
 *      - Every thread has a Thread Control Block
 *      - The Thread Control Block holds information about the Thread Such as the Stack Pointer, Priority Level, and Blocked Status
 *      - For Lab 2 the TCB will only hold the Stack Pointer, next TCB and the previous TCB (for Round Robin Scheduling)
 */
typedef struct tcb_t
{
    int32_t * sp;
    struct tcb_t * next;
    struct tcb_t * prev;

} tcb_t;

/*********************************************** Data Structure Definitions ***********************************************************/


/*********************************************** Public Variables *********************************************************************/

tcb_t * CurrentlyRunningThread;

/*********************************************** Public Variables *********************************************************************/

#endif /* G8RTOS_STRUCTURES_H_ */
