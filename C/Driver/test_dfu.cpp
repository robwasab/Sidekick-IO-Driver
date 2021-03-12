#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <stdlib.h>
#include "sidekickio.h"

#define SK SidekickIO

int main(void)
{
	SidekickIO sidekick(SK::FW_MODE_DFU);

	//sidekick.test_dfu();

	sidekick.firmware_update_from_file(
		"..\\..\\..\\..\\Firmware\\Application\\helloworld\\Debug\\serial_bridge.bin",
		SK::FW_MODE_APP);

	return 0;
}
