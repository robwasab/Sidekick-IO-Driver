#ifndef _DISPLAY_EPD_W21_SPI_
#define _DISPLAY_EPD_W21_SPI_

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#define isEPD_W21_BUSY   EPD_GPIO_readBusyLevel()
#define EPD_W21_RST_0    EPD_GPIO_assertResetLevel(false)
#define EPD_W21_RST_1    EPD_GPIO_assertResetLevel(true)

#ifdef __cplusplus
extern "C" {
#endif
	void EPD_GPIO_assertResetLevel(bool level);
	bool EPD_GPIO_readBusyLevel(void);

	void EPD_W21_WriteDATA(unsigned char data);
	void EPD_W21_WriteCMD(unsigned char command);
	void EPD_W21_WriteDataBuffer(const uint8_t* data, size_t len);
	void EPD_GPIO_Init(void);

	void delay_ms(uint32_t ms);
	void delay_init(void);
	void delay_s(uint32_t s);

#ifdef __cplusplus
}
#endif


#endif  //#ifndef _MCU_SPI_H_

/***********************************************************
						end file
***********************************************************/
