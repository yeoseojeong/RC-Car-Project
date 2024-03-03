#include "Drivers/gpt12.h"
#include "Drivers/asclin.h"
#include "Sensors/Ultrasonic.h"
#include "main.h"
#include <tc27xd/Ifx_reg.h>
#include <machine/wdtcon.h>
#include <machine/intrinsics.h>
#include "interrupts.h"

#define ISR_PRIORITY_GPT12_TIMER	6	    /* Define the GPT12 Timer interrupt priority  */
#define GPT1_BLOCK_PRESCALER        32      /* GPT1 block prescaler value                 */
#define TIMER_T3_INPUT_PRESCALER    32      /* Timer input prescaler value                */
#define GPT120_MODULE_FREQUENCY 100000000

static volatile unsigned int lMotorDuty = 20;
static volatile unsigned int rMotorDuty = 20;
static volatile unsigned int cnt_10us = 0;
static volatile unsigned int cntDelay = 0;

extern void IsrGpt120T3Handler_Beep();

unsigned int getLeftMotorDuty()
{
	return lMotorDuty;
}

unsigned int getRightMotorDuty()
{
	return rMotorDuty;
}

void setLeftMotorDuty(unsigned int duty)
{
	cnt_10us = 0;
	lMotorDuty = duty;
}

void setRightMotorDuty(unsigned int duty)
{
	cnt_10us = 0;
	rMotorDuty = duty;
}

unsigned int getcntDelay()
{
	return cntDelay;
}

void setcntDelay(unsigned int n)
{
	cntDelay = n;
}

void IsrGpt2T6Handler()
{
	if (cnt_10us  < lMotorDuty) {
		MODULE_P02.OUT.B.P1 = 1; /* Left Motor (CH-A) */
	} else {
		MODULE_P02.OUT.B.P1 = 0; /* Left Motor (CH-A) */
	}

	if (cnt_10us < rMotorDuty) {
		MODULE_P10.OUT.B.P3 = 1; /* Right Motor (CH-B) */
	} else {
		MODULE_P10.OUT.B.P3 = 0; /* Right Motor (CH-B) */
	}

	if (cnt_10us == 100) {
		cnt_10us = 0;
	} else {
		cnt_10us++;
	}
	cntDelay++;
}

void init_gpt1(void)
{
    /* Initialize the GPT12 module */
	unlock_wdtcon();
	MODULE_GPT120.CLC.B.DISR = 0; /* Enable the GPT12 module */
	lock_wdtcon();

	/* Initialize the Timer T3 (PWM) */
	MODULE_GPT120.T3CON.B.BPS1 = 0x2; /* Set GPT1 block prescaler: 32 */
	MODULE_GPT120.T3CON.B.T3M = 0x0; /* Set T3 to timer mode */
	MODULE_GPT120.T3CON.B.T3UD = 0x1; /* Set T3 count direction(down) */
	MODULE_GPT120.T3CON.B.T3I = 0x5; /* Set T3 input prescaler(2^5=32) */
	/* Calculate dutyUpTime and dutyDownTime for reloading timer T3 */
	MODULE_GPT120.T3.U = 100; /* Set timer T3 value */
    /* Timer T2: reloads the value DutyDownTime in timer T3 */
    MODULE_GPT120.T2CON.B.T2M = 0x4; /* Set the timer T2 in reload mode */
    MODULE_GPT120.T2CON.B.T2I = 0x7; /* Reload Input Mode : Rising/Falling Edge T3OTL */
    MODULE_GPT120.T2.U = 100;
    InterruptInstall(SRC_ID_GPT120T3, (void(*)(int))IsrGpt120T3Handler_Beep, 3, 0);

    /* Initialize the Timer T4 for Ultrasonic */
    MODULE_GPT120.T4CON.B.T4M = 0x0; /* Set T4 to timer mode */
    MODULE_GPT120.T4CON.B.T4UD = 0x0; /* Set T4 count direction(up) */
    MODULE_GPT120.T4CON.B.T4I = 0x5; /* Set T4 input prescaler(2^5=32) */
    MODULE_GPT120.T4.U = 0u;
}


void init_gpt2(void)
{
    /* Initialize the GPT12 module */
	unlock_wdtcon();
	MODULE_GPT120.CLC.B.DISR = 0; /* Enable the GPT12 module */
	lock_wdtcon();

    /* Initialize the Timer T6 for delay_ms */
    MODULE_GPT120.T6CON.B.BPS2 = 0x0; /* Set GPT2 block prescaler: 4 */
    MODULE_GPT120.T6CON.B.T6M = 0x0; /* Set T6 to timer mode */
    MODULE_GPT120.T6CON.B.T6UD = 0x1; /* Set T6 count direction(down) */
    MODULE_GPT120.T6CON.B.T6I = 0x0; /* Set T6 input prescaler(2^0=1) */
    MODULE_GPT120.T6CON.B.T6OE = 0x1; /* Overflow/Underflow Output Enable */
    MODULE_GPT120.T6CON.B.T6SR = 0x1; /* Reload from register CAPREL Enabled */
    MODULE_GPT120.T6.U = 250u; /* Set T6 start value (10us) */

    MODULE_GPT120.CAPREL.U = 250u; /* Set CAPREL reload value */
	InterruptInstall(SRC_ID_GPT120T6, (void(*)(int))IsrGpt2T6Handler, 2, 0);
}


void runGpt12_T3()
{
	MODULE_GPT120.T3CON.B.T3R = 1;
}

void stopGpt12_T3()
{
	MODULE_GPT120.T3CON.B.T3R = 0;
}

void runGpt12_T6()
{
	MODULE_GPT120.T6CON.B.T6R = 1;
}

void stopGpt12_T6()
{
	MODULE_GPT120.T6CON.B.T6R = 0;
}

void runGpt12_T4()
{
	MODULE_GPT120.T4CON.B.T4R = 1;
}

void stopGpt12_T4()
{
	MODULE_GPT120.T4CON.B.T4R = 0;
}

void setGpt12_T4(unsigned short value)
{
	MODULE_GPT120.T4.U = value;
}

unsigned int getGpt12_T4()
{
	return MODULE_GPT120.T4.U;
}

// init_gpt1() should be called before using delay_ms()
void delay_ms(unsigned int delay_time)
{
	volatile unsigned int i, j;
	for(i=0; i<delay_time; i++)
		for(j=0; j<18200; j++)
			_nop();
}


