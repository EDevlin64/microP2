#include <BSP.h>
#include "msp.h"
#include "G8RTOS.h"
#include "ThreadsQuiz.h"
#include <stdbool.h>

#define RMS_NVALUE 16
#define RMS_COUNT_MASK (RMS_NVALUE - 1)

semaphore_t ledI2cSemaphore;
semaphore_t sensorI2cSemaphore;

static bool rmsBelowThresh = false;
volatile int16_t joystickX = 0;
static int32_t currentTemperature = 0;
static int32_t decayedJoystickAvg = 0;

// QUIZ variables
volatile uint16_t playerPosition = (1<<14);
volatile uint16_t enemyPosition = (1<<3);
volatile uint16_t coinCounter = 0;
volatile uint16_t lossCounter = 0;
volatile uint16_t score = 0;

uint16_t coinPosition0 = (1<<0);
uint16_t coinPosition1 = (1<<4);
uint16_t coinPosition2 = (1<<8);
uint16_t coinPosition3 = (1<<12);




/*
 * player is led position periodic thread where if joystick <0 go left, if joystick > 0 go right one spot every 500 ms
 *
 * if player position && coin position = true, then remove coin (~player position) but put it back 5 seconds later
 *
 * periodic thread 1 second for displaying
 */

/*
 * Periodic task that gets joystick reading every 100 ms
 */
void player_joystick(void)
{
    int16_t x_coord, y_coord;
    GetJoystickCoordinates(&x_coord, &y_coord);
    joystickX = x_coord;

}

/*
 * Periodically places 4 coins every 4 spots every 5 seconds
 */
void coins(void)
{
    G8RTOS_WaitSemaphore(&ledI2cSemaphore);
    LP3943_LedModeSet(GREEN, coinPosition0 | coinPosition1 | coinPosition2 | coinPosition3);
    G8RTOS_SignalSemaphore(&ledI2cSemaphore);
}
/*
 * Periodic thread - print out the score every 1 second
 */
void score_thread(void)
{
        score = coinCounter + lossCounter;
        BackChannelPrintIntVariable("Your current score is: ", score);
}

/*
 * enemy speed = 1/8 the max speed of player, but increases by 1/8 speed every 5 seconds
 */
void enemy_position(void)
{
    while(1)
    {
        /*
        if (coinCounter > 5)
        {
            // update enemy speed .. and so on
        }
        */

        enemyPosition = enemyPosition << 1;

        G8RTOS_WaitSemaphore(&ledI2cSemaphore);
        LP3943_LedModeSet(RED, enemyPosition);
        G8RTOS_Scheduler(&ledI2cSemaphore);
    }
}

void player_position(void)
{
    int16_t x_coord;
    int16_t shiftAmount = 0, oldLedBitMask = 0;
    while(1)
    {
        //Read joystick FIFO
        x_coord = joystickX;

        //decayedJoystickAvg = (decayedJoystickAvg + joystickX) >> 1;

        //Set led bit mask corresponding to the joystick data
        if(x_coord > 6000) shiftAmount = 4;
        else if(x_coord > 4000) shiftAmount = 3;
        else if(x_coord > 2000) shiftAmount = 2;
        else if(x_coord > 500) shiftAmount = 1;
        else if(x_coord < -6000) shiftAmount = -4;
        else if(x_coord < -4000) shiftAmount = -3;
        else if(x_coord < -2000) shiftAmount = -2;
        else if(x_coord < -500) shiftAmount = -1;
        else shiftAmount = 0;

        // updating the player position and doing wrap around.
        if (playerPosition == (1<<0))
        {
            playerPosition = playerPosition << (shiftAmount + 15);
        }
        else if(playerPosition == (1<<15))
        {
            playerPosition = playerPosition << (shiftAmount - 15);
        }
        else
        {
            playerPosition = playerPosition << shiftAmount;

        }

        // calculating score: if a player ever lands on a coin position or enemy position update the counters
        if ((playerPosition == coinPosition0) || (playerPosition == coinPosition1) || (playerPosition == coinPosition2) || (playerPosition == coinPosition3))
        {
            coinCounter++;
        }
        else if (playerPosition == enemyPosition)
        {
            lossCounter++;
        }

        G8RTOS_WaitSemaphore(&ledI2cSemaphore);
        LP3943_LedModeSet(BLUE, playerPosition);
        G8RTOS_SignalSemaphore(&ledI2cSemaphore);
        //G8RTOS_Sleep(300);
    }
}


