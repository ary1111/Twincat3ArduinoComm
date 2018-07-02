#ifndef SERIALPORT_H
#define SERIALPORT_H

#define ARDUINO_WAIT_TIME 2000
#define MAX_DATA_LENGTH_IN 11
#define MAX_DATA_LENGTH_OUT 2

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

class SerialPort
{
public:
	SerialPort(char *portName);
	~SerialPort();

	int readSerialPort(char *buffer, unsigned int buf_size);
	bool writeSerialPort(char *buffer, unsigned int buf_size);
	bool isConnected();

private:
	HANDLE handler;
	bool connected;
	COMSTAT status;
	DWORD errors;
};

#endif // SERIALPORT_H
//view rawSerialPort.h hosted with ❤ by GitHub

