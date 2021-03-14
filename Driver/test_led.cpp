
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

#include "sidekickio.h"

#define SK SidekickIO

int main(void)
{
	SidekickIO sidekick;

  sidekick.config_layout_gpio();

  while(true) {
    static bool led = true;

    printf("toggling led: %s\n", led ? "on" : "off");
    sidekick.gpio_set_led(led);
    led ^= true;

    sleep(1);
  }
  return 0;
}
