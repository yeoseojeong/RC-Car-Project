#ifndef MAIN_H_
#define MAIN_H_

/* Include System Function Headers */
#include <Drivers/vadc.h>
#include "system_tc2x.h"
#include "interrupts.h"

/* Include Register Definition Headers */
#include <tc27xd/Ifx_reg.h>

/* Include Device Drivers */
#include "Drivers/asclin.h"
#include "Drivers/can.h"
#include "Drivers/gpt12.h"
#include "Drivers/vadc.h"

/* Include Sensor Headers */
#include "Sensors/Buzzer.h"
#include "Sensors/Ultrasonic.h"
#include "Sensors/Motor.h"
#include "Sensors/Tof.h"
#include "Sensors/GPIO.h"
#include "Sensors/Motor.h"
#include "Sensors/Bluetooth.h"

/* Include Standard C Library */
#include <stdio.h>

/* Definition of Serial Command */
extern unsigned char cmd_clr_scr[8];
extern unsigned char cmd_clr_line[5];
extern unsigned char cmd_mov_cur_up[5];
extern unsigned char cmd;

#define CLR_SCR() my_printf("%s", cmd_clr_scr)
#define CLR_LINE() my_printf("%s", cmd_clr_line)
#define MOV_CUR_UP() my_printf("%s", cmd_mov_cur_up)

#ifndef NULL
#define NULL 0
#endif

#endif /* MAIN_H_ */
