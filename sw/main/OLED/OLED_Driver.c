/******************************************************************************
***************************Intermediate driver layer***************************
* | file        :   OLED_Driver.c
* | version     :   V1.0
* | date        :   2017-11-09
* | function    :   SSD1327 Drive function
    
note:
Image scanning:
Please use progressive scanning to generate images or fonts         
******************************************************************************/
#include "OLED_Driver.h"
#include <stdio.h>

COLOR Buffer[OLED_WIDTH / 2 * OLED_HEIGHT];
OLED_DIS sOLED_DIS;

/*******************************************************************************
function:
        Common register initialization
*******************************************************************************/
static void OLED_InitReg(void)
{
    OLED_WriteReg(0xae);//--turn off oled panel

    OLED_WriteReg(0x15);    //   set column address
    OLED_WriteReg(0x00);    //  start column   0
    OLED_WriteReg((OLED_WIDTH/2)-1);    //  end column   127

    OLED_WriteReg(0x75);    //   set row address
    OLED_WriteReg(0x00);    //  start row   0
    OLED_WriteReg(OLED_HEIGHT-1);    //  end row   127

    OLED_WriteReg(0x81);  // set contrast control
    OLED_WriteReg(0x80);

    OLED_WriteReg(0xa0);    // gment remap
    OLED_WriteReg(0x51);   //51

    OLED_WriteReg(0xa1);  // start line
    OLED_WriteReg(0x00);

    OLED_WriteReg(0xa2);  // display offset
    OLED_WriteReg(0x00);

    OLED_WriteReg(0xa4);    // rmal display
    OLED_WriteReg(0xa8);    // set multiplex ratio
    OLED_WriteReg(0x7f);

    OLED_WriteReg(0xb1);  // set phase leghth
    OLED_WriteReg(0xf1);

    OLED_WriteReg(0xb3);  // set dclk
    OLED_WriteReg(0x00);  //80Hz:0xc1 90Hz:0xe1   100Hz:0x00   110Hz:0x30 120Hz:0x50   130Hz:0x70     01

    OLED_WriteReg(0xab);  //
    OLED_WriteReg(0x01);  //

    OLED_WriteReg(0xb6);  // set phase leghth
    OLED_WriteReg(0x0f);

    OLED_WriteReg(0xbe);
    OLED_WriteReg(0x0f);

    OLED_WriteReg(0xbc);
    OLED_WriteReg(0x08);

    OLED_WriteReg(0xd5);
    OLED_WriteReg(0x62);

    OLED_WriteReg(0xfd);
    OLED_WriteReg(0x12);

}

/********************************************************************************
function:   Set the display scan and color transfer modes
parameter:
        Scan_dir   :   Scan direction
        Colorchose :   RGB or GBR color format
********************************************************************************/
void OLED_SetGramScanWay(OLED_SCAN_DIR Scan_dir)
{
    //Get the screen scan direction
    sOLED_DIS.OLED_Scan_Dir = Scan_dir;

    //Get GRAM and OLED width and height
    if(Scan_dir == L2R_U2D || Scan_dir == L2R_D2U || Scan_dir == R2L_U2D || Scan_dir == R2L_D2U) {
        sOLED_DIS.OLED_Dis_Column   = OLED_WIDTH;
        sOLED_DIS.OLED_Dis_Page = OLED_HEIGHT;
        sOLED_DIS.OLED_X_Adjust = OLED_X;
        sOLED_DIS.OLED_Y_Adjust = OLED_Y;
    } else {
        sOLED_DIS.OLED_Dis_Column   = OLED_HEIGHT;
        sOLED_DIS.OLED_Dis_Page = OLED_WIDTH;
        sOLED_DIS.OLED_X_Adjust = OLED_Y;
        sOLED_DIS.OLED_Y_Adjust = OLED_X;
    }
}

/********************************************************************************
function:
            initialization
********************************************************************************/
void OLED_Init(OLED_SCAN_DIR OLED_ScanDir)
{
    //Hardware reset
    OLED_Reset();

    //Set the initialization register
    OLED_InitReg();

    //Set the display scan and color transfer modes
    OLED_SetGramScanWay(OLED_ScanDir );
    OLED_Delay_ms(200);

    //Turn on the OLED display
    OLED_WriteReg(0xAF);
}

/********************************************************************************
function:   Set the display Window(Xstart, Ystart, Xend, Yend)
parameter:
        xStart :   X direction Start coordinates
        Ystart :   Y direction Start coordinates
        Xend   :   X direction end coordinates
        Yend   :   Y direction end coordinates
********************************************************************************/
void OLED_SetWindow(POINT Xstart, POINT Ystart, POINT Xend, POINT Yend)
{
    Xstart &= ~1;
    Xend |= 1;
    if((Xstart > sOLED_DIS.OLED_Dis_Column - 1) || (Ystart > sOLED_DIS.OLED_Dis_Page - 1) ||
       (Xend > sOLED_DIS.OLED_Dis_Column - 1) || (Yend > sOLED_DIS.OLED_Dis_Page - 1))
        return;

    Xstart = Xstart / 2;
    Xend = Xend / 2;

    uint8_t set_col_address[] = {0x15, Xstart, Xend};
    OLED_WriteRegBlock(set_col_address, sizeof(set_col_address) / sizeof(uint8_t));

    uint8_t set_row_address[] = {0x75, Ystart, Yend};
    OLED_WriteRegBlock(set_row_address, sizeof(set_row_address) / sizeof(uint8_t));
}

/********************************************************************************
function:   Set show color
parameter:
        Color  :   Set show color,16-bit depth
********************************************************************************/
//static void OLED_SetColor(LENGTH Dis_Width, LENGTH Dis_Height, COLOR Color ){
void OLED_SetColor(POINT Xpoint, POINT Ypoint, COLOR Color)
{
    if(Xpoint > sOLED_DIS.OLED_Dis_Column - 1 || Ypoint > sOLED_DIS.OLED_Dis_Page - 1) {
        return;
    }
    unsigned buffer_stride = (sOLED_DIS.OLED_Dis_Column / 2);
    COLOR *p = &Buffer[Xpoint / 2 + Ypoint * buffer_stride];
    *p &= ~(0xf << (Xpoint & 1 ? 0 : 4));
    *p |= ((Color & 0xf) << (Xpoint & 1 ? 0 : 4));
}

/********************************************************************************
function:
            Clear screen
********************************************************************************/
void OLED_Clear(COLOR Color)
{
    unsigned int i,m;
    //OLED_SetWindow(0, 0, sOLED_DIS.OLED_Dis_Column, sOLED_DIS.OLED_Dis_Page);
    for(i = 0; i < sOLED_DIS.OLED_Dis_Page; i++) {
        for(m = 0; m < (sOLED_DIS.OLED_Dis_Column / 2); m++) {
            Buffer[i * (sOLED_DIS.OLED_Dis_Column / 2) + m] = (Color & 0xF) | ((Color & 0xF) << 4);
        }
    }
}

/********************************************************************************
function:   Update all memory to LCD
********************************************************************************/
void OLED_Display(void)
{
    OLED_SetWindow(0, 0, sOLED_DIS.OLED_Dis_Column - 1, sOLED_DIS.OLED_Dis_Page - 1);
    OLED_WriteDataBlock((uint8_t*)Buffer, sOLED_DIS.OLED_Dis_Page * sOLED_DIS.OLED_Dis_Column / 2);
}

/********************************************************************************
function:
            Clear Window
********************************************************************************/
void OLED_ClearWindow(POINT Xstart, POINT Ystart, POINT Xend, POINT Yend, COLOR Color)
{
    Xstart &= ~1;
    if (Xend % 2 == 0)
        Xend++;
    uint16_t i,m, Xpoint, Ypoint;
    Xpoint = (Xend - Xstart + 1) / 2;
    Ypoint = Yend - Ystart;
    
    uint16_t Num = Xstart + Ystart * (sOLED_DIS.OLED_Dis_Column / 2);
    for(i = 0; i < Ypoint; i++) {
        for(m = 0; m < Xpoint; m++) {
            Buffer[Num + m] = 0x00;
        }
        Num = Xstart + (Ystart + i + 1) * (sOLED_DIS.OLED_Dis_Column / 2);
    }
}

/********************************************************************************
function:   Update Window memory to LCD
********************************************************************************/
void OLED_DisWindow(POINT Xstart, POINT Ystart, POINT Xend, POINT Yend)
{
    OLED_SetWindow(Xstart, Ystart, Xend, Yend);
    Xstart &= ~1;
    Xend |= 1;
    unsigned buffer_stride = (sOLED_DIS.OLED_Dis_Column / 2);
    unsigned window_stride = (Xend - Xstart + 1)/2;
    unsigned y;
    for (y = Ystart; y <= Yend; y++) {
        COLOR *p = &Buffer[(Xstart / 2) + (y * buffer_stride)];
        OLED_WriteDataBlock(p, window_stride);
    }
}
