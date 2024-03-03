#include "Sensors/Ultrasonic.h"
#include "Drivers/gpt12.h"
#include "main.h"
#include <tc27xd/Ifx_reg.h>

void Init_Ultrasonics()
{
	/* Init Rear Ultrasonic Pin */
	MODULE_P00.IOCR0.B.PC0 = 0b10000; /* Rear TRIG */
	MODULE_P00.IOCR0.B.PC1 = 0b00010; /* Rear ECHO */
	/* Init Left Ultrasonic Pin */
	MODULE_P00.IOCR4.B.PC6 = 0b10000; /* Left TRIG */
	MODULE_P00.IOCR4.B.PC7 = 0b00010; /* Left ECHO */
	/* Init Right Ultrasonic Pin */
	MODULE_P00.IOCR0.B.PC3 = 0b10000; /* Right TRIG */
	MODULE_P00.IOCR4.B.PC4 = 0b00010; /* Right ECHO */

	// Init GPT1 for T4 timer
	init_gpt1();
}

float ReadRearUltrasonic_noFilt()
{
	volatile int j=0;
	unsigned int timer_end;
	float rear_duration;
	float distance;

	/* Send Trigger Pulse */
	MODULE_P00.OUT.B.P0 = 1; // Rear TRIG_HIGH
	for(j=0; j<1000; j++) continue;
	MODULE_P00.OUT.B.P0 = 0; // Rear TRIG_LOW

	/* Calculate Distance */
	setGpt12_T4(0);
	while (MODULE_P00.IN.B.P1 == 0); // wait for Rear ECHO_HIGH
	runGpt12_T4();
	while (MODULE_P00.IN.B.P1 == 1); // wait for Rear ECHO_LOW
	stopGpt12_T4();

	timer_end = getGpt12_T4();
	rear_duration = (timer_end - 0) * 10.24;

	distance = 0.0343 * rear_duration / 2.0; // cm/us
	return distance;
}



float ReadRearUltrasonic_Filt()
{
	float distance_nofilt;
	static float rear_avg_filt_buf[FILT_SIZE] = {0,};
	static int rear_old_index = 0;
	float distance_filt;
	static int rear_sensorRxCnt = 0;

	distance_nofilt = ReadRearUltrasonic_noFilt();

	++rear_old_index;
	rear_old_index %= FILT_SIZE;  // Buffer Size = 5
	rear_avg_filt_buf[rear_old_index] = distance_nofilt;
	rear_sensorRxCnt++;

	/* Calculate Moving Average Filter */
	if (rear_sensorRxCnt >= FILT_SIZE) {
		float sum = 0;
		for (int i = 0; i < FILT_SIZE; i++) { sum += rear_avg_filt_buf[i]; }
		distance_filt = sum / FILT_SIZE;
	}
	else
		distance_filt = distance_nofilt;

	return distance_filt;
}

float ReadLeftUltrasonic_noFilt()
{
	volatile int j=0;
	unsigned int timer_end;
	float left_duration;
	float distance;

	/* Send Trigger Pulse */
	MODULE_P00.OUT.B.P6 = 1; // Left TRIG_HIGH
	for(j=0; j<1000; j++) continue;
	MODULE_P00.OUT.B.P6 = 0; // Left TRIG_LOW

	/* Calculate Distance */
	setGpt12_T4(0);
	while (MODULE_P00.IN.B.P7 == 0); // wait for Left ECHO_HIGH
	runGpt12_T4();
	while (MODULE_P00.IN.B.P7 == 1); // wait for Left ECHO_LOW
	stopGpt12_T4();

	timer_end = getGpt12_T4();
	left_duration = (timer_end - 0) * 10.24;

	distance = 0.0343 * left_duration / 2.0; // cm/us
	return distance;
}

float ReadLeftUltrasonic_Filt()
{
	float distance_nofilt;
	static float left_avg_filt_buf[FILT_SIZE] = {0,};
	static int left_old_index = 0;
	float distance_filt;
	static int left_sensorRxCnt = 0;

	distance_nofilt = ReadLeftUltrasonic_noFilt();

	++left_old_index;
	left_old_index %= FILT_SIZE;  // Buffer Size = 5
	left_avg_filt_buf[left_old_index] = distance_nofilt;
	left_sensorRxCnt++;

	/* Calculate Moving Average Filter */
	if (left_sensorRxCnt >= FILT_SIZE) {
		float sum = 0;
		for (int i = 0; i < FILT_SIZE; i++) { sum += left_avg_filt_buf[i]; }
		distance_filt = sum / FILT_SIZE;
	}
	else
		distance_filt = distance_nofilt;

	return distance_filt;
}

float ReadRightUltrasonic_noFilt()
{
	volatile int j=0;
	unsigned int timer_end;
	float right_duration;
	float distance;

	/* Send Trigger Pulse */
	MODULE_P00.OUT.B.P3 = 1; // Right TRIG_HIGH
	for(j=0; j<1000; j++) continue;
	MODULE_P00.OUT.B.P3 = 0; // Right TRIG_LOW

	/* Calculate Distance */
	setGpt12_T4(0);
	while (MODULE_P00.IN.B.P4 == 0); // wait for Right ECHO_HIGH
	runGpt12_T4();
	while (MODULE_P00.IN.B.P4 == 1); // wait for Right ECHO_LOW
	stopGpt12_T4();

	timer_end = getGpt12_T4();
	right_duration = (timer_end - 0) * 10.24;

	distance = 0.0343 * right_duration / 2.0; // cm/us
	return distance;
}



float ReadRightUltrasonic_Filt()
{
	float distance_nofilt;
	static float right_avg_filt_buf[FILT_SIZE] = {0,};
	static int right_old_index = 0;
	float distance_filt;
	static int right_sensorRxCnt = 0;

	distance_nofilt = ReadRightUltrasonic_noFilt();

	++right_old_index;
	right_old_index %= FILT_SIZE;  // Buffer Size = 5
	right_avg_filt_buf[right_old_index] = distance_nofilt;
	right_sensorRxCnt++;

	/* Calculate Moving Average Filter */
	if (right_sensorRxCnt >= FILT_SIZE) {
		float sum = 0;
		for (int i = 0; i < FILT_SIZE; i++) { sum += right_avg_filt_buf[i]; }
		distance_filt = sum / FILT_SIZE;
	}
	else
		distance_filt = distance_nofilt;

	return distance_filt;
}
