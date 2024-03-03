#include "Drivers/can.h"
#include "interrupts.h"
#include <machine/wdtcon.h>
#include <machine/intrinsics.h>
#include <tc27xd/Ifx_reg.h>
#include "Drivers/asclin.h"

// SCU Registers
#define LCK             1
#define ENDINIT         0

#define CAN0_LOOPBACK_EN 1

unsigned int rx_id;
unsigned int rx_dlc;
unsigned char rxd[8];

void initCAN(void)
{
	/* Configure RGB */
	MODULE_P02.IOCR4.B.PC7 = 0b10000; /* Set RGB_LED(P02.7) as push-pull output */
	MODULE_P10.IOCR4.B.PC5 = 0b10000; /* Set RGB_LED(P10.5) as push-pull output */
	MODULE_P10.IOCR0.B.PC3 = 0b10000; /* Set RGB_LED(P10.3) as push-pull output */
	/* Configure Rx, Tx pin */
	MODULE_P20.IOCR4.B.PC7 = 0b00000; /* Set P20.7 as RXDCAN0 */
	MODULE_P20.IOCR8.B.PC8 = 0b10101; /* Set P20.8 as TXDCAN0 */

	unlock_wdtcon();
	MODULE_CAN.CLC.U = 0; /* Disable module control */
	MODULE_CAN.FDR.U = (1 << 14) | 0x3FF; /* DM=1, STEP=1023 */
	lock_wdtcon();
	MODULE_CAN.MCR.U = 0x1; /* CLKSEL=1, use fASYNC_CAN */

	/* Init CAN0 node */
	MODULE_CAN.N[0].CR.U  = (1 << 6) | 1; /* CCE=1, INIT=1 */
	MODULE_CAN.N[0].BTR.U = 0x3EC9; 		/* set bit segments */
	MODULE_CAN.N[0].PCR.B.LBM = 0;
#ifdef CAN0_LOOPBACK_EN
	MODULE_CAN.N[0].PCR.U = (1 << 8); 	/* LBM(Loop-Back mode)=1 */
	/* Init CAN1 nodes */
	MODULE_CAN.N[1].CR.U = (1 << 6) | 1; /* CCE=1, INIT=1 */
	MODULE_CAN.N[1].PCR.U = (1 << 8); /* LBM(Loop-Back mode)=1 */
	MODULE_CAN.N[1].BTR.U = 0x3EC9; /* set bit */
#else
	MODULE_CAN.N[0].PCR.B.RXSEL = 0b001;
#endif

	/* Allocate message objects to CAN nodes */
	while (MODULE_CAN.PANCTR.U & (0x00000100 | 0x00000200));
	MODULE_CAN.PANCTR.U = (1 << 24) | (0 << 16) | 2; /* MO 0 to list 1 */
	while (MODULE_CAN.PANCTR.U & (0x00000100 | 0x00000200));
#ifdef CAN0_LOOPBACK_EN
	MODULE_CAN.PANCTR.U = (2 << 24) | (1 << 16) | 2; /* MO 1 to list 2 */
#else
	MODULE_CAN.PANCTR.U = (1 << 24) | (1 << 16) | 2; /* MO 1 to list 1 */
#endif

	/* Init MO_0 (list 1) */
	MODULE_CAN.MO[0].CTR.U = (1 << 27) | (1 << 26) | (1 << 25) | (1<< 21); /* set to Tx */
	MODULE_CAN.MO[0].FCR.U = (8 << 24); /* DLC = 8 */
	MODULE_CAN.MO[0].AR.U = (1 << 30) | (0xFF << 18); /* RPI=01, ID=0XFF */
	MODULE_CAN.MO[0].IPR.U = (1 << 4); /* TXINP=1 -> select INT_01 */
	MODULE_CAN.MO[0].FCR.B.TXIE = 1;
	/* Init MO_1 (list 2) */
	MODULE_CAN.MO[1].CTR.U = (1 << 23) | (1 << 21); /* set to Rx */
	MODULE_CAN.MO[1].FCR.U = (1 << 16); /* RXIE=1 */
	MODULE_CAN.MO[1].AR.U = (1 << 30) | (0x200 << 18); /* PRI=01, ID=0x0 */
	MODULE_CAN.MO[1].AMR.U = (1 << 29) | (0x200 << 18); /* No acceptance filter */
	MODULE_CAN.MO[1].IPR.U = 0x0; /* RXINP=0 -> select INT_00 */

	MODULE_CAN.N[0].CR.U &= ~((1 << 6) | 1); /* Reset CCE and INIT */
#ifdef CAN0_LOOPBACK_EN
	MODULE_CAN.N[1].CR.U &= ~((1 << 6) | 1); /* Reset CCE and INIT */
#endif
	InterruptInstall(SRC_ID_CANINT0, (void(*)(int))isr_rx_can, 2, 0);
	InterruptInstall(SRC_ID_CANINT1, (void(*)(int))isr_tx_can, 3, 0);
}

void Send_CanMsg(unsigned int id, const unsigned char *txData, int len)
{
	/* Load Data, Start CAN nodes */
	MODULE_CAN.MO[0].AR.B.ID = id << 18;
	for (int i = 0; i < len; i++) {
		if (i < 4) MODULE_CAN.MO[0].DATAL.U |= txData[i] << (i * 8);
		else MODULE_CAN.MO[0].DATAH.U |= txData[i] << ((i - 4) * 8);
	}
	MODULE_CAN.MO[0].FCR.B.DLC = len;

	/* Set transmit request to send message */
	MODULE_CAN.MO[0].CTR.U = (1 << 24) | (1 << 19) | (1 << 6); /* TXRQ=1, NEWDAT=1 */
}

int Recv_CanMsg(unsigned int *id, unsigned char *rxData, int *len)
{
	int err = 0;

	while ((MODULE_CAN.MO[1].STAT.B.NEWDAT) != 1); // check if new data is received
	*len = MODULE_CAN.MO[1].FCR.B.DLC;
	*id = MODULE_CAN.MO[1].AR.B.ID >> 18;
	switch (*len) {
	case 8: rxData[7] = MODULE_CAN.MO[1].DATAH.B.DB7;
		/* no break */
	case 7: rxData[6] = MODULE_CAN.MO[1].DATAH.B.DB6;
		/* no break */
	case 6:	rxData[5] = MODULE_CAN.MO[1].DATAH.B.DB5;
		/* no break */
	case 5:	rxData[4] = MODULE_CAN.MO[1].DATAH.B.DB4;
		/* no break */
	case 4:	rxData[3] = MODULE_CAN.MO[1].DATAL.B.DB3;
		/* no break */
	case 3:	rxData[2] = MODULE_CAN.MO[1].DATAL.B.DB2;
		/* no break */
	case 2:	rxData[1] = MODULE_CAN.MO[1].DATAL.B.DB1;
		/* no break */
	case 1: rxData[0] = MODULE_CAN.MO[1].DATAL.B.DB0;
		break;
	}

	return err;
}

void isr_rx_can(void)
{
	volatile unsigned int i = 0;
	/* Green RGB ON */
	MODULE_P02.OUT.B.P7 = 0;
	MODULE_P10.OUT.B.P5 = 1;
	MODULE_P10.OUT.B.P3 = 0;
	while (i++ < 1000000);
	rx_dlc = MODULE_CAN.MO[1].FCR.B.DLC;
	rx_id = MODULE_CAN.MO[1].AR.B.ID >> 18;
	switch (rx_dlc) {
	case 8: rxd[7] = MODULE_CAN.MO[1].DATAH.B.DB7;
		/* no break */
	case 7: rxd[6] = MODULE_CAN.MO[1].DATAH.B.DB6;
		/* no break */
	case 6:	rxd[5] = MODULE_CAN.MO[1].DATAH.B.DB5;
		/* no break */
	case 5:	rxd[4] = MODULE_CAN.MO[1].DATAH.B.DB4;
		/* no break */
	case 4:	rxd[3] = MODULE_CAN.MO[1].DATAL.B.DB3;
		/* no break */
	case 3:	rxd[2] = MODULE_CAN.MO[1].DATAL.B.DB2;
		/* no break */
	case 2:	rxd[1] = MODULE_CAN.MO[1].DATAL.B.DB1;
		/* no break */
	case 1: rxd[0] = MODULE_CAN.MO[1].DATAL.B.DB0;
		break;
	}
	my_printf("CAN_RX: %s (ID: 0x%X, DLC: %d)\n", rxd, rx_id, rx_dlc);
	MODULE_P02.OUT.B.P7 = 0;
	MODULE_P10.OUT.B.P5 = 0;
	MODULE_P10.OUT.B.P3 = 0;
}

void isr_tx_can(void)
{
	volatile int i = 0;
	/* Blue RGB ON*/
	MODULE_P02.OUT.B.P7 = 0;
	MODULE_P10.OUT.B.P5 = 0;
	MODULE_P10.OUT.B.P3 = 1;
	while (i++ < 1000000);
	MODULE_P02.OUT.B.P7 = 0;
	MODULE_P10.OUT.B.P5 = 0;
	MODULE_P10.OUT.B.P3 = 0;
}
