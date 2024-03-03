#ifndef SENSORS_BLUETOOTH_H_
#define SENSORS_BLUETOOTH_H_

void Init_Bluetooth();
void setBluetoothName(char *name);
void setBluetoothPswd(char *pswd);

char getBluetoothByte_Blocked();
char getBluetoothByte_nonBlocked();
void setBluetoothByte_Blocked(unsigned char ch);
void bl_printf(const char *fmt, ...);

#endif /* SENSORS_BLUETOOTH_H_ */
