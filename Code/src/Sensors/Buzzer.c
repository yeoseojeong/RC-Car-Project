#include "Sensors/Buzzer.h"
#include "Drivers/gpt12.h"
#include "Drivers/asclin.h"
#include <machine/wdtcon.h>
#include <machine/intrinsics.h>
#include "interrupts.h"
#include "main.h"

int beepCnt = 0;
int beepOnOff = 0;

void Init_Buzzer()
{
	MODULE_P02.IOCR0.B.PC3 = 0b10000;

	init_gpt1();

	runGpt12_T3();
}

void setBeepCycle(int cycle)
{
	beepOnOff = cycle;
}



void Beep(unsigned int hz)
{
	volatile int loop = 1000000 / hz / 2; /* 주파수에 따른 us 계산 */
	for (volatile int i = 0; i < loop; i++) /* loop * 1us 동안 delay */
		for (volatile int j = 0; j < 1; j++)
			for (volatile int k = 0; k < 1; k++)
				_nop();
}

void IsrGpt120T3Handler_Beep()
{
	if ((beepCnt < beepOnOff) || (beepOnOff == 1)) {
		MODULE_P02.OUT.B.P3 ^= 1;
	} else if (beepCnt < beepOnOff * 2) {
		MODULE_P02.OUT.B.P3 = 0;
	} else {
		beepCnt = 0;
	}
	beepCnt++;
}
