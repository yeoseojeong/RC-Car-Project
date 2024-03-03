#ifndef CAN_H_
#define CAN_H_

void initCAN(void);

void Send_CanMsg(unsigned int id, const unsigned char *txData, int len);

int Recv_CanMsg(unsigned int *id, unsigned char *rxData, int *len);

void isr_rx_can(void);

void isr_tx_can(void);

#endif /* CAN_H_ */
