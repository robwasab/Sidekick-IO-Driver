
#include "sidekickio.h"


void SidekickIO::config_layout_gpio(void) {
	Packet rsp = {0};
	size_t rsplen = 0;

	transfer_cmd(
		&rsp,
		&rsplen,
		CMD_CFG,
		"b",
		LAYOUT_CONFIG_GPIO);

	assert(SK_ERROR_NONE == rsp.header.error);

	if(mPrintFlag) {
		printf("application layout: gpio configured!\n");
	}
}


bool SidekickIO::gpio_config(
		uint8_t gpio_index, enum GPIO_CONFIG_DIR dir, enum GPIO_CONFIG_PULL pull)
{
	Packet rsp = {0};
	size_t rsplen = 0;

	transfer_cmd(
		&rsp,
		&rsplen,
		CMD_GPIO_CFG,
		"bbb",
		gpio_index,
		dir,
		pull);

	if(SK_ERROR_NONE == rsp.header.error) {
		if(mPrintFlag) {
			printf("gpio index: %d configured!\n", gpio_index);
		}
		return true;
	}
	else if(SK_ERROR_PARAMETER == rsp.header.error) {
		printf("gpio config failed due to parameter, check to see if pins are available in layout config");
		return false;
	}
	else {
		assert(false);
		return false;
	}

}


void SidekickIO::gpio_pin_set(
		uint8_t gpio_index, bool level)
{
	Packet rsp = {0};
	size_t rsplen = 0;

	transfer_cmd(
		&rsp,
		&rsplen,
		CMD_GPIO_PIN_SET,
		"bb",
		gpio_index,
		level);

	assert(SK_ERROR_NONE == rsp.header.error);
	if(mPrintFlag) {
		printf("gpio index: %d level set: %d!\n", gpio_index, level);
	}
}


bool SidekickIO::gpio_read(
		uint8_t gpio_index)
{
	Packet rsp = {0};
	size_t rsplen = 0;

	transfer_cmd(
		&rsp,
		&rsplen,
		CMD_GPIO_PIN_READ,
		"b",
		gpio_index);

	assert(SK_ERROR_NONE == rsp.header.error);
	assert(rsplen >= 1 + sizeof(PacketHeader));

	return rsp.data[0] > 0;
}


void SidekickIO::gpio_set_led(bool on)
{
	Packet rsp = {0};
	size_t rsplen = 0;

	transfer_cmd(
		&rsp,
		&rsplen,
		CMD_GPIO_SET_LED,
		"b",
		(uint8_t) !!on);

	assert(SK_ERROR_NONE == rsp.header.error);
}

void SidekickIO::gpio_get_intrpt_status(uint32_t * gpio_int_mask)
{
	assert(NULL != gpio_int_mask);

	Packet rsp = {0};
	size_t rsplen = 0;

	transfer_cmd(
		&rsp,
		&rsplen,
		CMD_GPIO_GET_INTRPT_STATUS,
		NULL);

	assert(SK_ERROR_NONE == rsp.header.error);
	assert(rsplen >= sizeof(uint32_t) + sizeof(PacketHeader));

	memcpy(gpio_int_mask, rsp.data, sizeof(uint32_t));

	if(mPrintFlag) {
		printf("got interrupt status mask: %04x!\n", *gpio_int_mask);
	}
}


uint32_t SidekickIO::gpio_get_intrpt_status(void)
{
	uint32_t mask = 0;
	gpio_get_intrpt_status(&mask);

	return mask;
}

void SidekickIO::gpio_clr_intrpt_status(uint32_t gpio_int_mask)
{
	Packet rsp = {0};
	size_t rsplen = 0;

	transfer_cmd(
		&rsp,
		&rsplen,
		CMD_GPIO_CLR_INTRPT_STATUS,
		"u",
		gpio_int_mask);

	assert(SK_ERROR_NONE == rsp.header.error);

	if(mPrintFlag) {
		printf("cleared interrupt status with: %04x!\n", gpio_int_mask);
	}
}


void SidekickIO::gpio_enable_pin_intrpt(
		uint8_t gpio_index,
		enum GPIO_PIN_DETECTION detection,
		GPIOHandler handler,
		void * obj)
{
	Packet rsp = {0};
	size_t rsplen = 0;

	transfer_cmd(
		&rsp,
		&rsplen,
		CMD_GPIO_ENA_PIN_INTRPT,
		"bb",
		gpio_index,
		(uint8_t) detection);

	assert(SK_ERROR_NONE == rsp.header.error);

	if(mPrintFlag) {
		printf("enabled pin interrupt: %d!\n", gpio_index);
	}

	mGPIOHandlers[gpio_index] = handler;
	mGPIOHandlersArgs[gpio_index] = obj;
}


void SidekickIO::gpio_disable_pin_intrpt(uint8_t gpio_index)
{
	Packet rsp = {0};
	size_t rsplen = 0;

	transfer_cmd(
		&rsp,
		&rsplen,
		CMD_GPIO_DIS_PIN_INTRPT,
		"b",
		gpio_index);

	assert(SK_ERROR_NONE == rsp.header.error);

	if(mPrintFlag) {
		printf("disabled pin interrupt: %d!\n", gpio_index);
	}

	mGPIOHandlers[gpio_index] = NULL;
	mGPIOHandlersArgs[gpio_index] = NULL;
}
