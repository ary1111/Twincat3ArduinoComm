/*
*****************************************************
\author: Adam Ryason
\version    1.1
\date        03/2019
\bug	        None yet
\brief	    This project reads the data from the actuators and logs it to a csv file

*****************************************************
*/

#include <iostream>
#include <fstream>      //needed for streaming to file
#include <sstream>     //needed for log title
#include <ctime>         //needed for time & date (file name creation)
#include "Windows.h"  //needed for float

//ADS Source. Use path where these files are installed. Remember to include the ADS lib in Linker->Input->Additional Dependencies.
#include "C:\TwinCAT\AdsApi\TcAdsDll\Include\TcAdsDef.h"
#include "C:\TwinCAT\AdsApi\TcAdsDll\Include\TcAdsAPI.h"

typedef struct MotorComm{
    FLOAT PX;
    FLOAT PY;
    FLOAT PZ;
    FLOAT PHI;
    FLOAT PSI;
    FLOAT FX;
    FLOAT FY;
    FLOAT FZ;
    FLOAT TY;
};

//ADS Variables
long      nErr, nPort;
AmsAddr   Addr;
PAmsAddr  pAddr;
ULONG     lHdlVar;
MotorComm nData;
char      szVar[] = { "Motors.ADS_data.MotorComm" };

//Variables
bool run=true;

void		InitADS();

int main()
{
    time_t now = time(0);
    tm *ltm = localtime(&now);
    std::ofstream LogFile;
    std::stringstream Filename;
    Filename << "log_" << ltm->tm_year << "_" << (1 + ltm->tm_mon) << "_" << ltm->tm_mday << "_" << ltm->tm_min <<".csv";
    LogFile.open(Filename.str());
    /*Initialize the ADS comm*/
    InitADS();

    while (run)
    {
        //Read from ADS
        nErr = AdsSyncReadReq(pAddr, ADSIGRP_SYM_VALBYHND, lHdlVar, sizeof(nData), &nData);

        if (nErr)
            std::cerr << "Error: AdsSyncReadReq: " << nErr << '\n';
        else
        {
            LogFile << nData.PX << "," << nData.PY << "," << nData.PZ << ",";
            LogFile << nData.PHI << "," << nData.PSI << ",";
            LogFile << nData.FX << "," << nData.FY << "," << nData.FZ << "," << nData.TY << std::endl;
        }
    }
}

void InitADS()
{
    pAddr = &Addr;

    // Open communication port on the ADS router
    nPort = AdsPortOpen();
    nErr = AdsGetLocalAddress(pAddr);
    if (nErr) std::cerr << "Error: AdsGetLocalAddress: " << nErr << '\n';
    pAddr->port = 350;

    // Get the handle of the C++-variable <szVar> 
    nErr = AdsSyncReadWriteReq(pAddr, ADSIGRP_SYM_HNDBYNAME, 0x0, sizeof(lHdlVar), &lHdlVar, sizeof(szVar), szVar);
    if (nErr) std::cerr << "Error: AdsSyncReadWriteReq: " << nErr << '\n';
}