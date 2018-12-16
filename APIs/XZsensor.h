/* 
 * File:   XZsensor.h
 * Author: Samson
 *
 * Created on November 27, 2018, 1:56 PM
 */

#ifndef XZSENSOR_H
#define	XZSENSOR_H

////////////////////////////// Application Macros //////////////////////////////
#define ISR_NO_PSV  __attribute__((__interrupt__, no_auto_psv))
#define _wchar_t    unsigned int

// I2C address of XZ sensor
#define ADDR        0x20

// Register addresses of XZ sensor
#define STATUS_REG  0x00
#define DRE_REG     0x01
#define DRCFG_REG   0x02
#define X_REG       0x08
#define Z_REG       0x0A

// hex config values for DRE and DRCFG registers of XZ sensor
#define DRE_CFG     0x02
#define DRCFG_CFG   0x81

// push button ports for read operations
#define S3          PORTDbits.RD6
#define S4          PORTDbits.RD13
#define S5          PORTAbits.RA7

// screen dimension used in main()
#define SCREEN_H    79
#define SCREEN_W    184
#define MIDPOINT    120
#define DEADZONE    20

////////////////////////////// Function prototypes /////////////////////////////
void INT0_Initialize(void);
void WriteByte(_wchar_t reg, char data);
char ReadByte(_wchar_t reg);
void XZ_Initialize(void);
unsigned char readX(void);
unsigned char readZ(void);
void colorPix(void);
void erasePix(void);
void ClrScr(void);

#endif	/* XZSENSOR_H */