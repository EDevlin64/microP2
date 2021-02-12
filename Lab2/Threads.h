/*
 * Threads.h
 */

#ifndef THREADS_H_
#define THREADS_H_

#include <stdint.h>
#include "G8RTOS_Scheduler.h"
#include "G8RTOS_Semaphores.h"

uint32_t sensor_I2C_semaphore;
uint32_t led_I2C_semaphore;

void Task0 (void);
void Task1 (void);
void Task2 (void);

#endif /* THREADS_H_ */
