
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

#include "sidekickio.h"

#define SK SidekickIO
#define SW_GPIO 0

int main(void)
{
	SidekickIO sidekick;

  sidekick.config_layout_gpio();

  sidekick.gpio_config(
		SW_GPIO,
		SK::GPIO_CONFIG_DIR_IN,
		SK::GPIO_CONFIG_PULL_UP);

  sidekick.gpio_enable_pin_intrpt(
		SW_GPIO,
		SK::GPIO_PIN_DETECTION_RISE,
		NULL,
		NULL);

  while(true) {
    sleep(1);

    printf("getting status...\n");
    if(sidekick.get_status() & SK::INTRPT_STATUS_GPIO_Msk) {

      printf("interrupt triggered!\n");
      if(sidekick.gpio_get_intrpt_status() & (1 << SW_GPIO))
      {
        printf("gpio interrupt!\n");

        sidekick.gpio_clr_intrpt_status((1 << SW_GPIO));
      }
    }
  }
  return 0;
}
