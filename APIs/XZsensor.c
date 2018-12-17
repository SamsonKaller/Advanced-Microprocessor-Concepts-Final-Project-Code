/* 
 * File:   XZsensor.c
 * Author: Samson
 *
 * Created on November 27, 2018, 1:56 PM
 */

#include <xc.h>
#include <string.h>
#include "XZsensor.h"
#include "i2c1.h"
#include "uart2.h"

/*
 *  Initializes INT0 to detect a HIGH from XZ sensor's DR pin.
 *  Parameters: None
 *  Return:     None
 */
void INT0_Initialize(void)
{
    TRISFbits.TRISF6 = 1;
    INTCON2bits.INT0EP = 0;
    IFS0bits.INT0IF = 0;
    IPC0bits.INT0IP = 1;
    IEC0bits.INT0IE = 1;
}

/*
 *  Writes a byte to XZ sensor at address ADDR using I2C.
 *  Parameters: - reg   address of XZ sensor register
 *              - data  data to write to register
 *  Return:     None
 */
void WriteByte(_wchar_t reg, char data)
{
    char data_w[2];
    
    data_w[0] = reg & 0xFF;
    data_w[1] = data & 0xFF;
    
    writeNI2C1(ADDR, data_w, 1);
}

/*
 *  Reads a byte from XZ sensor at address ADDR using I2C.
 *  Parameters: - reg   address of XZ sensor register
 *  Return:     None
 */
char ReadByte(_wchar_t reg)
{
    char data_w;
    char data_r;
    
    data_w = reg & 0xFF;
    
    writeNI2C1(ADDR, &data_w, 1);
    read1I2C1(ADDR, &data_r);
    return(data_r);
}

/*
 *  Initializes the XZ sensor. DRE reg is set to assert DR pin when coordinate
 *  data is available, and DRECFG reg is set to DR pin is active HIGH and DR is
 *  enabled.
 *  Parameters: None
 *  Return:     None
 */
void XZ_Initialize(void)
{
    WriteByte(DRE_REG, DRE_CFG);
    WriteByte(DRCFG_REG, DRCFG_CFG);
}

/*
 *  Initializes push buttons on Explorer16 as inputs.
 *  Parameters: None
 *  Return:     None
 */
void IO_Initialize(void)
{
    TRISDbits.TRISD6 = 1;       // pushbutton S3
    TRISDbits.TRISD13 = 1;      // pushbutton S4
    TRISAbits.TRISA7 = 1;       // pushbutton S5
}

/*
 *  Reads data from X Position register of XZ sensor.
 *  Parameters: None
 *  Return:     xData: X position from XZ sensor
 */
unsigned char readXcoord(void)
{
    unsigned char xData;
    xData = ReadByte(X_REG);
    return(xData);
}

/*
 *  Reads data from Z Position register of XZ sensor.
 *  Parameters: None
 *  Return:     zData: Z position from XZ sensor
 */
unsigned char readZcoord(void)
{
    unsigned char zData;
    zData = ReadByte(Z_REG);
    return(zData);
}

/*
 *  Determines cursor movement direction from XZ sensor coordinates and updates
 *  cursor position.
 *  Parameters: - xL: last X Position from XZ Sensor
 *              - zL: last Z Position from XZ Sensor
 *              - xCo: X Position from XZ Sensor
 *              - zCo: Z Position from XZ Sensor
 *              - pxCU: pointer to cursor X position
 *              - pzCu: pointer to cursor Z position
 */
void checkDir(unsigned char xL, unsigned char zL, unsigned char xCo, unsigned char zCo, unsigned char* pxCu, unsigned char* pzCu)
{
    // xDir: determines left or right cursor movement
    // zDir: determines up or down cursor movement
    int xDir = 0, zDir = 0;
    
    // first, if coordinates are same as the last then no movement,
    // then determines if received coordinates are within deadzone,
    // if not decides left, right, up, or down movement
    if(xCo == xL) xDir = NOMOVE;                                                          // same coords check
    else if((MIDPOINT - DEADZONE < xCo) && (xCo < MIDPOINT + DEADZONE)) xDir = NOMOVE;    // x deadzone check
    else xDir = (MIDPOINT > (int)xCo)? LEFT : RIGHT;                                     // left or right movement

    if(zCo == zL) zDir = NOMOVE;                                                          // same coords check
    else if((MIDPOINT - DEADZONE < zCo) && (zCo < MIDPOINT + DEADZONE)) zDir = NOMOVE;    // z deadzone check
    else zDir = ((int)zCo > MIDPOINT)? UP : DOWN;                                         // up or down movement 

    // moves cursor depending on value of xDir, only if within 0 to SCREEN_W boundaries
    if(xDir == LEFT && *pxCu > 0) *pxCu--;
    else if(xDir == RIGHT && *pxCu < SCREEN_W) *pxCu++;

    // moves cursor depending on value of zDir, only if within 0 to SCREEN_H boundaries
    if(zDir == UP && *pzCu > 0) *pzCu--;
    else if(zDir == DOWN && *pzCu < SCREEN_H) *pzCu++;
}

/*
 *  Outputs the cursor position to Tera Term via UART
 *  Parameters: - pBuff:    text buffer
 *              - z:        z coordinate of cursor
 *              - x:        x coordinate of cursor
 *  Return:     None
 */
void moveCursor(char* pBuff, unsigned char z, unsigned char x)
{
    sprintf(pBuff, "\033[%u;%uH", z, x);
    outString(pBuff);
}

/*
 *  Colors a pixel at the current cursor location.
 *  Parameters: None
 *  Return:     None
 */
void colorPix(void)
{
    outString("\xDB");
}

/*
 *  Erases a pixel at the current cursor location.
 *  Parameters: None
 *  Return:     None
 */
void erasePix(void)
{
    outString("\x20");
}

/*
 *  Clears the screen.
 *  Parameters: None
 *  Return:     None
 */
void clrScr(void)
{
    outString("\033[2J\033[HSystem initialized!\n\r");
}

