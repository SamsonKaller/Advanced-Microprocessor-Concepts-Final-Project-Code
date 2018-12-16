/**
 * Filename	: VL6180.h
 * 
 * Author	: Samson Kaller
 * 
 * Descriptions:
 * This header file is created based on Explorer16 board PIC24 platform, using 
 * I2C1. It contains declaration of of basic general VL6180 range sensor operations
 * and transactions, so as to define related constants and macro.
 * 
**/

#ifndef VL6180_H
#define	VL6180_H

// Address: 0x29 (7 bits) => 0b010 1001 + W/R bit => 0b0101 0010 => 0x52
#define ADDR    0x52    //0b0101 0010

#define wchar_t  unsigned int

void WriteByte(wchar_t reg, char data);
char ReadByte(wchar_t reg); 
void initVL6180(void);
void StartRange(void);
unsigned char ReadRange(void);
void ClearInterrupts(void);

#endif	/* VL6180_H */

