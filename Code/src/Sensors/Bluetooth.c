#include "Drivers/asclin.h"
#include "Drivers/gpt12.h"
#include "Sensors/Bluetooth.h"
#include <stdio.h>
#include <string.h>

void Init_Bluetooth()
{
	_init_uart0();	// bluetooth module use uart0 (RX : 14.1, TX : 14.0)
}

void setBluetoothName(char *name)
{
	char buf[100];
	int i =0;
	sprintf(buf, "AT+NAME%s", name);

	while(buf[i] != 0)
	{
		_out_uart0(buf[i]);
		i++;
	}

	for(i=0; i<9; i++) { buf[i] = _in_uart0(); }
	buf[i] = '\0';
	if (strcmp(buf, "OKsetname") == 0 || strcmp(buf, "OKname")  == 0 ||
	   strcmp(buf, "OKSetName") == 0) {
	  my_printf("Success to set NAME!\n");
	} else {
	  my_printf("Fail to set NAME!\n");
	}
	delay_ms(100);
}

void setBluetoothPswd(char *pswd)
{
	char buf[100];
	int i =0;
	sprintf(buf, "AT+PIN%s", pswd);

	while(buf[i] != 0)
	{
		_out_uart0(buf[i]);
		i++;
	}

	for(i=0; i<8; i++) { buf[i] = _in_uart0(); }
	buf[i] = '\0';
	if (strcmp(buf, "OKsetPIN") == 0 || strcmp(buf, "OKpin") == 0 ||
	   strcmp(buf, "OKSetPin") == 0) {
	  my_printf("Success to set PIN!\n");
	} else {
	  my_printf("Fail to set PIN!\n");
	}
    delay_ms(100);
}

char getBluetoothByte_Blocked()
{
	return _in_uart0();
}

char getBluetoothByte_nonBlocked()
{
	unsigned char ch = 0;
	int res;
	res = _poll_uart0(&ch);
	if(res == 1)
		return ch;
	else
		return -1;
}

void setBluetoothByte_Blocked(unsigned char ch)
{
	_out_uart0(ch);
}

void bl_printf(const char *fmt, ...)
{
	char buffer[128];
	char buffer2[128]; // add \r before \n
	char *ptr;
	va_list ap;

	va_start(ap, fmt);
	vsprintf(buffer, fmt, ap);
	va_end(ap);
	int j = 0;
	for (int i = 0; buffer[i]; i++) {
		if (buffer[i] == '\n') {
			buffer2[j++] = '\r';
			buffer2[j++] = buffer[i];
		} else {
			buffer2[j++] = buffer[i];
		}
	}
	buffer2[j] = '\0';

	for (ptr = buffer2; *ptr; ++ptr)
		setBluetoothByte_Blocked((const unsigned char) *ptr);
}
