/**
 * Filename	: I2C1.H
 * 
 * Author	: Day Yann Fong (2018)
 * 
 * Descriptions:
 * This header file is created based on Explorer16 board PIC24 platform, using 
 * I2C1. It contains declaration of of basic general I2C operations and 
 * transactions, so as to define related constants and macro.
 * 
**/

#ifndef _I2C1_H
#define _I2C1_H

// FCY instruction clock macro
#define FCY 16000000L   // Fcy = 16MHz

// define type of I2C acknowledgments
#define I2C_ACK 0
#define I2C_NAK 1

// macro to setup 7 bit device address and R/W option
#define I2C_WADDR(x) (x & 0xFE) //clear R/W bit of I2C addr
#define I2C_RADDR(x) (x | 0x01) //set R/W bit of I2C addr

// I2C Operations
void I2C1_Initialize(unsigned int uiSpeed_Khz);
void startI2C1(void);
void stopI2C1(void);
void putI2C1(char cSendByte);
char getI2C1(char cAck2Send);

// I2C Transactions
void write1I2C1(char cAddr, char cSendByte);
void writeNI2C1(char cAddr, char* pcArraydata, int iCnt); 
void read1I2C1(char cAddr, char* pcRcvByte);
void readNI2C1(char cAddr, char* pcArrayData, int iCnt);

#endif //_I2C1_H


