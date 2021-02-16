#ifndef __SIDEKICKIO_H__
#define __SIDEKICKIO_H__

#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <stdlib.h>
#include "libusb.h"


//#define VENDOR_INTERFACE     2
#define VENDOR_EP_SIZE       64 // bytes
#define MAX_PACKET_SIZE      (VENDOR_EP_SIZE - 2)

//#define VENDOR_WRITE_DATA_EP 0x04
//#define VENDOR_READ_DATA_EP  0x05 | (1 << 7) // masked in 0x80 because it's an IN endpoint
//#define VENDOR_NOTIFY_EP     0x06 | (1 << 7) // masked in 0x80 because it's an IN endpoint
#define TIMEOUT_MS           1000
#define NUM_RETRIES          1000

#define SK_VENDOR_ID         0x03eb
#define SK_PRODUCT_ID_APP    0xfffe
#define SK_PRODUCT_ID_DFU    0xffff

#define SK_NVM_SIZE          0x40000
#define SK_APP_SIZE          0x39C00


#define MIN(a,b)	((a) > (b) ? (b) : (a))

class SidekickIO {
public:
	// Main Sidekick IO commands
	enum CMD {
		CMD_ECHO          = 0x01,
		CMD_CFG           = 0x02,
		CMD_GPIO_CFG      = 0x03,
		CMD_GPIO_PIN_SET  = 0x04,
		CMD_GPIO_PIN_READ = 0x05,
		CMD_GPIO_GET_INTRPT_STATUS = 0x06,
		CMD_GPIO_CLR_INTRPT_STATUS = 0x07,
		CMD_GPIO_ENA_PIN_INTRPT = 0x08,
		CMD_GPIO_WRITE_PARALLEL = 0x09,
		CMD_SPIM_TRANSFER_DATA  = 0x0A,
    CMD_DFU_START = 0x0B,
    CMD_DFU_WRITE_DATA = 0x0C,
    CMD_DFU_READ_DATA  = 0x0D,
		CMD_DFU_RESET_READ_PTR = 0x0E,
		CMD_DFU_DONE_WRITING = 0x0F,
		CMD_DFU_RESET = 0x10,
	};


	// Error codes
	// Unfortuanately, the name ERROR is already taken by libusb
	enum SK_ERROR {
		SK_ERROR_NONE             = 0x00,
		SK_ERROR_UNKNOWN_CMD      = 0x01,
		SK_ERROR_NO_MEMORY        = 0x02,
		SK_ERROR_MALFORMED_PACKET = 0x03,
		SK_ERROR_RESOURCE_BUSY    = 0x04,
		SK_ERROR_PARAMETER        = 0x05,
		SK_ERROR_STATE            = 0x06,
		SK_ERROR_OPERATION_FAILED = 0x07,
		SK_ERROR_MAX,
	};


	enum LAYOUT_CONFIG {
		LAYOUT_CONFIG_GPIO = 0x00,
		LAYOUT_CONFIG_SPI_MASTER = 0x03,
	};


	enum GPIO_CONFIG_DIR {
		GPIO_CONFIG_DIR_IN  = 0x00,
		GPIO_CONFIG_DIR_OUT = 0x01,
	};


	enum GPIO_CONFIG_PULL {
		GPIO_CONFIG_PULL_NONE = 0x00,
		GPIO_CONFIG_PULL_UP   = 0x01,
		GPIO_CONFIG_PULL_DOWN = 0x02,
	};


	enum INTRPT_STATUS_Msk {
		INTRPT_STATUS_GPIO_Msk = (1 << 0),
		INTRPT_STATUS_SPI_Msk  = (1 << 1),
	};

	enum FW_MODE {
		FW_MODE_APP = 0x00,
		FW_MODE_DFU = 0x01,
		FW_MODE_MAX,
	};


	struct PacketHeader {
		uint8_t seq_no:1;
		uint8_t cmd:7;
		uint8_t error;
	} __attribute__((packed));


	struct Packet {
		struct PacketHeader header;
		uint8_t data[VENDOR_EP_SIZE - sizeof(PacketHeader)];
	} __attribute__((packed));


	SidekickIO(enum FW_MODE mode);
	~SidekickIO(void);


	void transfer_cmd(
		Packet * rsp, size_t * rsplen, enum CMD cmd, const char fmt[], ...);


	const char * error2str(enum SK_ERROR error_code);

	void send_config_layout_gpio(void);

	enum SPI_MODE {
		SPI_MODE_0 = 0x00,
		SPI_MODE_1 = 0x01,
		SPI_MODE_2 = 0x02,
		SPI_MODE_3 = 0x03,
	};

	enum SPI_DATA_ORDER {
		SPI_DATA_ORDER_MSB = 0x00,
		SPI_DATA_ORDER_LSB = 0x01,
	};

	void send_config_layout_spim(
			enum SPI_MODE spi_mode = SPI_MODE_0,
			enum SPI_DATA_ORDER data_order = SPI_DATA_ORDER_MSB);

	void send_gpio_config(uint8_t gpio_index, enum GPIO_CONFIG_DIR dir,
		enum GPIO_CONFIG_PULL pull);

	void send_gpio_pin_set(uint8_t gpio_index, bool level);
	void send_gpio_read(uint8_t gpio_index, bool * level);
	void send_gpio_get_intrpt_status(uint32_t * gpio_int_mask);
	void send_gpio_clr_intrpt_status(uint32_t gpio_int_mask);
	void send_gpio_enable_pin_intrpt(uint8_t gpio_index);

	void send_echo(uint8_t * data, size_t len, bool * match);

	void query_mode(enum FW_MODE * mode);

	void test_echo(void);

	void main_loop_task(void);

	void test_dfu(void);

	void reset_into_mode(enum FW_MODE mode);

	void firmware_update(
		const uint8_t * fw_data, size_t fw_data_len, enum FW_MODE reset_mode);

	void firmware_update_from_file(
		const char * fw_file, enum FW_MODE reset_mode);

private:
	libusb_context * mUSB;
	libusb_device_handle * mHandle;
	bool mSeqNo;
	time_t mLastPollTime;
	time_t mPollInterval;

	uint8_t mInterface;
	uint8_t mWriteEP;
	uint8_t mReadEP;
	uint8_t mNotifyEP;

	enum FW_MODE mFWMode;

	const char * fw_mode2str(enum FW_MODE mode);

	void init(enum FW_MODE mode);
	void send_dfu_start(uint32_t image_size, uint32_t crc);
	uint32_t send_dfu_write_data(const uint8_t * data, size_t len);
	void send_dfu_read_data(uint8_t * data, uint8_t * len);
	void send_dfu_reset_read_ptr(void);
	void send_dfu_done_writing(void);
	void send_dfu_reset(enum FW_MODE mode);

	bool search_for_sidekick(enum FW_MODE * fw_mode);
	bool wait_for_sidekick(enum FW_MODE mode, uint32_t timeout_sec);
};

#endif
