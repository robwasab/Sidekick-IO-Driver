
#include "sidekickio.h"


void SidekickIO::config_layout_spim(
			enum SPI_MODE spi_mode,
			enum SPI_DATA_ORDER data_order) {

	Packet rsp = {0};
	size_t rsplen = 0;

	transfer_cmd(
		&rsp,
		&rsplen,
		CMD_CFG,
		"bbb",
		LAYOUT_CONFIG_GPIO,
		(uint8_t) spi_mode,
		(uint8_t) data_order);

	assert(SK_ERROR_NONE == rsp.header.error);
	printf("application layout: SPIM configured!\n");
}
