#ifndef SENSORS_GPIO_H_
#define SENSORS_GPIO_H_

#include <tc27xd/Ifx_reg.h>

void Init_GPIO();

void setLED1(int onoff);
void setLED2(int onoff);

int getSW1(void);
int getSW2(void);

#endif /* SENSORS_GPIO_H_ */
