#ifndef SENSORS_TOF_H_
#define SENSORS_TOF_H_

void Init_ToF(void);
void IsrUart1RxHandler_tof(void);
int getTofDistance(void);

#endif /* SENSORS_TOF_H_ */
