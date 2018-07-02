/*
*****************************************************
\author:
		Adam Ryason
		Melih Turkseven		

\version    1.0
\date        06/2018
\bug	        None yet
\read		Arduino Serial Port
\write		TwinCat3 ADS
\brief	    This project reads from the serial port of an Arduino and transmits the data
				over ADS (Automation Device Specification) to be read by a TwinCat3 Real-Time System

				This example reads the value of a digital, incremental encoder through an Arduino with a
				Robogaia Encoder Plate Shield to be used as a reference within an automation system (TC3)
\features
				- Single axis (can be expanded up to 3)

\credit		SerialPort code adapted from github:manashmndl
*****************************************************
*/
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SerialPort.h"
#include "Windows.h" 

//ADS Source. Use path where these files are installed. Remember to include the ADS lib.
#include "C:\TwinCAT\AdsApi\TcAdsDll\Include\TcAdsDef.h"
#include "C:\TwinCAT\AdsApi\TcAdsDll\Include\TcAdsAPI.h"

typedef struct ArduinoComm{
	FLOAT EncoderValue;
};

//ADS Variables
long      nErr, nPort;
AmsAddr   Addr;
PAmsAddr  pAddr;
ULONG     lHdlVar;
ArduinoComm nData;

double getTime();

int frequency = 50;											//Hz
double period = 1000 / (double)frequency;		//ms
double currentTime = 0.0;
double startTime = 0.0;
double lastTime = 0.0;

/*port_name must contain these backslashes, and remember to
replace the following com port*/
char *port_name = "\\\\.\\COM8";
char *val; char *val2; char *delimiter = ",";
std::string str;
int data[2]; int i = 0;
int prevdata;
//String for incoming data
char incomingData[MAX_DATA_LENGTH_IN];
char *myChar = "?";
int main()
{
	pAddr = &Addr;
	char      szVar[] = { "Motors.ADS_data.ArduinoComm" };

	// Open communication port on the ADS router
	nPort = AdsPortOpen();
	nErr = AdsGetLocalAddress(pAddr);
	if (nErr) std::cerr << "Error: AdsGetLocalAddress: " << nErr << '\n';
	pAddr->port = 350;

	// Get the handle of the C++-variable <szVar> 
	nErr = AdsSyncReadWriteReq(pAddr, ADSIGRP_SYM_HNDBYNAME, 0x0, sizeof(lHdlVar), &lHdlVar, sizeof(szVar), szVar);
	if (nErr) std::cerr << "Error: AdsSyncReadWriteReq: " << nErr << '\n';

	SerialPort arduino(port_name);
	if (arduino.isConnected()) std::cout << "Connection Established" << std::endl;
	else std::cout << "ERROR, check port name";

	//We collect a value to throw away
	arduino.writeSerialPort(myChar, MAX_DATA_LENGTH_OUT);
	int read_result = arduino.readSerialPort(incomingData, MAX_DATA_LENGTH_IN);

	for (val = strtok_s(incomingData, delimiter, &val2); val != NULL; val = strtok_s(NULL, delimiter, &val2)) {
		data[i] = (int)atof(val); i++;
	}
	i = 0;
	Sleep(1000);

	//We then collect the first value for prevdata
	currentTime = (getTime() - startTime);
	arduino.writeSerialPort(myChar, MAX_DATA_LENGTH_OUT);
	read_result = arduino.readSerialPort(incomingData, MAX_DATA_LENGTH_IN);

	for (val = strtok_s(incomingData, delimiter, &val2); val != NULL; val = strtok_s(NULL, delimiter, &val2)) {
		data[i] = (int)atof(val); i++;
	}
	i = 0;
	lastTime = currentTime;
	prevdata = data[0];

	while (arduino.isConnected() && nErr == NULL) {
		currentTime = (getTime() - startTime);
		if (currentTime >= period + lastTime)
		{
			//Check if data has been read or not
			arduino.writeSerialPort(myChar, MAX_DATA_LENGTH_OUT);
			read_result = arduino.readSerialPort(incomingData, MAX_DATA_LENGTH_IN);

			for (val = strtok_s(incomingData, delimiter, &val2); val != NULL; val = strtok_s(NULL, delimiter, &val2)) {
				data[i] = (int)atof(val); i++;
			}
			i = 0;
			
			if (std::abs(prevdata - data[0]) > 20)
				nData.EncoderValue = prevdata;
			else
			{
				nData.EncoderValue = data[0];
				prevdata = data[0];
			}

			/*Prints out data, uncomment to debug*/	
			//std::cout << read_result << ',' << data[0] << ',' << data[1] << ',' << sizeof(incomingData) << std::endl;
			//std::cout << data[0] << std::endl;
			//std::cout << nData.EncoderValue << std::endl;
			
			nErr = AdsSyncWriteReq(pAddr, ADSIGRP_SYM_VALBYHND, lHdlVar, sizeof(nData), &nData);
			if (nErr) std::cerr << "Error: AdsSyncWriteReq: " << nErr << '\n';

			lastTime = currentTime;
		}
	}
}
double getTime()
{
	LARGE_INTEGER timeCounts, timeFrequency;
	QueryPerformanceCounter(&timeCounts);
	QueryPerformanceFrequency(&timeFrequency);
	return double(timeCounts.QuadPart) / double(timeFrequency.QuadPart) * 1000;
}