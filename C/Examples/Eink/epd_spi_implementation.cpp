
#include "sidekickio.h"
#include "Display_EPD_W21_spi.h"


#ifdef _MSC_VER

#include <windows.h>

// eww....
static void usleep(int waitTime) {
    __int64 time1 = 0;
    __int64 time2 = 0;

    QueryPerformanceCounter((LARGE_INTEGER*)&time1);

    do {
        QueryPerformanceCounter((LARGE_INTEGER*)&time2);
    } while ((time2 - time1) < waitTime);
}


static void sleep(uint32_t sec)
{
    Sleep(sec * 1000);
}

#else

// get usleep() from unistd
#include <unistd.h>

#endif


#define SK SidekickIO

SidekickIO sidekick;

#define EPD_EINK_CS   5
#define EPD_DATA_CMD  6
#define EPD_SRAM_CS   7
#define EPD_RESET     8
#define EPD_BUSY      9

void delay_init(void)
{

}

void delay_ms(uint32_t ms)
{
  usleep(ms * 1000);
}

void delay_s(uint32_t s)
{
  printf("sleeping for %d seconds...\n", s);
  sleep(s);
}

bool EPD_GPIO_readBusyLevel(void)
{
  return sidekick.gpio_read(EPD_BUSY);
}

void EPD_GPIO_assertResetLevel(bool level)
{
  sidekick.gpio_pin_set(EPD_RESET, level);
}


void EPD_GPIO_Init(void)
{
  printf("config spim...\n");
  sidekick.config_layout_spim(SK::SPI_MODE_0);

  #define CONFIG_AS_OUTPUT_WITH_LEVEL(pin, level) \
  do {\
    sidekick.gpio_config(\
      pin,\
      SK::GPIO_CONFIG_DIR_OUT,\
      SK::GPIO_CONFIG_PULL_NONE);\
      sidekick.gpio_pin_set(pin, level);\
  } while(0)


  printf("config output...\n");
  CONFIG_AS_OUTPUT_WITH_LEVEL(EPD_EINK_CS , true);
  printf("config output...\n");
  CONFIG_AS_OUTPUT_WITH_LEVEL(EPD_DATA_CMD, true);
  printf("config output...\n");
  CONFIG_AS_OUTPUT_WITH_LEVEL(EPD_SRAM_CS , true);
  printf("config output...\n");
  CONFIG_AS_OUTPUT_WITH_LEVEL(EPD_RESET   , true);

  printf("config input...\n");
  sidekick.gpio_config(
    EPD_BUSY,
    SK::GPIO_CONFIG_DIR_IN,
    SK::GPIO_CONFIG_PULL_UP);
}


void EPD_W21_WriteCMD(unsigned char command)
{
  sidekick.gpio_pin_set(EPD_DATA_CMD, false);
  sidekick.spim_write_byte(EPD_EINK_CS, command);
}


void EPD_W21_WriteDATA(unsigned char data)
{
  sidekick.gpio_pin_set(EPD_DATA_CMD, true);
  sidekick.spim_write_byte(EPD_EINK_CS, data);
}


void EPD_W21_WriteDataBuffer(const uint8_t * data, size_t len)
{
  sidekick.gpio_pin_set(EPD_DATA_CMD, true);

  sidekick.spim_write(EPD_EINK_CS, data, len);
}
