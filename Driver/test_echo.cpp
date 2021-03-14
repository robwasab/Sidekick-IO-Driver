
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <stdlib.h>
#include "sidekickio.h"

#define SK SidekickIO

int main(void)
{
	SidekickIO sidekick(SK::FW_MODE_APP);
	//sidekick.send_config_layout_gpio();
	sidekick.config_layout_spim(
		SK::SPI_MODE_0,
		SK::SPI_DATA_ORDER_MSB);

	sidekick.gpio_config(6,
		SK::GPIO_CONFIG_DIR_OUT,
		SK::GPIO_CONFIG_PULL_NONE);

	sidekick.gpio_config(2,
		SK::GPIO_CONFIG_DIR_OUT,
		SK::GPIO_CONFIG_PULL_NONE);

	//bool level = true;
	//sidekick.gpio_pin_set(2, level);

	time_t init_time = time(NULL);

	while(true) {
		if(init_time + 0.5 < time(NULL)) {
			init_time = time(NULL);
			/*
			level ^= true;
			sidekick.send_gpio_pin_set(2, level);
			printf("toggle!\n");
			*/
			sidekick.test_echo();
		}
		sidekick.main_loop_task();
	}
	return 0;
}
