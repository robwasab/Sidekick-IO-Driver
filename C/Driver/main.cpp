#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <stdlib.h>
#include "sidekickio.h"

#define SK SidekickIO

void test_application(void)
{
	SidekickIO sidekick(SK::FW_MODE_APP);
	//sidekick.send_config_layout_gpio();
	sidekick.send_config_layout_spim(
		SK::SPI_MODE_0,
		SK::SPI_DATA_ORDER_MSB);

	sidekick.send_gpio_config(6,
		SK::GPIO_CONFIG_DIR_OUT,
		SK::GPIO_CONFIG_PULL_NONE);

	sidekick.send_gpio_config(2,
		SK::GPIO_CONFIG_DIR_OUT,
		SK::GPIO_CONFIG_PULL_NONE);

	bool level = true;
	sidekick.send_gpio_pin_set(2, level);

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

	printf("done\n");
}

void test_dfu(void)
{
	SidekickIO sidekick(SK::FW_MODE_DFU);

	//sidekick.test_dfu();

	sidekick.firmware_update_from_file(
		"C:\\Users\\robbytong\\Documents\\Sidekick\\IO\\Firmware\\Application\\helloworld\\Debug\\serial_bridge.bin",
		SK::FW_MODE_APP);
}

int main(void) {
	//test_application();
	test_dfu();
}
