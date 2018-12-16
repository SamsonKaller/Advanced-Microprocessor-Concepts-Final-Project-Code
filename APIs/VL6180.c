///////////////////////////////////////////////////////////////////////////////
//
//  Filename:       VL6180.c
//
//  Lab #4:         I2C with Range Sensor
//
//  Author:         Samson Kaller
//
//  Date:           October 23 2018
//
//  Description:    
//  These source code are created based on Explorer16 board PIC24 platform, using 
//  I2C1. It contains functions that perform basic general VL6180 range sensor
//  operations and transactions. 
//  No device specific code should be added into this generic functions.
//
///////////////////////////////////////////////////////////////////////////////
#include <xc.h>
#include "VL6180.h"
#include "i2c1.h"

///////////////////////////////////////////////////////////////////////////////
//      Name:       WriteByte
//
//  Operation:      Writes one byte to the slave using I2C.
//
//      Accepts:    - wchar_t reg:      slave register address
//                  - char data:        data to send
//
//      Returns:    None
//
///////////////////////////////////////////////////////////////////////////////
void WriteByte(wchar_t reg, char data)
{
    char data_w[3];
    
    data_w[0] = (reg >> 8) & 0xFF;
    data_w[1] = reg & 0xFF;
    data_w[2] = data & 0xFF;
    
    writeNI2C1(ADDR, data_w, 3);
}

///////////////////////////////////////////////////////////////////////////////
//      Name:       ReadByte
//
//  Operation:      Read one byte from the slave using I2C
//
//      Accepts:    - wchar_t reg:      slave register address
//
//      Returns:    - char data_r:      Byte read from transaction
//
///////////////////////////////////////////////////////////////////////////////
char ReadByte(wchar_t reg)
{
    char data_w[2];
    char data_r[1];
    
    data_w[0] = (reg >> 8) & 0xFF;
    data_w[1] = reg & 0xFF;
    
    writeNI2C1(ADDR, data_w, 2);
    read1I2C1(ADDR, data_r);
    return(data_r[0]);
}

///////////////////////////////////////////////////////////////////////////////
//      Name:       initVL6180
//
//  Operation:      Initializes the VL6180 range sensor
//
//      Accepts:    None
//
//      Returns:    None
//
///////////////////////////////////////////////////////////////////////////////
void initVL6180(void)
{
    char reset;
    
    reset = ReadByte(0x016);
    
    if(reset == 1)
    {
        WriteByte(0x0207, 0x01);
        WriteByte(0x0208, 0x01);
        WriteByte(0x0096, 0x00);
        WriteByte(0x0097, 0xfd);
        WriteByte(0x00e3, 0x00);
        WriteByte(0x00e4, 0x04);
        WriteByte(0x00e5, 0x02);
        WriteByte(0x00e6, 0x01);
        WriteByte(0x00e7, 0x03);
        WriteByte(0x00f5, 0x02);
        WriteByte(0x00d9, 0x05);
        WriteByte(0x00db, 0xce);
        WriteByte(0x00dc, 0x03);
        WriteByte(0x00dd, 0xf8);
        WriteByte(0x009f, 0x00);
        WriteByte(0x00a3, 0x3c);
        WriteByte(0x00b7, 0x00);
        WriteByte(0x00bb, 0x3c);
        WriteByte(0x00b2, 0x09);
        WriteByte(0x00ca, 0x09);
        WriteByte(0x0198, 0x01);
        WriteByte(0x01b0, 0x17);
        WriteByte(0x01ad, 0x00);
        WriteByte(0x00ff, 0x05);
        WriteByte(0x0100, 0x05);
        WriteByte(0x0199, 0x05);
        WriteByte(0x01a6, 0x1b);
        WriteByte(0x01ac, 0x3e);
        WriteByte(0x01a7, 0x1f);
        WriteByte(0x0030, 0x00);
        
        WriteByte(0x0011, 0x10);    // Enables polling for ?New Sample ready?
                                    // when measurement completes
        WriteByte(0x010a, 0x30);    // Set the averaging sample period
                                    // (compromise between lower noise and
                                    // increased execution time)
        WriteByte(0x003f, 0x46);    // Sets the light and dark gain (upper
                                    // nibble). Dark gain should not be
                                    // changed.
        WriteByte(0x0031, 0xFF);    // sets the # of range measurements after
                                    // which auto calibration of system is
                                    // performed
        WriteByte(0x0040, 0x63);    // Set ALS integration time to 100ms
        
        WriteByte(0x002e, 0x01);    // perform a single temperature calibration
                                    // of the ranging sensor
        
        WriteByte(0x016, 0x00);
    }
}

///////////////////////////////////////////////////////////////////////////////
//      Name:       StartRange
//
//  Operation:      Start range measurement
//
//      Accepts:    None
//
//      Returns:    None
//
///////////////////////////////////////////////////////////////////////////////
void StartRange(void)
{
    WriteByte(0x018, 0x01);
}

///////////////////////////////////////////////////////////////////////////////
//      Name:       PollRange
//
//  Operation:      Polls the range (Function did not work in lab)
//
//      Accepts:    None
//
//      Returns:    None
//
///////////////////////////////////////////////////////////////////////////////
void PollRange(void)
{
    char status;
    char range_status;
    
    do
    {
        status = ReadByte(0x04F);
        range_status = status & 0x07;
    }
    while (range_status != 0x04);
}

///////////////////////////////////////////////////////////////////////////////
//      Name:       ReadRange
//
//  Operation:      Uses I2C to read the range reported by the VL6180 sensor.
//
//      Accepts:    None
//
//      Returns:    - unsigned char ReadRange:      Range data read from sensor
//
///////////////////////////////////////////////////////////////////////////////
unsigned char ReadRange(void)
{
    return(ReadByte(0x062));
}

///////////////////////////////////////////////////////////////////////////////
//      Name:       ClearInterrupts
//
//  Operation:      Clears VL6180 interrupts.
//
//      Accepts:    None
//
//      Returns:    None
//
///////////////////////////////////////////////////////////////////////////////
void ClearInterrupts(void)
{
    WriteByte(0x015, 0x07);
}