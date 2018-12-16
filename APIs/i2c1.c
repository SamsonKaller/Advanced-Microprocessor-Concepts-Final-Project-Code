///////////////////////////////////////////////////////////////////////////////
//
//  Filename:       i2c1.c
//
//  Lab #4:         I2C with Range Sensor
//
//  Author:         Samson Kaller
//
//  Date:           October 23 2018
//
//  Description:    
//  These source code are created based on Explorer16 board PIC24 platform, using 
//  I2C1. It contains functions that perform basic general I2C operations and transactions. 
//  No device specific code should be added into this generic functions.
//
///////////////////////////////////////////////////////////////////////////////

#include "i2c1.h"
#include <xc.h>

///////////////////////////////////////////////////////////////////////////////
//      Name:       I2C1_Initialize
//
//  Operation:      Perform an I2C initialization.
//
//      Accepts:    - unsigned int uiSpeed_Khz:  desired speed of I2C communication
//
//      Returns:    None
///////////////////////////////////////////////////////////////////////////////
void I2C1_Initialize(unsigned int uiSpeed_Khz) 
{   
    // BRG = Fcy / Fscl - Fcy / 10,000,000 - 1 **Fscl must be converted to Hz**
    I2C1BRG = FCY/(uiSpeed_Khz*1000L) - FCY/10000000L - 1;
    
	// Initialiaze the I2C1 Peripherial for Master Mode,
	// 7-bit Slave Address and Slew Rate Control Disabled,
	// and leave the peripherial disabled
    I2C1CON = 0x0200;
   
    // Clear both receive and transmit buffers	
    I2C1RCV = 0x0000; 		
	I2C1TRN = 0x0000; 		
    
    // Enables the I2Cx module and configures the SDAx and SCLx pins as serial port pins
    I2C1CONbits.I2CEN = 1;	
}

///////////////////////////////////////////////////////////////////////////////
//      Name:       startI2C1
//  
//  Operation:      Perform an I2C start operation.
//
//      Accepts:    None
//
//      Returns:    None
///////////////////////////////////////////////////////////////////////////////
void startI2C1(void) 
{
    // initiate start on I2C
    I2C1CONbits.SEN = 1; 
    
    // wait until start finished
    while (I2C1CONbits.SEN);
}

///////////////////////////////////////////////////////////////////////////////
//      Name:       stopI2C1
//  
//  Operation:      Perform an I2C stop operation.
//
//      Accepts:    None
//
//      Returns:    None
///////////////////////////////////////////////////////////////////////////////
void stopI2C1(void) 
{
    // initiate stop condition on I2C
    I2C1CONbits.PEN=1;   
    
    // wait until stop finished
    while (I2C1CONbits.PEN);  
}

///////////////////////////////////////////////////////////////////////////////
//      Name:       putI2C1
//  
//  Operation:      Send one byte <cSendByte>, if NAK is returned use outString() 
//                  function to display the error via UART interface.
//
//      Accepts:    - char cSendByte:    The byte to be sent
//
//      Returns:    None
///////////////////////////////////////////////////////////////////////////////
void putI2C1(char cSendByte) 
{
    I2C1TRN = cSendByte; 
  
    // wait for 8bits+ack bit from slave to finish
    while (I2C1STATbits.TRSTAT);
    
    if (I2C1STATbits.ACKSTAT != I2C_ACK) 
    {
        //NAK returned
        outString("*** I2C1PUT, NAK returned ***");
    }
}

///////////////////////////////////////////////////////////////////////////////
//      Name:       getI2C1
//  
//  Operation:      Wait for a byte on the I2C bus, send <cAck2Send> as the 
//                  acknowledgement bit to be sent to the slave.
//                  The byte read will be passed as return value of the function.
//
//      Accepts:    - char cAck2Send:   acknowledgment bit
//
//      Returns:    - char cRcvByte:    byte read from I2C module
///////////////////////////////////////////////////////////////////////////////
char getI2C1(char cAck2Send) 
{
    char cRcvByte;

    //wait for idle condition, lower 5 bits must be 0
    while (I2C1CON & 0x1F);   
    
    // enable receiving
    I2C1CONbits.RCEN = 1;    
	while (!I2C1STATbits.RBF);		//wait for receive to complete     

	//read byte;
    cRcvByte = I2C1RCV;           
    
    //wait for idle condition before attempting ACK
    while (I2C1CON & 0x1F);         
    
    I2C1CONbits.ACKDT = cAck2Send; 
    I2C1CONbits.ACKEN = 1;          //enable ACKbit transmittion
    while (I2C1CONbits.ACKEN);      //wait for completion
    
    return(cRcvByte);                  
}

///////////////////////////////////////////////////////////////////////////////
//      Name:       write1I2C1
//  
//  Transaction:    Write 1 byte <cSendByte> to I2C slave at address <cAddr>
//
//      Accepts:    - char cAddr:       address of slave register
//                  - char cSendByte:   byte to be sent
//
//      Returns:    None
///////////////////////////////////////////////////////////////////////////////
void write1I2C1(char cAddr, char cSendByte) 
{
    startI2C1();
    putI2C1(I2C_WADDR(cAddr));
    putI2C1(cSendByte);
    stopI2C1();
}

///////////////////////////////////////////////////////////////////////////////
//      Name:       writeNI2C1
//  
//  Transaction:    Write <iCnt> number of bytes stored in buffer <pcArrayData>
//                  to I2C slave at address <cAddr>.
//
//      Accepts:    - char cAddr:           address of slave register
//                  - char* pcArrayData:    buffer holding bytes to be sent
//                  - int iCnt:             number of bytes
//
//      Returns:    None
///////////////////////////////////////////////////////////////////////////////
void writeNI2C1(char cAddr, char* pcArrayData, int iCnt) 
{
    int i;
    startI2C1();
    putI2C1(I2C_WADDR(cAddr));
    
    // for loop sends bytes one at a time
    for(i = 0; i < iCnt; i++) putI2C1(*(pcArrayData + i));
    
    stopI2C1();
}

///////////////////////////////////////////////////////////////////////////////
//      Name:       read1I2C1
//  
//  Transaction:    Read one byte from I2C slave at address <cAddr>, save to 
//                  pointer to location <pcRcvByte>.
//                  As per the I2C standard, a NAK is returned for the last byte
//                  read from the slave.
//
//      Accepts:    - char cAddr:           address of slave register
//                  - char* pcRcvByte:      buffer to store received bytes
//
//      Returns:    None
///////////////////////////////////////////////////////////////////////////////
void read1I2C1(char cAddr, char* pcRcvByte) 
{
    startI2C1();
    putI2C1(I2C_RADDR(cAddr));
    
    //last ack bit from master to slave during read must be a NAK
    *pcRcvByte = getI2C1(I2C_NAK); 
    
    stopI2C1();
}

///////////////////////////////////////////////////////////////////////////////
//      Name:       readNI2C1
//  
//  Transaction:    Read <iCnt> number of bytes from I2C slave at address <cAddr>, 
//                  save to buffer pointer <*pcArrayData>
//                  As per the I2C standard, a NAK is returned for the last byte
//                  read from the slave, while ACKs are returned for the other bytes.
//
//      Accepts:    - char cAddr:           address of slave register
//                  - char* pcArrayData:    buffer to store received bytes
//                  - int iCnt:             number of bytes
//
//      Returns:    None
///////////////////////////////////////////////////////////////////////////////
void readNI2C1(char cAddr, char* pcArrayData, int iCnt) 
{
    int i;
    startI2C1();
    putI2C1(I2C_RADDR(cAddr));
    
    for(i = 0; i < iCnt - 1; i++) *(pcArrayData + i) = getI2C1(I2C_ACK);
    
    *(pcArrayData + i) = getI2C1(I2C_NAK);
    stopI2C1();
}