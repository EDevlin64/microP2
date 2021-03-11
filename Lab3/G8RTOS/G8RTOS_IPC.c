/*
 * G8RTOS_IPC.c
 */



/***************************************************** Includes ***********************************************************************/

#include "G8RTOS_IPC.h"
#include "G8RTOS.h"

/***************************************************** Includes ***********************************************************************/

/*************************************************** Defines Used *********************************************************************/

#define MAX_FIFOS 4
#define MAX_FIFO_SIZE 16

/*************************************************** Defines Used *********************************************************************/

/*********************************************** Data Structures Used *****************************************************************/

/*
 * Declaration of the Array of FIFOs
 */
static struct FIFO_t fifoArray[MAX_FIFOS];

/*********************************************** Data Structures Used *****************************************************************/

/*********************************************** Public Functions *********************************************************************/

int G8RTOS_InitFIFO(uint32_t index)
{
    if (index < MAX_FIFOS)
    {
        fifoArray[index].head = &fifoArray[index].buffer[0];
        fifoArray[index].tail = &fifoArray[index].buffer[0];
        fifoArray[index].lostData = 0;
        G8RTOS_InitSemaphore(&fifoArray[index].currentSize, 0); // init to 0 because empty FIFOs should block threads until it has data
        G8RTOS_InitSemaphore(&fifoArray[index].mutex, 1);

        return 1;
    }
    else
    {
        return 0;
    }
}

int32_t G8RTOS_ReadFIFO(uint32_t index)
{
    // ensures there is data to be read (buffer isn't empty)
    G8RTOS_WaitSemaphore(&fifoArray[index].currentSize);

    // ensures that FIFO was not in the middle of being read by another thread
    G8RTOS_WaitSemaphore(&fifoArray[index].mutex);

    // data read is the first in the FIFO
    int32_t data = *fifoArray[index].head;

    // if the head is at the final spot in the buffer, wrap around and make the head the first spot in the buffer
    // otherwise, increment the head
    if (fifoArray[index].head == &fifoArray[index].buffer[MAX_FIFO_SIZE-1])
    {
        fifoArray[index].head = &fifoArray[index].buffer[0];
    }
    else
    {
        fifoArray[index].head++;
    }

    // signal other threads that the FIFO is done being read by the currently running thread
    G8RTOS_SignalSemaphore(&fifoArray[index].mutex);

    return data;
}

int G8RTOS_WriteFIFO(uint32_t index, uint32_t data)
{
    // since the write FIFO is used in periodic tasks, if the mutex semaphore is blocked then it will wrongfully block a regular
    //G8RTOS_WaitSemaphore(&fifoArray[index].mutex);
    *fifoArray[index].tail = data;

    // release currentSize semaphore
    G8RTOS_SignalSemaphore(&fifoArray[index].currentSize);

    // check if current size is at full capacity
    if (fifoArray[index].currentSize > MAX_FIFO_SIZE - 1)
    {
        // discard new data, increment lostData, return error
        fifoArray[index].currentSize = MAX_FIFO_SIZE - 1;
        fifoArray[index].lostData++;

        return 1;
    }
    else
    {
        // write data to the tail and increment its position
        fifoArray[index].tail++;
        if(fifoArray[index].tail == &fifoArray[index].head) {
            fifoArray[index].tail = fifoArray[index].buffer;
        }

    }

    //G8RTOS_SignalSemaphore(&fifoArray[index].mutex);

    return 0;
}


/*********************************************** Public Functions *********************************************************************/

