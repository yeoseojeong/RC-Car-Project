#ifndef ULTRASONIC_H_
#define ULTRASONIC_H_

void Init_Ultrasonics(void);
float ReadRearUltrasonic_noFilt();
float ReadRearUltrasonic_Filt();
float ReadLeftUltrasonic_noFilt();
float ReadLeftUltrasonic_Filt();
float ReadRightUltrasonic_noFilt();
float ReadRightUltrasonic_Filt();

#define FILT_SIZE 5

#endif /* ULTRASONIC_H_ */

