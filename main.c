/* 
 * File:   main.c
 * Author: Samson
 *
 * Created on November 27, 2018, 1:30 PM
 */

/**
  Section: Included Files
*/
#include <xc.h>
#include <stdio.h>
#include "mcc_generated_files/system.h"
#include "APIs/XZsensor.h"
#include "APIs/i2c1.h"
#include "APIs/uart2.h"

/*
                         Main application
 */
int main(void)
{
    // enum for movement macros
    enum{NOMOVE, LEFT, RIGHT, UP, DOWN};
    
    // InitMsg: string with clearscreen and cursor home commands + Initialization message
    // txtBuff: char array to hold string to output to Tera Term
    char InitMsg[32] = "\033[2J\033[HSystem initialized!\n\r";
    char txtBuff[64] = "";
    
    // xCoord: X Position from XZ Sensor
    // zCoord: Z Position from XZ Sensor
    // LastX: last X Position from XZ Sensor
    // LastZ: last Z Position from XZ Sensor
    // xScreen: cursor X position in Tera Term window, initialized to middle
    // zScreen: cursor Z position in Tera Term window, initialized to middle
    // xDir: determines left or right cursor movement
    // zDir: determines up or down cursor movement
    unsigned char xCoord = 0, zCoord = 0, LastX, LastZ, xCursor = 92, zCursor = 39;
    int xDir, zDir;
    
    // initialize the device
    SYSTEM_Initialize();
    UART2_Initialize();
    I2C1_Initialize(400);       // I2C baud rate = 400kHz
    XZ_Initialize();
//    INT0_Initialize();        // DR pin did not work with interrupt, unused
    
    outString(InitMsg);
    
    while (1)
    {
        // delay to control speed at which cursor moves
        // FCY = 16000000, delay = FCY/10 = 100ms
        __delay32(FCY/10);
        
        // save previous coordinates
        LastX = xCoord;
        LastZ = zCoord;
        
        // read x-z values from sensor
        xCoord = readX();
        zCoord = readZ();
        
        // first, if coordinates are same as the last then no movement
        // determines if received coordinates are within deadzone,
        // if not decides left, right, up, or down movement
        if(xCoord == LastX) xDir = NOMOVE;                                                          // same coords check
        else if((MIDPOINT - DEADZONE < xCoord) && (xCoord < MIDPOINT + DEADZONE)) xDir = NOMOVE;    // x deadzone check
        else xDir = (MIDPOINT > (int)xCoord )? LEFT : RIGHT;                                        // left or right movement
    
        if(zCoord == LastZ) zDir = NOMOVE;                                                          // same coords check
        else if((MIDPOINT - DEADZONE < zCoord) && (zCoord < MIDPOINT + DEADZONE)) zDir = NOMOVE;    // z deadzone check
        else zDir = ((int)zCoord > MIDPOINT)? UP : DOWN;                                            // up or down movement 
        
        // moves cursor depending on value of xDir, only if within 0 to SCREEN_W boundaries
        if(xDir == LEFT && xCursor > 0) xCursor--;
        else if(xDir == RIGHT && xCursor < SCREEN_W) xCursor++;
        
        // moves cursor depending on value of zDir, only if within 0 to SCREEN_H boundaries
        if(zDir == UP && zCursor > 0) zCursor--;
        else if(zDir == DOWN && zCursor < SCREEN_H) zCursor++;
        
        // outputs cursor position to Tera Term window
        moveCursor(txtBuff, zCursor, xCursor);

        // polls push buttons for action
        if(!S4) colorPix();     // draw at cursor
        if(!S5) erasePix();     // erase at cursor
        if(!S3) clrScr();       // clears the screen
        
        // debugging: displays X-Z positions from XZ sensor
        // (does not work when outputting cursor position to screen)
//        sprintf(txtBuff, "\t X position: %3u \t Z position: %3u \t\r", xCoord, zCoord);
//        outString(txtBuff);
    }

    return 1;
}

/*
 * ISR for INT0 interrupt, finally unused in application
 */
//void ISR_NO_PSV _INT0Interrupt(void)
//{
//    xCoord = ReadByte(X_REG);
//    zCoord = ReadByte(Z_REG);
//    IFS0bits.INT0IF = 0;
//}



