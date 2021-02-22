import libusb as usb
from ctypes import *
import pdb
import sys

from usbid import retrieve_usb_ids

vendor_id2str, vendor_id2products = retrieve_usb_ids()

ctx = None

def human_readable_usbid(idVendor, idProduct):
    vendor_name = vendor_id2str.get(idVendor, '?')
    product_name = vendor_id2products.get(idVendor, {}).get(idProduct, '?')

    txt = 'idVendor: 0x%04x (%s), idProduct: 0x%04x (%s)'%(
        idVendor, vendor_name, idProduct, product_name)
    return txt

def check_usb_result(res):
    if res < 0:
        print('ERROR [%d]: %s'%(res, usb.strerror(res)))
        assert(False)

def find_usb_device(idVendor, idProduct):
    # pointer to a list of device pointers
    dev_list = POINTER(POINTER(usb.device))()

    cnt = usb.get_device_list(ctx, byref(dev_list))

    print('count: %d'%cnt)

    found = POINTER(usb.device)()
    found_desc = None

    for k in range(cnt):
        desc = usb.device_descriptor()

        dev = dev_list[k]
        res = usb.get_device_descriptor(dev, byref(desc))

        if res < 0:
            print("failed to get device descriptor")
            sys.exit(-1)

        print('[%s], bus:%d, addr:%2d'%(
            human_readable_usbid(desc.idVendor, desc.idProduct),
            usb.get_bus_number(dev),
            usb.get_device_address(dev)))

        if idVendor == desc.idVendor and idProduct == desc.idProduct:
            found = dev
            found_desc = desc

    if None == found:
        print('Did not find device...')
        sys.exit(-1)
    else:
        print('Found device!')

        # open the device
        handle = POINTER(usb.device_handle)()

        # opening a device increments its reference
        res = usb.open(found, byref(handle))
        check_usb_result(res)
        print('Opened usb device!')

        # free the list, and unref each device
        usb.free_device_list(dev_list, c_int(1))

        return found, found_desc, handle
