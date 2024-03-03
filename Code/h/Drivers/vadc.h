#ifndef VADC_INT_H_
#define VADC_INT_H_

void init_VADC(void);

void VADC_startConversion(void);

unsigned int VADC_readResult(void);

#endif /* VADC_INT_H_ */
