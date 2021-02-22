from time import time
import libusb as usb
from ctypes import *
import pdb
import sys


from libusb_tools import check_usb_result, find_usb_device


ctx = None

def set_line_coding(dev_handle, interface):
    bmRequestType = 0b00100001
    bRequest = 0x20

    line_coding = (c_ubyte * 7)()
    baud = 115200
    line_coding[0] =  baud & 0xff
    line_coding[1] = (baud >> 8) & 0xff
    line_coding[2] = (baud >> 16) & 0xff
    line_coding[3] = (baud >> 24) & 0xff
    line_coding[4] = 0  # bCharFormat: 0 = 1 stop bit
    line_coding[5] = 0  # bParityType: 0 = None
    line_coding[6] = 8  # bDataBits: 8 = 8 bits

    print('interface: %d'%interface)

    res = usb.control_transfer(
        dev_handle=dev_handle,
        request_type=c_ubyte(bmRequestType),
        bRequest=c_ubyte(bRequest),
        wValue=c_ushort(0),
        wIndex=c_ushort(interface),
        data=line_coding,
        wLength=c_ushort(len(line_coding)),
        timeout=c_uint(1000))

    check_usb_result(res)

last_interrupt_check = 0

def check_interrupt(dev_handle):
    global last_interrupt_check
    if time() > last_interrupt_check + 1:
        last_interrupt_check = time()
        print('checking interrupt')

        data = (c_ubyte * 8)()
        num_transfered = c_int();

        res = usb.interrupt_transfer(dev_handle,
            c_uint8(0x83),
            data,
            c_int(len(data)),
            byref(num_transfered),
            c_uint(1000))
        print('interrupt transfer: %s'%res)

VENDOR_WRITE_DATA_EP = 0x04
VENDOR_READ_DATA_EP  = 0x05

last_send = 0

def send_data(dev_handle):
    global last_send
    if time() > last_send + 1:
        last_send = time()

        print('sending data')

        data = (c_ubyte * 64)()
        num_transfered = c_int();

        res = usb.bulk_transfer(
            dev_handle,
            0x04,
            data,
            c_int(len(data)),
            byref(num_transfered),
            c_uint(1000))
        check_usb_result(res)


        print('bulk transfer: %s'%res)






result = usb.init(ctx)

if result < 0:
    print('couldnt init libusb')
    sys.exit(-1)

idVendor = 0x03eb
idProduct = 0xffff

dev, dev_desc, dev_handle = find_usb_device(idVendor, idProduct)

print('Setting configuration')
config = c_int()

res = usb.get_configuration(dev_handle, byref(config))
check_usb_result(res)

print('Configuration: %d'%config.value)

res = usb.set_configuration(dev_handle, 1)
check_usb_result(res)

# comms interface
def claim_interface(dev_handle, interface_no):
    print('claiming interface: %d'%interface_no)
    res = usb.claim_interface(dev_handle, interface_no)
    check_usb_result(res)

    '''
    res = usb.kernel_driver_active(dev_handle, interface_no)

    if 0 == res:
        # no kernel driver is active
        print('No kernel driver is active on interface %d'%interface_no)

        res = usb.claim_interface(dev_handle, interface_no)
        check_usb_result(res)
    elif 1 == res:
        print('Kernel driver active on interface %d'%interface_no)
    else:
        print('Kernel driver active returned code')
        check_usb_result(res)
    '''

#claim_interface(dev_handle, 0x00)
claim_interface(dev_handle, 0x02)

set_line_coding(dev_handle, 0)

while True:
    send_data(dev_handle)
