///////////////////////////////////////////////////////////////////////////////
//
//  Filename:       uart2.c
//
//  Lab #3a:        Interrupt Driven UART
//
//  Author:         Samson Kaller
//
//  Date:           September 18 2018
//
//  Description:    Library file for the pic24FJ128GA010 UART2 module
//
///////////////////////////////////////////////////////////////////////////////

#include "uart2.h"
#include <xc.h>

// global variables
char fifoBuffer[FIFO_SIZE];         // software FIFO buffer

// pointers to head and tail of software FIFO stack
char* pHead = fifoBuffer, * pTail = fifoBuffer;

///////////////////////////////////////////////////////////////////////////////
//  Name:           U2RXInterrupt
//  Description:    ISR for UART2 Rx Interrupt when a character enters the Rx
//                  buffer. The character is store in a software FIFO buffer.
///////////////////////////////////////////////////////////////////////////////
void ISR_NO_PSV _U2RXInterrupt()
{
    pHead++;            // increment head
    
    if(pHead >= fifoBuffer + FIFO_SIZE)     //
        pHead = fifoBuffer;                 // wrap head pointer around buffer
    
    // print error if new char overruns FIFO stack
    if(pHead == pTail)
        outString("\nSOFTWARE FIFO receive buffer overrun error\n");
    
    // save data in U2RXREG into FIFO buffer pointed by pHead
    else
        *pHead = U2RXREG;
    
    // clear UART2 Rx interrupt flag
    IFS1bits.U2RXIF = 0;
}

///////////////////////////////////////////////////////////////////////////////
//  Name:           UART2_Initialize
//  Description:    Initializes the UART2 Module
//      Accepts:    None
//      Returns:    None
///////////////////////////////////////////////////////////////////////////////
void UART2_Initialize(void) 
{
    // UART2 Mode Register
    U2MODE = 0x8800;        // bit 15: 1, UART2 Enable bit
                            // bit 11: 1, U2RTS pin in simplex mode
                            // bit  3: 0, BRG generates 16 clocks/bit period
                            // bit <2:1>: 00, 8-bit data, no parity
                            // bit  0: 0, one stop bit
    
    // UART2 Status and Control Register
    U2STA = 0x0000;         // clear register / reset settings
 
	// Set up your BRG register for Baud Rate = 19200
    U2BRG = 51;

	// clear transmit buffer 
    U2TXREG = 0x0000;
	
	// enable UART2 transmission 
    U2STAbits.UTXEN = 1;
    
    // interrupt flag set when any character is in the Rx buffer
    U2STAbits.URXISEL = 0;
    
    // clear UART2 Rx interrupt flag
    IFS1bits.U2RXIF = 0;
    
    // enable UART2 Rx Interrupt
    IEC1bits.U2RXIE = 1;
}

///////////////////////////////////////////////////////////////////////////////
//  Name:           outChar
//  Description:    Transmits a byte of data using UART2 module
//      Accepts:    - unsigned char c:  character to transmit
//      Returns:    None
///////////////////////////////////////////////////////////////////////////////
void outChar(unsigned char c)
{
    while (U2STAbits.UTXBF);        // wait if Tx buffer is full
    U2TXREG = c;                    // send byte
}

///////////////////////////////////////////////////////////////////////////////
//  Name:           inChar
//  Description:    Reads a byte of data from the software FIFO buffer
//      Accepts:    None
//      Returns:    - char* pTail   pointer to byte on bottom of FIFO stack
///////////////////////////////////////////////////////////////////////////////
unsigned char inChar(void)
{
	while(pHead == pTail);      // loop while buffer is empty
    
    // check for Rx buffer overrun error
    checkRxErrorUART2();

    pTail++;            // increment tail pointer
    
    if(pTail >= fifoBuffer + FIFO_SIZE)     //
        pTail = fifoBuffer;                 // wrap tail pointer around buffer
    
    return(*pTail);            // Implement return of received data to caller
}

///////////////////////////////////////////////////////////////////////////////
//  Name:           outString
//  Description:    Transmits a string using the UART2 module and outChar()
//      Accepts:    - const char* psz_s:    pointer to string
//      Returns:    None
///////////////////////////////////////////////////////////////////////////////
void outString(const char* psz_s) 
{
    // loop until null byte
    while(*psz_s != '\0')
    {
        outChar(*psz_s);        // transmit char
        psz_s++;                // increment pointer to next char
    }
}

///////////////////////////////////////////////////////////////////////////////
//  Name:           inString
//  Description:    Reads a string from the UART2 module using inChar and
//                  returns the size of the string
//      Accepts:    - char* psz_buff:               pointer to string buffer
//                  - unsigned int u16_maxCount:    max string size
//      Returns:    - unsigned int u16_i:           size of string
///////////////////////////////////////////////////////////////////////////////
unsigned int inString(char* psz_buff, unsigned int u16_maxCount) {
    
    // local variables
    unsigned char u8_c;     // input character from UART2 Rx buffer
    unsigned int u16_i;     // string index

    // for loop using inChar() to read string from UART2 Rx buffer
    for (u16_i = 0; u16_i < u16_maxCount; u16_i++) {
        u8_c = inChar();

        //terminate loop
        if (u8_c == '\n' ||u8_c == '\r' || u8_c == '\0')
            break;

        *psz_buff = u8_c; //save character
        psz_buff++;
    }
    
    //stop reading, terminate, return characters read.
    *psz_buff = '\0';
    return(u16_i);
}

///////////////////////////////////////////////////////////////////////////////
//  Name:           checkRxErrorUART2
//  Description:    checks UART2 OERR bit for Rx buffer overrun
//      Accepts:    None
//      Returns:    None
///////////////////////////////////////////////////////////////////////////////
void checkRxErrorUART2(void) 
{
	// Implement error checking on receive buffer overrun
    if (U2STAbits.OERR)
    {
        // clear error flag
		U2STAbits.OERR = 0;
        
        // error message
        outString("UART2 receive buffer overrun error\n");
    }
}