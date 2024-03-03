#include "Sensors/GPIO.h"

void Init_GPIO()
 {
	/* Set P10.2(LED1) as push-pull output */
	MODULE_P10.IOCR0.B.PC2 = 0b10000;

	/* Set P10.1(LED2) as push-pull output */
	MODULE_P10.IOCR0.B.PC1 = 0b10000;

	/* Set P2.0(SW1) as input pull-up */
	MODULE_P02.IOCR0.B.PC0 = 0b00010;

	/* Set P2.1(SW2) as input pull-up */
	MODULE_P02.IOCR0.B.PC1 = 0b00010;
}

void setLED1(int onoff)	// on if true, off if false
{
	if(onoff)
	{
		MODULE_P10.OUT.B.P2 = 1;
	}
	else
	{
		MODULE_P10.OUT.B.P2 = 0;
	}
}

void setLED2(int onoff)
{
	if(onoff)
	{
		MODULE_P10.OUT.B.P1 = 1;
	}
	else
	{
		MODULE_P10.OUT.B.P1 = 0;
	}
}

int getSW1(void)
{
	return MODULE_P02.IN.B.P0;
}

int getSW2(void)
{
	return MODULE_P02.IN.B.P1;
}
