
#include "sidekickio.h"


void SidekickIO::send_config_layout_i2cm(
			enum I2CM_CLK_SEL clk_sel) {

	Packet rsp = {0};
	size_t rsplen = 0;

	transfer_cmd(
		&rsp,
		&rsplen,
		CMD_CFG,
		"bb",
		LAYOUT_CONFIG_I2C_MASTER,
		(uint8_t) clk_sel);

	assert(SK_ERROR_NONE == rsp.header.error);
	printf("application layout: I2CM configured!\n");
}


bool SidekickIO::analyze_i2cm_rsp(
		const uint8_t * rspdata,
		size_t rsplen,
		uint8_t * outdata,
		size_t  * outlen)
{
	struct RspHeader{
		uint8_t type;
		uint8_t sk_error;
		uint8_t asf_error;
		uint8_t data[];
	};

	const uint8_t * orig_rspdata = rspdata;
	size_t orig_rsplen = rsplen;

	size_t outmax = 0;

	if(NULL != outdata) {
		outmax = *outlen;
		*outlen = 0;
	}

	if(true == mPrintFlag) {
		print_arr(rspdata, rsplen);
	}

	while(0 < rsplen)
	{
		uint8_t len = rspdata[0];

		rspdata++;
		rsplen--;

		assert(len <= rsplen);
		assert(len >= sizeof(struct RspHeader));

		struct RspHeader * header =
			(struct RspHeader *) rspdata;

		size_t datalen = len - sizeof(struct RspHeader);

		rspdata += len;
		rsplen -= len;

		if(true == mPrintFlag) {
			printf("     type: %c\n"   , header->type);
			printf(" sk error: %02xh\n", header->sk_error);
			printf("asf error: %02xh\n", header->asf_error);
			printf("  datalen: %d\n"   , datalen);
		}

		// check if there was an error
		if(SidekickIO::SK_ERROR_NONE != header->sk_error) {
			mFWErrorCode = header->asf_error;

			printf("an error occurred...\n");

			// force a trace of the data by setting the print flag
			if(false == mPrintFlag) {
				mPrintFlag = true;
				analyze_i2cm_rsp(orig_rspdata, orig_rsplen, NULL, NULL);
				mPrintFlag = false;
			}
			return false;
		}

		// copy data to the output buffer
		if(NULL != outdata && *outlen < outmax) {
			size_t num2write = MIN(datalen, outmax - *outlen);
			memcpy(&outdata[*outlen],
				header->data, num2write);
			*outlen += num2write;
		}
	}

	return true;
}


bool SidekickIO::i2cm_write_data(
			uint8_t slave_addr,
			const uint8_t * write_data,
			size_t write_len) {

	Packet rsp = {0};
	size_t rsplen = 0;

	uint8_t i2c_fmt_str[] = {I2CM_CMD_WRITE_DATA, I2CM_CMD_STOP};

	transfer_cmd(
		&rsp,
		&rsplen,
		CMD_I2CM_TRANSACTION,
		"bbaba",
		slave_addr,                       // 'b'
		sizeof(i2c_fmt_str),              // 'b'
		i2c_fmt_str, sizeof(i2c_fmt_str), // 'a' i2c format string
		write_len,                        // 'b' data length byte
		write_data, write_len);           // 'a' data array

	return analyze_i2cm_rsp(
			rsp.data,
			rsplen - sizeof(PacketHeader),
			NULL,
			NULL);
}


bool SidekickIO::i2cm_read_data(
			uint8_t slave_addr,
			uint8_t * read_data,
			size_t  * read_len) {

	Packet rsp = {0};
	size_t rsplen = 0;

	uint8_t i2c_fmt_str[] = {I2CM_CMD_READ_DATA};

	transfer_cmd(
		&rsp,
		&rsplen,
		CMD_I2CM_TRANSACTION,
		"bbab",
		slave_addr,                       // 'b'
		sizeof(i2c_fmt_str),              // 'b'
		i2c_fmt_str, sizeof(i2c_fmt_str), // 'a' i2c format string
		*read_len);                        // 'b' data length byte

	return analyze_i2cm_rsp(
			rsp.data,
			rsplen - sizeof(PacketHeader),
			read_data,
			read_len);
}


bool SidekickIO::i2cm_write_register(
		uint8_t slave_addr,
		uint8_t reg,
		const uint8_t * data,
		size_t  len)
{
	Packet rsp = {0};
	size_t rsplen = 0;

	uint8_t i2c_fmt_str[] = {
			I2CM_CMD_WRITE_DATA,
			I2CM_CMD_STOP
	};

	//printf("CMD_I2CM_TRANSACTION: %02xh\n", CMD_I2CM_TRANSACTION);

	transfer_cmd(
		&rsp,
		&rsplen,
		CMD_I2CM_TRANSACTION,
		"bbabba",
		slave_addr,                       // 'b'
		sizeof(i2c_fmt_str),              // 'b'
		i2c_fmt_str, sizeof(i2c_fmt_str), // 'a' i2c format string
		len + 1,                          // 'b' length of array + register byte
		reg,                              // 'b' insert register byte at beginning
		data, len);                       // 'a' add the rest of the data

	return analyze_i2cm_rsp(
			rsp.data,
			rsplen - sizeof(PacketHeader),
			NULL,
			NULL);
}


bool SidekickIO::i2cm_write_register_byte(
		uint8_t slave_addr,
		uint8_t reg,
		uint8_t val)
{
	return i2cm_write_register(slave_addr, reg, &val, 1);
}


bool SidekickIO::i2cm_read_register(
		uint8_t slave_addr,
		uint8_t reg,
		uint8_t * read_data,
		size_t * read_len) {

	Packet rsp = {0};
	size_t rsplen = 0;

	uint8_t i2c_fmt_str[] = {
			I2CM_CMD_WRITE_BYTE,
			I2CM_CMD_READ_DATA,
			I2CM_CMD_STOP};

	transfer_cmd(
		&rsp,
		&rsplen,
		CMD_I2CM_TRANSACTION,
		"bbabb",
		slave_addr,                       // 'b'
		sizeof(i2c_fmt_str),              // 'b' length of fmt string
		i2c_fmt_str, sizeof(i2c_fmt_str), // 'a' i2c format string
		reg,                              // 'b' register value
		*read_len);                       // 'b' amount to read

	return analyze_i2cm_rsp(
			rsp.data,
			rsplen - sizeof(PacketHeader),
			read_data,
			read_len);
}


bool SidekickIO::i2cm_read_register_byte(
		uint8_t slave_addr,
		uint8_t reg,
		uint8_t * val)
{
	size_t read_amt = 1;
	return i2cm_read_register(slave_addr, reg, val, &read_amt);
}
