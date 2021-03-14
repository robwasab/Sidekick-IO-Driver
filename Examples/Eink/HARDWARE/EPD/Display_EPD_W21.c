#include "Display_EPD_W21_spi.h"
#include "Display_EPD_W21.h"
#include <stdio.h>
#include <string.h>

void delay_xms(unsigned int xms)
{
	delay_ms(xms);
}

void EPD_W21_Init(void)
{
	EPD_W21_RST_0;		// Module reset
	delay_xms(10);//At least 10ms delay
	EPD_W21_RST_1;
	delay_xms(10);//At least 10ms delay

}

void EPD_Display(unsigned char *Image)
{
	unsigned int Width, Height;
	Width = (EPD_WIDTH % 8 == 0)? (EPD_WIDTH / 8 ): (EPD_WIDTH / 8 + 1);
	Height = EPD_HEIGHT;

	printf("epd display begin...\n");

	EPD_W21_WriteCMD(0x10);
	/*
	for (j = 0; j < Height; j++) {
		for ( i = 0; i < Width; i++) {
			EPD_W21_WriteDATA(0x00);
		}
	}
	*/
	uint8_t * zeros = (uint8_t *) malloc(Height * Width);
	memset(zeros, 0, Height * Width);
	EPD_W21_WriteDataBuffer(zeros, Height * Width);

	free(zeros);


	EPD_W21_WriteCMD(0x13);
	/*
	for ( j = 0; j < Height; j++) {
		for ( i = 0; i < Width; i++) {
			EPD_W21_WriteDATA(Image[i + j * Width]);
		}
	}
	*/
	EPD_W21_WriteDataBuffer(Image, Height * Width);

	EPD_W21_WriteCMD(0x12);			//DISPLAY REFRESH
	delay_xms(1);	    //!!!The delay here is necessary, 200uS at least!!!
	lcd_chkstatus();

	printf("epd display end...\n");
}


//UC8151C
void EPD_init(void)
{
		printf("epd init begin...\n");
		EPD_W21_Init(); //Electronic paper IC reset

		EPD_W21_WriteCMD(0x06);     //boost soft start
		EPD_W21_WriteDATA (0x17);		//A
		EPD_W21_WriteDATA (0x17);		//B
		EPD_W21_WriteDATA (0x17);		//C

		EPD_W21_WriteCMD(0x04);  //Power on
		lcd_chkstatus();        //waiting for the electronic paper IC to release the idle signal

		EPD_W21_WriteCMD(0x00);			 //panel setting
		EPD_W21_WriteDATA(0x1f);		 //LUT from OTP
	  EPD_W21_WriteDATA(0x0d);     //VCOM to 0V fast,This data is necessary, please do not delete!!!

		EPD_W21_WriteCMD(0x61);			//resolution setting
		EPD_W21_WriteDATA (0x80);  //128
		EPD_W21_WriteDATA (0x01);	//296
		EPD_W21_WriteDATA (0x28);

		EPD_W21_WriteCMD(0X50);			//VCOM AND DATA INTERVAL SETTING
		EPD_W21_WriteDATA(0x97);		//WBmode:VBDF 17|D7 VBDW 97 VBDB 57		WBRmode:VBDF F7 VBDW 77 VBDB 37  VBDR B7
		printf("epd init end...\n");
}

void EPD_sleep(void)
{
		EPD_W21_WriteCMD(0X50);  //VCOM AND DATA INTERVAL SETTING
		EPD_W21_WriteDATA(0xf7); //WBmode:VBDF 17|D7 VBDW 97 VBDB 57		WBRmode:VBDF F7 VBDW 77 VBDB 37  VBDR B7

		EPD_W21_WriteCMD(0X02);  	//power off
	  lcd_chkstatus();          //waiting for the electronic paper IC to release the idle signal
		EPD_W21_WriteCMD(0X07);  	//deep sleep
		EPD_W21_WriteDATA(0xA5);
}



void PIC_display(const unsigned char* picData)
{
	//Write Data
	EPD_W21_WriteCMD(0x10);	       //Transfer old data

	uint8_t blank[4736] = {0};
	memset(blank, 0xff, sizeof(blank));

	/*
	for(i=0;i<4736;i++) {
		EPD_W21_WriteDATA(0xff);
	}
	*/
	EPD_W21_WriteDataBuffer(blank, 4736);


	EPD_W21_WriteCMD(0x13);		     //Transfer new data

	/*
	for(i=0;i<4736;i++) {
		EPD_W21_WriteDATA(*picData);  //Transfer the actual displayed data
		picData++;
	}
	*/
	EPD_W21_WriteDataBuffer(picData, 4736);

	//Refresh
	EPD_W21_WriteCMD(0x12);		//DISPLAY REFRESH
	delay_xms(1);	  //!!!The delay here is necessary, 200uS at least!!!
	lcd_chkstatus();          //waiting for the electronic paper IC to release the idle signal
}


void PIC_display_Clear(void)
{
	uint8_t * buf = (uint8_t *) malloc(4736);
	//Write Data
	EPD_W21_WriteCMD(0x10);	       //Transfer old data
	/*
	for(i=0;i<4736;i++) {
		EPD_W21_WriteDATA(0xFF);
	}
	*/

	memset(buf, 0xff, 4736);
	EPD_W21_WriteDataBuffer(buf, 4736);


	EPD_W21_WriteCMD(0x13);		     //Transfer new data

	/*
	for(i=0;i<4736;i++) {
		EPD_W21_WriteDATA(0xFF);  //Transfer the actual displayed data
	}
	*/
	memset(buf, 0xff, 4736);
	EPD_W21_WriteDataBuffer(buf, 4736);


	//Refresh
	EPD_W21_WriteCMD(0x12);		//DISPLAY REFRESH
	delay_xms(1);	             //!!!The delay here is necessary, 200uS at least!!!
	lcd_chkstatus();          //waiting for the electronic paper IC to release the idle signal

	free(buf);
}


void lcd_chkstatus(void)
{
	printf("begin chkstatus...\n");
	unsigned char busy;
	do
	{
		EPD_W21_WriteCMD(0x71);
		busy = isEPD_W21_BUSY;
		busy =!(busy & 0x01);
	}
	while(busy);
	delay_xms(200);
	printf("end chkstatus...\n");
}














/***********************************************************
						end file
***********************************************************/
