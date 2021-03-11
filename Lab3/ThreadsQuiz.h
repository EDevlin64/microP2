/*
 * Threads.h
 */

#ifndef THREADSQUIZ_H_
#define THREADSQUIZ_H_

#include <stdint.h>
#include "G8RTOS.h"

extern semaphore_t ledI2cSemaphore;
extern semaphore_t sensorI2cSemaphore;

#define FIFO_INDEX_JOYSTICK 0
#define FIFO_INDEX_TEMP 1
#define FIFO_INDEX_LIGHT 2

/* Semaphore/Resource Declarations */
extern semaphore_t sensorI2cSemaphore;
extern semaphore_t ledI2cSemaphore;

/* Thread Declarations */
void score_thread(void);
void player_joystick(void);
void player_position(void);
void coins(void);

/* Functions */

/*
 * Calculates the square root of a number using Newton's method
 * Returns the square root value
 */
uint32_t squareRoot(uint32_t value);



//void Task0 (void);
//void Task1 (void);
//void Task2 (void);
//void PeriodicTask0 (void);

#endif /* THREADSQUIZ_H_ */
