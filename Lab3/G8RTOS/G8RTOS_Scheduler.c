/*
 * G8RTOS_Scheduler.c
 */

/*********************************************** Dependencies and Externs *************************************************************/
#include "BSP.h"
#include <G8RTOS/G8RTOS_Scheduler.h>
#include <G8RTOS/G8RTOS_Structures.h>
#include <stdint.h>
#include "msp.h"

/*
 * G8RTOS_Start exists in asm
 */
extern void G8RTOS_Start();

/* System Core Clock From system_msp432p401r.c */
extern uint32_t SystemCoreClock;

/*
 * Pointer to the currently running Thread Control Block
 */
extern tcb_t * CurrentlyRunningThread;

/*********************************************** Dependencies and Externs *************************************************************/


/*********************************************** Defines ******************************************************************************/

/* Status Register with the Thumb-bit Set */
#define THUMBBIT 0x01000000

#define SHPR3 (*((volatile unsigned int *)(0xe000ed20)))
#define PendSV_Priority (0xFF << 16)
#define SysTick_Priority (0xFF << 24)

/*********************************************** Defines ******************************************************************************/


/*********************************************** Data Structures Used *****************************************************************/

/* Thread Control Blocks
 *	- An array of thread control blocks to hold pertinent information for each thread
 */
static tcb_t threadControlBlocks[MAX_THREADS];

/* Periodic Thread Control Blocks
 *  - An array of periodic thread control blocks to hold pertinent information for each thread
 */
static ptcb_t periodicThreadControlBlocks[MAX_PERIODIC_THREADS];


/* Thread Stacks
 *	- An array of arrays that will act as individual stacks for each thread
 */
static int32_t threadStacks[MAX_THREADS][STACKSIZE];

/*********************************************** Data Structures Used *****************************************************************/


/*********************************************** Private Variables ********************************************************************/

/*
 * Current Number of Threads currently in the scheduler
 */
static uint32_t NumberOfThreads;

/*
 * Current Number of Periodic Threads currently in the scheduler
 */
static uint32_t NumberOfPeriodicThreads;

/*********************************************** Private Variables ********************************************************************/


/*********************************************** Private Functions ********************************************************************/

/*
 * Initializes the Systick and Systick Interrupt
 * The Systick interrupt will be responsible for starting a context switch between threads
 * Param "numCycles": Number of cycles for each systick interrupt
 */
static void InitSysTick(uint32_t numCycles)
{

    /* Init Systick to overflow every 1 ms */
    SysTick_Config(numCycles);

    /* Enable interrupts for systick */
    SysTick_enableInterrupt();

}

/*
 * Chooses the next thread to run.
 * Lab 2 Scheduling Algorithm:
 * 	- Simple Round Robin: Choose the next running thread by selecting the currently running thread's next pointer
 */
void G8RTOS_Scheduler()
{
    CurrentlyRunningThread = CurrentlyRunningThread->next;
    while (CurrentlyRunningThread->asleep || CurrentlyRunningThread->blocked)
    {
        CurrentlyRunningThread = CurrentlyRunningThread->next;
    }
}

/*
 * SysTick Handler
 * Currently the Systick Handler will only increment the system time
 * and set the PendSV flag to start the scheduler
 *
 * In the future, this function will also be responsible for sleeping threads and periodic threads
 */
void SysTick_Handler()
{
    SystemTime++;

    /* PERIODIC THREADS*/
    if (NumberOfPeriodicThreads > 0)
    {
        ptcb_t * pPtr = &periodicThreadControlBlocks[0];
        int i;
        for (i = 0 ; i < NumberOfPeriodicThreads ; i++)
        {
            if (SystemTime >= pPtr->executeTime)
            {
                pPtr->executeTime = SystemTime + pPtr->period;      // new execute time is 1 period from current time
                pPtr->handler();                                    // execute the code at the handler
            }
            pPtr = pPtr->next;
        }
    }

    /* SLEEPING THREADS - check to see if the NEXT thread is asleep and if it is time to wake it up (sleep count = system time) */
    tcb_t * nextThread = CurrentlyRunningThread->next;
    int i;
    for (i = 0 ; i < NumberOfThreads ; i++)
    {

        if ((nextThread->asleep) && (nextThread->sleepCount <= SystemTime))
        {
            nextThread->asleep = false;     // wake up thread
        }

        /* occurs every iteration */
        nextThread = nextThread->next;

    }

    /* Trigger context switch */
    SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;

}

void G8RTOS_Yield()
{
    SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
}

/*********************************************** Private Functions ********************************************************************/


/*********************************************** Public Variables *********************************************************************/

/* Holds the current time for the whole System */
uint32_t SystemTime;

/*********************************************** Public Variables *********************************************************************/


/*********************************************** Public Functions *********************************************************************/

/*
 * Sets variables to an initial state (system time and number of threads)
 * Enables board for highest speed clock and disables watchdog      $$$ make sure watchdog is diasbled and what not
 */
void G8RTOS_Init()
{
	/* Init system time to zero */
    SystemTime = 0;

    /* Set number of threads to zero */
    NumberOfThreads = 0;
    NumberOfPeriodicThreads = 0;

    /* Initalize all hardware on board */
    BSP_InitBoard();
}

/*
 * Starts G8RTOS Scheduler
 * 	- Initializes the Systick
 * 	- Sets Context to first thread
 * Returns: Error Code for starting scheduler. This will only return if the scheduler fails
 */
int G8RTOS_Launch()
{
    /* Init so interrupts occur every 1ms */
    double cycles = ClockSys_GetSysFreq()/1000;          // the number of cycles that occur in 1 ms               $$$
    InitSysTick((uint32_t) cycles);

	/* Sets context to the first thread */
    CurrentlyRunningThread = &threadControlBlocks[0];

    // set priorities
    //NVIC_SetPriority (PendSV_IRQn, (1UL << __NVIC_PRIO_BITS) - 1UL);
    SHPR3 |= SysTick_Priority | PendSV_Priority | 0x0000;

    G8RTOS_Start();

    return 0;
}


/*
 * Adds threads to G8RTOS Scheduler
 * 	- Checks if there are still available threads to insert to scheduler
 * 	- Initializes the thread control block for the provided thread
 * 	- Initializes the stack for the provided thread to hold a "fake context"
 * 	- Sets stack tcb stack pointer to top of thread stack
 * 	- Sets up the next and previous tcb pointers in a round robin fashion
 * Param "threadToAdd": Void-Void Function to add as preemptable main thread
 * Returns: Error code for adding threads
 */
int G8RTOS_AddThread(void (*threadToAdd)(void))
{
    if (NumberOfThreads < MAX_THREADS)
    {
        // init the thread control blocks for the provided thread

        // initialize the stack for the provided thread to hold fake context
        // Sets stack tcb stack pointer to top of thread stack
        setInitialStack(threadToAdd);

        // Sets up the next and previous tcb pointers in a round robin fashion
        if (NumberOfThreads == 0)
        {
            threadControlBlocks[0].sp = &threadStacks[0][STACKSIZE-16];
            threadControlBlocks[0].next = &threadControlBlocks[0];
            threadControlBlocks[0].prev = &threadControlBlocks[0];
        }
        else if (NumberOfThreads > 0)
        {
            threadControlBlocks[NumberOfThreads].sp = &threadStacks[NumberOfThreads][STACKSIZE-16];
            threadControlBlocks[0].prev = &threadControlBlocks[NumberOfThreads];
            threadControlBlocks[NumberOfThreads-1].next = &threadControlBlocks[NumberOfThreads];
            threadControlBlocks[NumberOfThreads].prev = &threadControlBlocks[NumberOfThreads-1];
            threadControlBlocks[NumberOfThreads].next = &threadControlBlocks[0];
        }

        // increment the number of threads
        NumberOfThreads += 1;

        return 0;
    }
    else
    {
        return 1;
    }
}

/*
 * Adds periodic threads to G8RTOS Scheduler
 *  - Checks if there are still available threads to insert to scheduler
 *  - Initializes the thread control block for the provided thread
 *  - Initializes the stack for the provided thread to hold a "fake context"
 *  - Sets stack tcb stack pointer to top of thread stack
 *  - Sets up the next and previous tcb pointers in a round robin fashion
 * Param "threadToAdd": Void-Void Function to add as preemptable main thread
 * Returns: Error code for adding threads
 */
int G8RTOS_AddPeriodicThread(void (*threadToAdd)(void), uint32_t period)
{
    if (NumberOfPeriodicThreads < MAX_PERIODIC_THREADS)
    {
        // init the periodic thread control blocks for the provided periodic thread
        periodicThreadControlBlocks[NumberOfPeriodicThreads].handler = threadToAdd;
        periodicThreadControlBlocks[NumberOfPeriodicThreads].period = period;
        periodicThreadControlBlocks[NumberOfPeriodicThreads].currentTime = SystemTime;

        // Sets up the next and previous tcb pointers in a round robin fashion
        if (NumberOfPeriodicThreads == 0)
        {
            periodicThreadControlBlocks[NumberOfPeriodicThreads].executeTime = SystemTime + period;
            periodicThreadControlBlocks[0].next = &periodicThreadControlBlocks[0];
            periodicThreadControlBlocks[0].prev = &periodicThreadControlBlocks[0];
        }
        else if (NumberOfPeriodicThreads > 0)
        {
            periodicThreadControlBlocks[NumberOfPeriodicThreads].executeTime = SystemTime + period + NumberOfPeriodicThreads;     // +1 to offset LCM periods
            periodicThreadControlBlocks[0].prev = &periodicThreadControlBlocks[NumberOfPeriodicThreads];
            periodicThreadControlBlocks[NumberOfPeriodicThreads-1].next = &periodicThreadControlBlocks[NumberOfPeriodicThreads];
            periodicThreadControlBlocks[NumberOfPeriodicThreads].prev = &periodicThreadControlBlocks[NumberOfPeriodicThreads-1];
            periodicThreadControlBlocks[NumberOfPeriodicThreads].next = &periodicThreadControlBlocks[0];
        }

        // increment the number of periodic threads
        NumberOfPeriodicThreads++;

        return 0;
    }
    else // if we are at full capacity for periodic threads
    {
        return 1;
    }
}

/* setInit
 * - Sets dummy values for the stacks of each thread
 * - R0-R3, R12, PC, LR, PSR get auto pushed onto stack (does not push SP)
 * - sets PC to function pointer of thread
 */
void setInitialStack(void (*threadToAdd)(void))
{
    threadControlBlocks[NumberOfThreads].sp = &threadStacks[NumberOfThreads][STACKSIZE-16];    // points to where core regs are going to be popped from in pendSV
    threadStacks[NumberOfThreads][STACKSIZE-1] = THUMBBIT;                      // as if thumb bit in psr was set (stacksize-1 is where psr is auto pushed)
    threadStacks[NumberOfThreads][STACKSIZE-2] = (int32_t)(threadToAdd);        // sets PC to the address in prog mem where the thread code is
    threadStacks[NumberOfThreads][STACKSIZE-3] = 0x0D0D0D0D;                    // r14 aka LR
    threadStacks[NumberOfThreads][STACKSIZE-4] = 0x0C0C0C0C;                    // r12
    threadStacks[NumberOfThreads][STACKSIZE-5] = 0x03030303;                    // r3
    threadStacks[NumberOfThreads][STACKSIZE-6] = 0x02020202;                    // r2
    threadStacks[NumberOfThreads][STACKSIZE-7] = 0x01010101;                    // r1
    threadStacks[NumberOfThreads][STACKSIZE-8] = 0x00000000;                    // r0
    threadStacks[NumberOfThreads][STACKSIZE-9] = 0x0B0B0B0B;                    // r11
    threadStacks[NumberOfThreads][STACKSIZE-10] = 0x0A0A0A0A;                   // r10
    threadStacks[NumberOfThreads][STACKSIZE-11] = 0x09090909;                   // r9
    threadStacks[NumberOfThreads][STACKSIZE-12] = 0x08080808;                   // r8
    threadStacks[NumberOfThreads][STACKSIZE-13] = 0x07070707;                   // r7
    threadStacks[NumberOfThreads][STACKSIZE-14] = 0x06060606;                   // r6
    threadStacks[NumberOfThreads][STACKSIZE-15] = 0x05050505;                   // r5
    threadStacks[NumberOfThreads][STACKSIZE-16] = 0x04040404;                   // r4
}

void G8RTOS_Sleep(uint32_t duration)
{
    CurrentlyRunningThread->sleepCount = duration + SystemTime;
    CurrentlyRunningThread->asleep = true;
    G8RTOS_Yield();
}

/*********************************************** Public Functions *********************************************************************/
