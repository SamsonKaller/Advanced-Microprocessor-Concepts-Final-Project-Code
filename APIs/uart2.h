///////////////////////////////////////////////////////////////////////////////
//
//  Filename:       uart2.h
//
//  Lab #3b:        Interrupt Driven UART
//
//  Author:         Samson Kaller
//
//  Date:           September 18 2018
//
//  Description:    Header file for the uart2.c library file
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _UART2_H
#define _UART2_H

/////////////////////////////////// MACROS ///////////////////////////////////

// use interrupt no_auto_psv attribute to reduce interrupt overhead
#define ISR_NO_PSV __attribute__((__interrupt__, no_auto_psv))

#define FIFO_SIZE   16      // max software FIFO stack size

#define ONESEC    16000000UL

///////////////////////////// FUNCTION PROTOTYPES /////////////////////////////

void UART2_Initialize(void);

unsigned char inChar(void);  
unsigned int inString(char* psz_buff, unsigned int u16_maxCount);

void outChar(unsigned char u8_c);
void outString(const char* psz_s);

void checkRxErrorUART2(void);

#endif  // _UART2_H

