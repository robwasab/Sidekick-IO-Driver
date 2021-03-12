import libusb as usb
from enum import Enum
from ctypes import *
import random
import struct
import pdb
import sys
from time import sleep, time
from threading import Thread, Lock


from libusb_tools import check_usb_result, find_usb_device

CMD_ECHO          = 0x01
CMD_CFG           = 0x02
CMD_GPIO_CFG      = 0x03
CMD_GPIO_PIN_SET  = 0x04
CMD_GPIO_PIN_READ = 0x05
CMD_GPIO_GET_INTERRUPT_STATUS   = 0x06
CMD_GPIO_CLEAR_INTERRUPT_STATUS = 0x07
CMD_GPIO_ENABLE_PIN_INTERRUPT   = 0x08
CMD_GPIO_WRITE_PARALLEL = 0x09
CMD_SPIM_TRANSFER_DATA = 0x0A


VENDOR_WRITE_DATA_EP = 0x04
VENDOR_READ_DATA_EP  = 0x05 | (1 << 7)
VENDOR_NOTIFY_EP     = 0x06 | (1 << 7)

VENDOR_INTERFACE = 0x02
VENDOR_EP_SIZE = 64

class USBHeader(Structure):
    _fields_ = [('seq_no', c_uint8, 1),
                ('cmd', c_uint8, 7),
                ('error', c_uint8, 8)]

class USBConfigLayout(Enum):
    GPIO       = 0x00
    SPI_MASTER = 0x03

class USBGPIOConfigDir(Enum):
    IN  = 0x00
    OUT = 0x01

class USBGPIOConfigPull(Enum):
    NONE = 0x00
    UP   = 0x01
    DOWN = 0x02

class USBInterruptStatus(Enum):
    GPIO = (1 << 0)
    SPI  = (1 << 1)

class USBSerialBridge(object):
    def __init__(self, pin_interrupt_callback=None):
        # initialize sequence number, master starts off at 1, while slave
        # starts off at 0
        self.seq_no = 0x01

        # save the pin interrupt callback
        self.pin_interrupt_callback = pin_interrupt_callback

        ctx = None
        res = usb.init(ctx)

        if res < 0:
            print('couldnt init libusb')
            sys.exit(-1)

        idVendor = 0x03eb
        idProduct = 0xffff

        self.dev, self.dev_desc, self.dev_handle = \
            find_usb_device(idVendor, idProduct)

        res = usb.reset_device(self.dev_handle)
        check_usb_result(res)

        # Set the configuration
        config = c_int()

        res = usb.get_configuration(self.dev_handle, byref(config))
        check_usb_result(res)

        #print('Configuration: %d'%config.value)

        res = usb.set_configuration(self.dev_handle, 1)
        check_usb_result(res)

        # Claim interface
        res = usb.claim_interface(self.dev_handle, VENDOR_INTERFACE)
        check_usb_result(res)

        self.error2str = [
            'NONE',
            'UNKNOWN_CMD',
            'NO_MEMORY',
            'MALFORMED_PACKET',
            'RESOURCE_BUSY',
            'PARAMETER',
        ]


    def sendReset(self, timeout_ms=1000):
        # build a control request:
        # D7: 0 = Host to device
        # Type: vendor
        # Recipient: interface
        bmRequestType = usb.LIBUSB_REQUEST_TYPE_VENDOR | \
                        usb.LIBUSB_RECIPIENT_INTERFACE

        # Reset command
        bRequest = 0x00

        # Reset command has no additional values to specify
        wValue = 0x00

        # Specify the interface here
        wIndex = VENDOR_INTERFACE

        # No data to transfer in data stage
        wLength = 0

        res = usb.control_transfer(
                self.dev_handle,
                c_uint8(bmRequestType),
                c_uint8(bRequest),
                c_uint16(wValue),
                c_uint16(wIndex),
                None,
                wLength,
                c_uint(timeout_ms))
        check_usb_result(res)

        self.last_notify_check = time()
        self.poll_interval_sec = .1


    def mainLoopTask(self):
        if time() > (self.poll_interval_sec + self.last_notify_check):
            self.last_notify_check = time()
            status = self.getStatus()
            if None == status:
                return

            if status & USBInterruptStatus.GPIO.value:
                print('GPIO interrupt')
                mask = self.sendGPIOGetInterruptStatus()
                if None == mask:
                    return
                else:
                    # Clear the interrupt
                    self.sendGPIOClearInterruptStatus(mask)
                    if None != self.pin_interrupt_callback:
                        self.pin_interrupt_callback(mask)
                    else:
                        print('pin interrupt: %x'%mask)

    def testSPIM(self, cs=0xff):
        for k in range(64):
            print("test #%d"%k)

            for datalen in range(1, VENDOR_EP_SIZE - 3):
                #print('datalen: %d'%datalen)
                data = bytes([x for x in range(datalen)])
                for k in range(1000):
                    #print('sending: %s'%self.arr2str(data))
                    rsp = self.sendSPIMTransfer(cs, data)
                    if rsp != None:
                        break
                    else:
                        print("retrying transmission %d"%(k + 1))


    '''
    def pinInterruptCallback(self, mask):
        print('Pin Status: %08x'%mask)

        data = bytearray([0xef, 0xeb, 0xad, 0xde])
        self.sendSPIMTransfer(1, data)

        for k in range(64):
            print("test #%d"%k)
            self.testSPIM()
    '''




    def getStatus(self, timeout_ms=0):
        status = c_uint32()
        data = (c_ubyte * sizeof(status))()
        num_transfered = c_int();

        res = usb.interrupt_transfer(
            self.dev_handle,
            VENDOR_NOTIFY_EP,
            data,
            c_int(len(data)),
            byref(num_transfered),
            c_uint(timeout_ms))

        if usb.LIBUSB_ERROR_TIMEOUT == res:
            print('Interrupt timeout')
            return None
        else:
            check_usb_result(res)
            #print('interrupt num transfered: %d'%num_transfered.value)
            memmove(byref(status), data, num_transfered.value)

            #print('interrupt status: %04x'%status.value)
            return status.value



    def sendData(self, arr, timeout_ms=1000):
        data = (c_ubyte * len(arr))(*arr)

        num_transfered = c_int();

        res = usb.bulk_transfer(
            self.dev_handle,
            VENDOR_WRITE_DATA_EP,
            data,
            c_int(len(data)),
            byref(num_transfered),
            c_uint(timeout_ms))

        check_usb_result(res)
        assert(num_transfered.value == len(arr))


    def recvData(self, timeout_ms=1000):
        data = (c_ubyte * VENDOR_EP_SIZE)()
        num_transfered = c_int()

        res = usb.bulk_transfer(
            self.dev_handle,
            VENDOR_READ_DATA_EP,
            data,
            c_int(len(data)),
            byref(num_transfered),
            c_uint(timeout_ms))
        check_usb_result(res)

        #print('read %d/%d bytes'%(num_transfered.value, len(data)))
        return data, num_transfered.value


    def arr2str(self, arr):
        return ':'.join('%02x'%val for val in arr)


    def transferCmd(self, cmd, cmd_data=[], num_attempts=1000):
        # prepare the data to send

        # prepare the command header
        cmd_header = USBHeader(self.seq_no, cmd, 0x00)
        assert(2 == sizeof(cmd_header))

        data = (c_uint8 * (sizeof(cmd_header) + len(cmd_data)))()

        # make sure the data payload is not bigger than the endpoint size
        assert(len(data) <= VENDOR_EP_SIZE)

        # copy the header into the data payload
        memmove(data, byref(cmd_header), sizeof(cmd_header))

        # copy in the rest of the data associated with the command
        for k in range(len(cmd_data)):
            data[k + 2] = cmd_data[k]

        for attempt in range(num_attempts):
            self.sendData(data)

            rsp, rsp_len = self.recvData()

            if rsp_len < 2:
                print('insufficient rsp data')
                continue

            #print('rsp: %s'%self.arr2str(rsp))

            rsp_header = USBHeader()
            memmove(byref(rsp_header), rsp, sizeof(rsp_header))

            #print('cmd_header.seq_no: %d'%cmd_header.seq_no)
            #print('rsp_header.seq_no: %d'%rsp_header.seq_no)
            # make sure the response agrees with the command we just sent
            if cmd_header.cmd != rsp_header.cmd:
                print('cmd mismatch')
                continue

            # make sure the sequence number agrees...
            if cmd_header.seq_no != rsp_header.seq_no:
                print('seq_no mismatch')
                continue

            break

        else:
            print('exceeded maximum attempts')
            assert(False)

        # ok increment sequence number
        self.seq_no ^= 0x01

        rsp = bytearray(rsp)

        #if 0 != cmd_header.error:
        #print('USB_ERROR: %s'%self.error2str[rsp_header.error])

        return rsp[2:rsp_len], rsp_header.error


    def sendConfigLayout(self, config, data=bytes()):
        assert(USBConfigLayout == type(config))
        rsp, error = self.transferCmd(CMD_CFG, config.
            value.to_bytes(1, 'little') + data)
        return error


    def sendConfigLayoutGPIO(self):
        return self.sendConfigLayout(USBConfigLayout.GPIO)


    def sendConfigLayoutSPIM(self, mode, dataorder):
        data = struct.pack("<BB", mode, dataorder)
        return self.sendConfigLayout(USBConfigLayout.SPI_MASTER, data)


    def sendGPIOConfig(self, gpio_index, dir, pull):
        assert(USBGPIOConfigDir == type(dir))
        assert(USBGPIOConfigPull == type(pull))

        cmd = bytearray([gpio_index, dir.value, pull.value])
        rsp, error = self.transferCmd(CMD_GPIO_CFG, cmd)
        return error


    def sendGPIOPinSet(self, gpio_index, level):
        rsp, error = self.transferCmd(CMD_GPIO_PIN_SET, bytes([gpio_index, level]))
        return error


    def sendGPIOPinRead(self, gpio_index):
        rsp, error = self.transferCmd(CMD_GPIO_PIN_READ, bytes([gpio_index]))
        if 0 == error:
            return 1 == rsp[0]
        else:
            return False


    def sendGPIOGetInterruptStatus(self):
        rsp, error = self.transferCmd(CMD_GPIO_GET_INTERRUPT_STATUS)

        if 0 == error:
            return int.from_bytes(rsp, 'little')
        else:
            return None

    def sendGPIOClearInterruptStatus(self, mask):
        rsp, error = self.transferCmd(
            CMD_GPIO_CLEAR_INTERRUPT_STATUS, mask.to_bytes(4, 'little'))
        return error


    def sendGPIOEnablePinInterrupt(self, index):
        rsp, error = self.transferCmd(
            CMD_GPIO_ENABLE_PIN_INTERRUPT, index.to_bytes(1, 'little'))
        print('enable pin interrupt: %d'%error)
        return error


    def sendEcho(self, echo_data):
        def print_data(prefix, data):
            if None != data:
                text = ''.join('%02x'%data[k] for k in range(len(data)))
                print('%s[%d]: %s'%(prefix, len(data), text))
            else:
                print('%s: None'%prefix)

        #print_data('send', echo_data)

        rsp, error = self.transferCmd(CMD_ECHO, echo_data)

        #print_data('recv', rsp)

        match = echo_data == rsp

        #print('match: %s'%match)
        return match


    def sendSPIMTransfer(self, index, data):
        assert(len(data) < VENDOR_EP_SIZE)

        data = index.to_bytes(1, 'little') + data
        #print('Sending: %s'%self.arr2str(data))

        rsp, error = self.transferCmd(
            CMD_SPIM_TRANSFER_DATA, data)

        #print('Error: %d'%error)
        #print('Respo: %s'%self.arr2str(rsp))

        if 0 != error:
            print('SPIM Transfer error: %s'%self.error2str[error])
            return None
        else:
            return rsp



    def generateRandomMsg(self, num):
        return bytearray( [int(random.random()*0xff) for _ in range(num)] )


    def testEcho(self):
        num_tests = 1000
        for testno in range(num_tests):
            print('echo test: %d'%testno)
            for msg_size in range(VENDOR_EP_SIZE - 2):
                msg = self.generateRandomMsg(msg_size)

                match = self.sendEcho(msg)
                #print('success: %s'%(rsp == msg))

                if False == match:
                    print('ERROR: echo test failed...')
                    return

        print('SUCCESS: echo test passed!')



if __name__ == '__main__':
    serial_bridge = USBSerialBridge()

    serial_bridge.sendReset()

    #serial_bridge.testEcho()

    print('sending config')
    def test_gpio():
        serial_bridge.sendConfigLayoutGPIO()
        serial_bridge.sendGPIOConfig(0, USBGPIOConfigDir.OUT, USBGPIOConfigPull.NONE)
        serial_bridge.sendGPIOConfig(6, USBGPIOConfigDir.IN, USBGPIOConfigPull.UP)
        serial_bridge.sendGPIOEnablePinInterrupt(6)

    def test_spi():
        serial_bridge.sendConfigLayoutSPIM(mode=0, dataorder=0)
        serial_bridge.sendGPIOConfig(6, USBGPIOConfigDir.OUT, USBGPIOConfigPull.NONE)
        serial_bridge.sendGPIOPinSet(6, True)

        serial_bridge.sendGPIOConfig(2, USBGPIOConfigDir.OUT, USBGPIOConfigPull.NONE)
        serial_bridge.sendGPIOPinSet(2, True)

        def callback(mask):
            serial_bridge.testSPIM(6)

        serial_bridge.pin_interrupt_callback = callback

    test_spi()
    #test_gpio()

    level = False
    current_time = time()

    while True:
        serial_bridge.mainLoopTask()

        if time() > current_time + 1:
            current_time = time()
            print('pin read: %s'%serial_bridge.sendGPIOPinRead(6))
            serial_bridge.sendGPIOPinSet(2, level)
            level ^= True

        #print('level: %s'%level)
        #serial_bridge.sendGPIOPinSet(0, level)
        #level ^= True
