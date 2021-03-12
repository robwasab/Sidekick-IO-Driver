
#include "sidekickio.h"


void SidekickIO::config_layout_spim(
			enum SPI_MODE spi_mode,
			enum SPI_DATA_ORDER data_order,
			uint32_t baudrate) {

	Packet rsp = {0};
	size_t rsplen = 0;

	transfer_cmd(
		&rsp,
		&rsplen,
		CMD_CFG,
		"bbbu",
		LAYOUT_CONFIG_SPI_MASTER,
		(uint8_t) spi_mode,
		(uint8_t) data_order,
		(uint32_t) baudrate);

	assert(SK_ERROR_NONE == rsp.header.error);
	printf("application layout: SPIM configured!\n");
}


bool SidekickIO::spim_transfer_packet(uint8_t cs_index, uint8_t * buf, size_t len)
{
	Packet rsp = {0};
	size_t rsplen = 0;

	transfer_cmd(
		&rsp,
		&rsplen,
		CMD_SPIM_TRANSFER_DATA,
		"ba",
		cs_index,  // 'b'
		buf, len); // 'a'

	switch(rsp.header.error) {
		case SK_ERROR_NONE:
			assert(rsplen - sizeof(PacketHeader) == len);
			memcpy(buf, rsp.data, len);
			return true;

		case SK_ERROR_PARAMETER:
			printf("invalid spi index\n");
			return false;

		case SK_ERROR_RESOURCE_BUSY: {
			// spi was busy
			assert(1 <= rsplen - sizeof(PacketHeader));
			uint8_t fw_error_code = rsp.data[0];
			printf("spi error occured: %02xh\n", fw_error_code);
		} return false;

		default:
			assert(false);
			return false;
	}
}


bool SidekickIO::spim_transfer(uint8_t cs_index, uint8_t * data, size_t len)
{
	// minus one because the packet includes the chip select index
	uint8_t buf[MAX_PACKET_SIZE - 1];
	size_t offset = 0;

	while(len > 0)
	{
		size_t num2write = MIN(len, sizeof(buf));

		memcpy(buf, &data[offset], num2write);

		size_t foo = num2write;

		bool success =
			spim_transfer_packet(cs_index, buf, foo);

		if(false == success) {
			return false;
		}

		memcpy(&data[offset], buf, foo);

		len -= num2write;
		offset += num2write;
	}

	return true;
}

bool SidekickIO::spim_write(uint8_t cs_index, const uint8_t * data, size_t len)
{
	// minus one because the packet includes the chip select index
	uint8_t buf[MAX_PACKET_SIZE - 1];
	size_t offset = 0;

	while(len > 0)
	{
		size_t num2write = MIN(len, sizeof(buf));

		memcpy(buf, &data[offset], num2write);

		bool success =
			spim_transfer_packet(cs_index, buf, num2write);

		if(false == success) {
			return false;
		}

		len -= num2write;
		offset += num2write;
	}
	return true;
}


bool SidekickIO::spim_write_byte(uint8_t cs_index, uint8_t val)
{
	return spim_transfer_packet(cs_index, &val, 1);
}
