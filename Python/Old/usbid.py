
import urllib.request, urllib.error, urllib.parse
import datetime
import pickle
import time
import pdb
import os
import re
import io

def retrieve_usb_ids():
    url = r'http://www.linux-usb.org/usb.ids'
    response = urllib.request.urlopen(url)
    # data is a byte array
    data = response.read()

    # convert data to a text stream
    textio = io.BytesIO(data)

    vendor_id = None
    vendor_str = None
    version = None

    saved_data = None

    vendor_id2str = {}
    vendor_id2products = {}

    pickle_file = 'usb_ids.pkl'

    if os.path.isfile(pickle_file):
        with open(pickle_file, 'rb') as f:
            saved_data = pickle.load(f)

            UPDATE_DELTA_DAYS = 1
            UPDATE_DELTA_SECONDS = 3600 * 24 * UPDATE_DELTA_DAYS

            delta_seconds = time.time() - saved_data['last_check']
            delta_hours = delta_seconds / 3600
            print('last check was %d hours ago'%delta_hours)

            if delta_seconds < UPDATE_DELTA_SECONDS:

                print('Using existing usb repository. Has not been %d days since last check'%UPDATE_DELTA_DAYS)
                vendor_id2str = saved_data['vendor_id2str']
                vendor_id2products = saved_data['vendor_id2products']

                return vendor_id2str, vendor_id2products
            else:
                print('Retrieving usb id list...')

    while True:

        bline = textio.readline()

        if 0 == len(bline):
            break

        try:
            line = str(bline, encoding='utf-8')
            line = line.rstrip()

        except UnicodeDecodeError:
            print('Decode error: %s'%line)
            continue

        if 0 == len(line):
            continue


        map = {
            '#': None,
            'C': None,
            'AT': None,
            'HID': None,
            'R': None,
            'B': None,
            'PHY': None,
            'HUT': None,
            'L': None,
            'HCC': None,
            'VT': None,
        }

        if '#' == line[0]:
            # Comment
            match = re.search(r'Version: (\d\d\d\d).(\d\d).(\d\d)', line)
            if match:
                year = int(match.group(1))
                mont = int(match.group(2))
                days = int(match.group(3))

                version = datetime.date(
                    year, mont, days)

                if None != saved_data:
                    if version <= saved_data['version']:
                        print('Using existing usb repository. Retrieved usb list is same age as our own!')
                        vendor_id2str = saved_data['vendor_id2str']
                        vendor_id2products = saved_data['vendor_id2products']
                        break
                    else:
                        print('Updating usb repository...')

        elif '\t' != line[0]:
            for kw in map:
                if line.startswith(kw):
                    vendor_id = None
                    vendor_str = None
                    break
            else:
                vendor_id, vendor_str = line.split('  ', 1)

                vendor_id = int(vendor_id, 16)

                vendor_id2str[vendor_id] = vendor_str
                vendor_id2products[vendor_id] = {}
        else:
            if None != vendor_id:
                line = line.lstrip()

                try:
                    product_id, product_str = line.split('  ', 1)

                    product_id = int(product_id, 16)

                    vendor_id2products[vendor_id][product_id] = product_str
                except ValueError:
                    print('Error unpacking: %s'%line)
                    pdb.set_trace()

    assert(None != version)

    saved_data = {}
    saved_data['vendor_id2str'] = vendor_id2str
    saved_data['vendor_id2products'] = vendor_id2products
    saved_data['version'] = version
    saved_data['last_check'] = time.time()

    with open(pickle_file, 'wb') as f:
        pickle.dump(saved_data, f)
        print('saving repository!')

    return vendor_id2str, vendor_id2products
