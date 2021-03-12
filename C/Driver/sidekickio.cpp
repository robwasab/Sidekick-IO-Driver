
#include "sidekickio.h"


#ifdef _MSC_VER


#include <windows.h>

// eww....
void usleep(int waitTime) {
	__int64 time1 = 0;
	__int64 time2 = 0;

	QueryPerformanceCounter((LARGE_INTEGER*)&time1);

	do {
		QueryPerformanceCounter((LARGE_INTEGER*)&time2);
	} while ((time2 - time1) < waitTime);
}

// Disable "Prefer enum class over enum"
#pragma warning(disable : 26812)


#else

#include <unistd.h>

#endif



static void usb_result(int error, const char * msg) {
	if(LIBUSB_SUCCESS > error) {
		fprintf(stderr, "%s, USB ERROR: %s\n", msg, libusb_error_name(error));
		assert(false);
		exit(-1);
	}
}



typedef libusb_device * libusb_device_ptr;


static void get_sidekick_interface(
		libusb_device_ptr dev,
		uint8_t * interface_no,
		uint8_t * write_ep,
		uint8_t * read_ep,
		uint8_t * notify_ep)
{
	assert(NULL != interface_no);
	assert(NULL != write_ep);
	assert(NULL != read_ep);
	assert(NULL != notify_ep);

	int res;

	struct libusb_config_descriptor * config;

	res = libusb_get_config_descriptor(dev, 0, &config);
	usb_result(res, "failed to get config descriptor");

	//printf("number of interfaces (bNumInterfaces): %d\n", config->bNumInterfaces);

	const struct libusb_interface * interface = NULL;

	const struct libusb_interface_descriptor * interface_descriptor = NULL;

	// Search for the vendor interface
	for(size_t k = 0; k < config->bNumInterfaces; k++) {
		interface = &config->interface[k];
		interface_descriptor = &interface->altsetting[0];

		uint8_t bInterfaceClass = interface_descriptor->bInterfaceClass;
		//uint8_t bInterfaceSubClass = interface_descriptor->bInterfaceSubClass;

		if(0xff == bInterfaceClass) {
			break;
		}
	}

	assert(NULL != interface);
	assert(NULL != interface_descriptor);

	*interface_no = interface_descriptor->bInterfaceNumber;

	const struct libusb_endpoint_descriptor * ep_arr = interface_descriptor->endpoint;
	*write_ep  = ep_arr[0].bEndpointAddress;
	*read_ep   = ep_arr[1].bEndpointAddress;
	*notify_ep = ep_arr[2].bEndpointAddress;

	//printf("write  ep: %02x\n", write_ep);
	//printf("read   ep: %02x\n", read_ep);
	//printf("notify ep: %02x\n", notify_ep);

	libusb_free_config_descriptor(config);
}


static libusb_device_handle * find_usb_device(
		libusb_context * context,
		uint16_t idVendor,
		uint16_t idProduct,
		uint8_t * out_interface_no,
		uint8_t * out_write_ep,
		uint8_t * out_read_ep,
		uint8_t * out_notify_ep)
{
	int res;
	// get the device list, which is a list of device pointers
	libusb_device_ptr * devices = NULL;

	// pass in a reference to our device list so the api can change its value
	ssize_t cnt = libusb_get_device_list(context, &devices);
	usb_result((int) cnt, "failed to get device list");

	assert(NULL != devices);

	ssize_t k;
	for(k = 0; k < cnt; k++) {
		struct libusb_device_descriptor desc;

		res = libusb_get_device_descriptor(devices[k], &desc);
		usb_result(res, "failed to get device descriptor");

		//printf("idVendor: %04x, idProduct: %04x\n", desc.idVendor, desc.idProduct);


		if(idVendor == desc.idVendor &&
			idProduct == desc.idProduct) {
			break;
		}
	}

	// if k made it to end of list, we didn't find anything...
	if(k == cnt) {
		printf("Did not find device...\n");
		return NULL;
	}


	// get a handle to the found device
	libusb_device_handle * handle = NULL;

	res = libusb_open(devices[k], &handle);
	usb_result(res, "failed to open device and get handle");
	assert(handle != NULL);

	get_sidekick_interface(devices[k],
			out_interface_no, out_write_ep, out_read_ep, out_notify_ep);

	libusb_free_device_list(devices, 1 /* unref all devices */);

	return handle;
}



static void initialize_sidekickio(
		libusb_device_handle * handle, uint8_t vendor_interface) {
	assert(NULL != handle);

	int res;

	res = libusb_reset_device(handle);
	usb_result(res, "failed to reset device");

	int config;
	res = libusb_get_configuration(handle, &config);
	usb_result(res, "failed to get configuration");

	//printf("current usb configuration is %d\n", config);

	// set the configuration to one and only configuration
	res = libusb_set_configuration(handle, 1);
	usb_result(res, "failed to set configuration to 1");

	res = libusb_claim_interface(handle, vendor_interface);
	usb_result(res, "failed to claim vendor interface");
}


static void send_reset(
		libusb_device_handle * handle, uint8_t vendor_interface) {
	// build a control request:
	// D7: 0 = Host to device
	// Type: vendor
	// Recipient: interface
	uint8_t bmRequestType = LIBUSB_REQUEST_TYPE_VENDOR |
		LIBUSB_RECIPIENT_INTERFACE;

	// Reset command
	uint8_t bRequest = 0x00;

	// Reset command has no additional values to specify
	uint16_t wValue = 0x00;

	// Specify interface here
	uint16_t wIndex = vendor_interface;

	// No data to transfer in data stage
	uint16_t wLength = 0x00;

	int res;

	res = libusb_control_transfer(
		handle,
		bmRequestType,
		bRequest,
		wValue,
		wIndex,
		NULL, // no data to send
		wLength,
		TIMEOUT_MS);

	usb_result(res, "failed to send application reset command");
}


static uint8_t get_firmware_mode(
		libusb_device_handle * handle, uint8_t vendor_interface) {
	// build a control request:
	// D7 Data transfer direction: (1) - device to host
	// D6 .. 5 Type: (2) - vendor
	// D4 .. 0 Recipient: (1) - Interface
	uint8_t bmRequestType =
		(1 << 7) | LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_RECIPIENT_INTERFACE;

	// Get firmware mode command
	uint8_t bRequest = 0x01;

	// no addional values to specify
	uint16_t wValue = 0x00;

	// Specify interface here
	uint16_t wIndex = vendor_interface;

	// No data to transfer in data stage
	uint8_t  rx_data[1] = {0};

	int res;

	res = libusb_control_transfer(
		handle,
		bmRequestType,
		bRequest,
		wValue,
		wIndex,
		rx_data,
		sizeof(rx_data),
		TIMEOUT_MS);

	usb_result(res, "failed to get firmware mode");

	//printf("firmware mode: %d", rx_data[0]);

	return rx_data[0];
}


static void send_data(
		libusb_device_handle * handle,
		uint8_t write_ep,
		uint8_t * data,
		size_t len)
{
	int num_transferred = 0;
	int res;

	res = libusb_bulk_transfer(
		handle,
		write_ep,
		data,
		len,
		&num_transferred,
		TIMEOUT_MS);
	usb_result(res, "bulk write transfer failed");
	assert(num_transferred == (int)len);
}


static void recv_data(
		libusb_device_handle * handle,
		uint8_t read_ep,
		uint8_t * data,
		size_t * len)
{
	int num_transferred = 0;
	int res;

	res = libusb_bulk_transfer(
		handle,
		read_ep,
		data,
		*len,
		&num_transferred,
		TIMEOUT_MS);
	usb_result(res, "bulk read transfer failed");

	//printf("recv_data len: %zu\n", *len);
	//printf("recv_data num: %u\n", num_transferred);

	assert(0 <= num_transferred);
	*len = (size_t) num_transferred;
}

static uint32_t get_status(libusb_device_handle * handle, uint8_t notify_ep)
{
	uint32_t status = 0;
	int num_transferred = 0;
	unsigned int timeout_ms = TIMEOUT_MS;
	int res;

	res = libusb_interrupt_transfer(
		handle,
		notify_ep,
		(uint8_t *)&status,
		sizeof(status),
		&num_transferred,
		timeout_ms);

	usb_result(res, "interrupt transfer failed");

	return status;
}

void SidekickIO::print_arr(const uint8_t * data, size_t len) {
	for(size_t k = 0; k < len; k++) {
		if(k < len - 1) {
			printf("%02x:", data[k]);
		}
		else {
			printf("%02x\n", data[k]);
		}
	}
}


const char * SidekickIO::fw_mode2str(enum FW_MODE mode) {
	switch(mode) {
		case FW_MODE_APP:
			return "APP";
		case FW_MODE_DFU:
			return "DFU";
		default:
			assert(false);
	}
	return "unimplemented";
}

void SidekickIO::init(enum FW_MODE mode)
{
	enum FW_MODE discovered_mode;
	bool sidekick_present = search_for_sidekick(&discovered_mode);

	if(false == sidekick_present) {
		printf("sidekick not present...\n");
		exit(-1);
	}

	if(mode != discovered_mode) {
		printf("incorrect mode: sidekick is in %s mode\n", fw_mode2str(discovered_mode));
	}
	else {
		//printf("correct mode: sidekick is in %s mode\n", fw_mode2str(discovered_mode));
	}

	mHandle = find_usb_device(
			mUSB,
			SK_VENDOR_ID,
			discovered_mode == FW_MODE_APP ? SK_PRODUCT_ID_APP : SK_PRODUCT_ID_DFU,
			&mInterface,
			&mWriteEP,
			&mReadEP,
			&mNotifyEP);

	if(NULL == mHandle) {
		printf("failed to find usb device\n");
		exit(-1);
	}

	// claim the interface, and set the usb configuration
	initialize_sidekickio(mHandle, mInterface);
	//printf("initialized sidekickio!\n");

	// reset the communication sequence number
	send_reset(mHandle, mInterface);
	//printf("reset seq no!\n");

	mSeqNo = true;

	mFWMode = (enum FW_MODE) discovered_mode;

	if(discovered_mode != mode) {
		//printf("sending dfu reset\n");
		reset_into_mode(mode);
	}
	else {
	}
}


SidekickIO::SidekickIO(enum FW_MODE mode) {
	mPrintFlag = false;
	memset(mGPIOHandlers, 0, sizeof(mGPIOHandlers));

	int ret;
	ret = libusb_init(&mUSB);
	usb_result(ret, "couldn't init libusb");

	assert(2 == sizeof(struct PacketHeader));
	assert(VENDOR_EP_SIZE == sizeof(Packet));

	mLastPollTime = time(NULL);
	mPollInterval = (time_t) 0.1;

	init(mode);
}


SidekickIO::~SidekickIO() {
	libusb_exit(mUSB);
	//printf("closed libusb!\n");
}


const char * SidekickIO::error2str(enum SK_ERROR error_code)
{
	/*
	static const char mError2Str[][32] = {
		[SK_ERROR_NONE]             = "NONE",
		[SK_ERROR_UNKNOWN_CMD]      = "UNKNOWN CMD",
		[SK_ERROR_NO_MEMORY]        = "NO MEMORY",
		[SK_ERROR_MALFORMED_PACKET] = "MALFORMED PACKET",
		[SK_ERROR_RESOURCE_BUSY]    = "RESOURCE BUSY",
		[SK_ERROR_PARAMETER]        = "PARAMETER",
		[SK_ERROR_MAX]              = "MAX",
	};
	*/
	//printf("SidekickIO Error: %s\n", mError2Str[error_code]);
	//return mError2Str[error_code];
	return NULL;
}

#define CMD_ARG_U08_FMT 'b'
#define CMD_ARG_U16_FMT 'h'
#define CMD_ARG_U32_FMT 'u'
#define CMD_ARG_ARR_FMT 'a'



void SidekickIO::transfer_cmd(
		Packet * rsp,
		size_t * rsplen,
		enum CMD cmd_type,
		const char fmt[],
		...)
{

	assert(NULL != rsp);
	Packet cmd = {0};


	cmd.header.seq_no = mSeqNo;
	cmd.header.cmd    = cmd_type;
	cmd.header.error  = 0x00;

	size_t cmd_datalen = 0;


	// If fmt string is NULL, then this command has
	// no args
	if(NULL != fmt) {
		//printf("copying args!\n");
		va_list args;
		va_start(args, fmt);

		#define COPY_ARG(T)                                      \
		do {                                                     \
			assert(cmd_datalen + sizeof(T) <= sizeof(cmd.data)); \
			T arg = (T) va_arg(args, unsigned int);              \
			memcpy(&cmd.data[cmd_datalen], &arg, sizeof(T));     \
			cmd_datalen += sizeof(T);                            \
		} while(0)


		// iterate over the format string
		for(size_t k = 0; fmt[k] != '\0'; k++) {
			switch(fmt[k]) {
				case CMD_ARG_U08_FMT: {
					COPY_ARG(uint8_t);
				} break;

				case CMD_ARG_U16_FMT: {
					COPY_ARG(uint16_t);
				} break;

				case CMD_ARG_U32_FMT: {
					COPY_ARG(uint32_t);
				} break;

				case CMD_ARG_ARR_FMT: {
					void * arr_ptr = va_arg(args, void *);
					size_t arr_len = va_arg(args, size_t);
					assert(cmd_datalen + arr_len <= sizeof(cmd.data));
					memcpy(&cmd.data[cmd_datalen], arr_ptr, arr_len);
					cmd_datalen += arr_len;
					//printf("arr: copied %d bytes\n", arr_len);
				} break;

				default:
					printf("unexpected command formatter\n");
					assert(false);
					exit(-1);
			}
		}
		va_end(args);
	}


	uint32_t k;

	for(k = 0; k < NUM_RETRIES; k++) {

		// Account for the header length
		cmd_datalen += sizeof(PacketHeader);

		// send the send buffer
		send_data(mHandle, mWriteEP, (uint8_t *)&cmd, cmd_datalen);

		// receive the response from usb
		*rsplen = sizeof(Packet);
		recv_data(mHandle, mReadEP, (uint8_t *)rsp, rsplen);


		if(*rsplen < 2) {
			printf("insufficient rsp data");
			continue;
		}

		if(cmd.header.cmd != rsp->header.cmd) {
			printf("cmd mismatch\n");
			continue;
		}

		// make sure sequence number agrees
		if(cmd.header.seq_no != rsp->header.seq_no) {
			printf("seq no mismatch\n");
			continue;
		}

		// increment sequence number
		mSeqNo ^= true;

		// make sure the error is within bounds
		assert(rsp->header.error < SK_ERROR_MAX);
		break;
	}

	if(k == NUM_RETRIES) {
		printf("exceeded maximum attempts\n");
		assert(false);
		exit(-1);
	}
}


uint8_t SidekickIO::get_fw_error_code(void)
{
	return mFWErrorCode;
}


void SidekickIO::send_dfu_start(uint32_t image_size, uint32_t crc)
{
	assert(FW_MODE_DFU == mFWMode);
	Packet rsp = {0};
	size_t rsplen = 0;

	transfer_cmd(
		&rsp,
		&rsplen,
		CMD_DFU_START,
		"uu",
		image_size,
		crc);
	assert(SK_ERROR_NONE == rsp.header.error);
}


uint32_t SidekickIO::send_dfu_write_data(
		const uint8_t * data, size_t len)
{
	assert(FW_MODE_DFU == mFWMode);
	assert(len <= MAX_PACKET_SIZE);

	Packet rsp = {0};
	size_t rsplen = 0;

	transfer_cmd(
		&rsp,
		&rsplen,
		CMD_DFU_WRITE_DATA,
		"a",
		data,
		len);
	assert(SK_ERROR_NONE == rsp.header.error);
	//printf("rsplen: %d\n", rsplen);
	assert(rsplen - sizeof(struct PacketHeader) == sizeof(uint32_t));

	uint32_t current_write_addr = 0;
	memcpy(&current_write_addr, rsp.data, sizeof(uint32_t));

	return current_write_addr;
}


void SidekickIO::send_dfu_read_data(
		uint8_t * data, uint8_t * len)
{
	assert(FW_MODE_DFU == mFWMode);
	assert(*len <= MAX_PACKET_SIZE);

	Packet rsp = {0};
	size_t rsplen = 0;

	transfer_cmd(
		&rsp,
		&rsplen,
		CMD_DFU_READ_DATA,
		"b",
		*len);

	assert(SK_ERROR_NONE == rsp.header.error);
	assert(rsplen - 2 <= 0xff);
	*len = (uint8_t)(rsplen - 2);
	memcpy(data, rsp.data, *len);
}


void SidekickIO::send_dfu_reset_read_ptr(void)
{
	assert(FW_MODE_DFU == mFWMode);
	Packet rsp = {0};
	size_t rsplen = 0;

	transfer_cmd(
		&rsp,
		&rsplen,
		CMD_DFU_RESET_READ_PTR,
		NULL);

	assert(rsp.header.error == SK_ERROR_NONE);
}


void SidekickIO::send_dfu_done_writing(void)
{
	assert(FW_MODE_DFU == mFWMode);
	Packet rsp = {0};
	size_t rsplen = 0;

	transfer_cmd(
		&rsp,
		&rsplen,
		CMD_DFU_DONE_WRITING,
		NULL);

	assert(rsp.header.error == SK_ERROR_NONE);
}


void SidekickIO::send_dfu_reset(enum FW_MODE mode)
{
	Packet rsp = {0};
	size_t rsplen = 0;

	transfer_cmd(
		&rsp,
		&rsplen,
		CMD_DFU_RESET,
		"b",
		mode);

	assert(rsp.header.error == SK_ERROR_NONE);
}

static void print_progress_bar(float fraction)
{
	#define RESOLUTION 50
	char buf[RESOLUTION + 1];

	uint8_t progress = (uint8_t)(fraction * RESOLUTION);

	progress = MIN(progress, RESOLUTION);

	uint8_t k = 0;

	for(k = 0; k < progress; k++) {
		buf[k] = '#';
	}

	for(; k < RESOLUTION; k++) {
		buf[k] = ' ';
	}

	buf[k] = '\0';

	printf("[%s] %03.1f\r", buf, fraction * 100.0);

	fflush(stdout);
}

void SidekickIO::test_dfu(void)
{
	size_t app_size = SK_APP_SIZE;

	printf("allocating fw memory...\n");
	uint8_t * fw_data = new uint8_t[app_size];

	printf("initializing fw data...\n");
	for(size_t k = 0; k < sizeof(fw_data); k++) {
		fw_data[k] = rand() % 0x100;
	}

	firmware_update(fw_data, app_size, FW_MODE_DFU);

	delete [] fw_data;
}


static void read_image(
		const char * fw_file, uint8_t ** out_ptr, size_t * out_len)
{
	FILE * file = fopen(fw_file, "rb");
	assert(NULL != file);

	// Determine fw image length
	fseek(file, 0, SEEK_END);
	size_t fw_len = ftell(file);

	// Reset the file pointer to beginning
	rewind(file);

	// allocate RAM to hold firmware
	uint8_t * fw_data = new uint8_t[fw_len];
	assert(NULL != fw_data);

	size_t num_read = fread(
		fw_data, sizeof(uint8_t), fw_len, file);

	assert(num_read == fw_len);

	*out_ptr = fw_data;
	*out_len = fw_len;
}


void SidekickIO::firmware_update_from_file(
		const char * fw_file, enum FW_MODE reset_mode)
{
	uint8_t * fw_data = NULL;
	size_t fw_len = 0;

	read_image(fw_file, &fw_data, &fw_len);

	firmware_update(fw_data, fw_len, reset_mode);

	delete [] fw_data;
}


void SidekickIO::firmware_update(
		const uint8_t * fw_data, size_t fw_data_len, enum FW_MODE reset_mode)
{
	// Total application size is 0x39C00 or 236544 bytes
	//size_t nvm_size = SK_NVM_SIZE;

	//printf("sending dfu start...\n");
	send_dfu_start((uint32_t) fw_data_len, 0);

	printf("writing data...\n");
	size_t ptr = 0;
	while(ptr < fw_data_len) {
		uint8_t num2send = (uint8_t) MIN(MAX_PACKET_SIZE, fw_data_len - ptr);

		//uint32_t current_addr = send_dfu_write_data(&fw_data[ptr], num2send);
		send_dfu_write_data(&fw_data[ptr], num2send);

		ptr += num2send;

		//printf("wrote: %x / %x\n", current_addr, app_size);
		print_progress_bar(((float)ptr)/fw_data_len);
	}

	printf("\n");

	printf("finalizing dfu...\n");
	send_dfu_done_writing();


	printf("sending reset read ptr...\n");
	send_dfu_reset_read_ptr();

	ptr = 0;
	uint8_t rdata[MAX_PACKET_SIZE];

	printf("reading data...\n");
	while(ptr < fw_data_len) {
		uint8_t num2read = (uint8_t) MIN(MAX_PACKET_SIZE, fw_data_len - ptr);

		//printf("attempting to read %d bytes...\n", num2read);

		send_dfu_read_data(rdata, &num2read);


		if(0 != memcmp(rdata, &fw_data[ptr], num2read)) {
			printf("fw read back failed...\n");
			printf("fw_data:\n");
			print_arr(&fw_data[ptr], num2read);
			printf("read data:\n");
			print_arr(rdata, num2read);
			break;
		}
		else {
			//printf("read %x pass!\n", ptr);
		}
		//printf("read %d bytes [%x / %x]...\n", num2read, ptr, app_size);
		ptr += num2read;
		print_progress_bar(((float)ptr) / fw_data_len);
	}
	printf("\n");

	printf("DFU done!\n");

	reset_into_mode(reset_mode);
}


void SidekickIO::reset_into_mode(enum FW_MODE mode)
{
	if(mode == mFWMode) {
		return;
	}

	printf("resetting sidekick from %s -> %s\n",
		fw_mode2str(mFWMode), fw_mode2str(mode));

	send_dfu_reset(mode);

	libusb_close(mHandle);

	bool success = wait_for_sidekick(mode, 10);

	if(false == success) {
		printf("couldn't force sidekick into %s mode...\n",
			mode == FW_MODE_DFU ? "DFU" : "APP");
		assert(false);
		exit(-1);
	}

	//printf("success! sidekick is (%s)!\n",
	//	fw_mode2str(mode));

	init(mode);
}


bool SidekickIO::search_for_sidekick(enum FW_MODE * fw_mode)
{
	int res;
	// get the device list, which is a list of device pointers
	libusb_device_ptr * devices = NULL;

	// pass in a reference to our device list so the api can change its value
	ssize_t cnt = libusb_get_device_list(mUSB, &devices);
	usb_result((int) cnt, "failed to get device list");

	assert(NULL != devices);

	ssize_t k;
	struct libusb_device_descriptor desc = {0};

	for(k = 0; k < cnt; k++) {
		res = libusb_get_device_descriptor(devices[k], &desc);
		usb_result(res, "failed to get device descriptor");

		//printf("idVendor: %04x, idProduct: %04x\n", desc.idVendor, desc.idProduct);

		if((SK_VENDOR_ID == desc.idVendor) && (
			(SK_PRODUCT_ID_APP == desc.idProduct) ||
			(SK_PRODUCT_ID_DFU == desc.idProduct))) {
			break;
		}
	}

	// if k made it to end of list, we didn't find anything...
	if(k == cnt) {
		return false;
	}

	if(SK_PRODUCT_ID_APP == desc.idProduct) {
		*fw_mode = FW_MODE_APP;
		return true;
	}
	else if(SK_PRODUCT_ID_DFU == desc.idProduct) {
		*fw_mode = FW_MODE_DFU;
		return true;
	}
	else {
		assert(false);
		return false;
	}
}


static void print_waiting(const char * msg)
{
	static uint8_t index = 0;
	char sprites[] = {'|', '/', '-', '\\'};

	printf("%s [%c]\r", msg, sprites[index]);
	fflush(stdout);

	index = (index + 1) % sizeof(sprites);
}


bool SidekickIO::wait_for_sidekick(enum FW_MODE mode, uint32_t timeout_sec)
{
	//printf("timeout_sec: %d\n", timeout_sec);
	for(uint8_t sec = 0; sec < timeout_sec * 10; sec++)
	{
		enum FW_MODE fw_mode;
		bool success = search_for_sidekick(&fw_mode);
		if(false == success) {
			//printf("not found...\n");
		}
		else if(fw_mode == mode){
			printf("\nfound sidekick in %s mode!\n", fw_mode2str(mode));
			return true;
		}
		else {
			// sidekick not in correct firmware mode
		}
		print_waiting("looking for sidekick...");
		usleep(100 * 1000);
	}
	printf("\nsidekick mode timeout...\n");
	return false;
}


void SidekickIO::main_loop_task(void)
{
	time_t current_time = time(NULL);

	if(current_time < mLastPollTime + mPollInterval) {
		return;
	}

	mLastPollTime = current_time;

	uint32_t status = get_status(mHandle, mNotifyEP);

	if(0 == status) {
		return;
	}

	//printf("interrupt: %04x\n", status);

	if(INTRPT_STATUS_GPIO_Msk & status) {
		// gpio interrupt
		uint32_t gpio_int_status = 0;

		gpio_get_intrpt_status(&gpio_int_status);

		//printf("gpio interrupt: %04x!\n", gpio_int_status);
		for(uint8_t k = 0; k < ARRAY_SIZE(mGPIOHandlers); k++)
		{
			if(mGPIOHandlers[k] != NULL) {
				mGPIOHandlers[k](this, k, mGPIOHandlersArgs[k]);
			}
		}

		// clear the interrupt
		gpio_clr_intrpt_status(gpio_int_status);

		//printf("gpio interrupt cleared!\n");
	}
}


void SidekickIO::send_echo(uint8_t * data, size_t len, bool * match) {
	assert(NULL != data);
	assert(NULL != match);

	Packet rsp = {0};
	size_t rsplen = 0;

	assert(len <= sizeof(rsp.data));

	transfer_cmd(
		&rsp,
		&rsplen,
		CMD_ECHO,
		"a",
		data, len);

	assert(SK_ERROR_NONE == rsp.header.error);
	assert(rsplen == len + sizeof(PacketHeader));

	// check if the echo matches
	*match = 0 == memcmp(data, rsp.data, len);

	//printf("echo match: %d\n", *match);
	//printf("echo data: "); print_arr(data, len);
}


void SidekickIO::test_echo(void) {
	uint8_t buf[VENDOR_EP_SIZE - sizeof(PacketHeader)] = {0};

	srand((unsigned int) time(NULL));

	for(int trial = 0; trial < 64; trial++) {

		for(size_t k = 0; k < sizeof(buf); k++) {
			buf[k] = rand() % 0xff;
		}

		bool match = false;
		send_echo(buf, sizeof(buf), &match);

		printf("trial %d: %s\n", trial, match ? "PASS" : "FAIL");

		if(false == match) {
			break;
		}
	}
}
