import sys
sys.path.append(r"../../Driver/VisualStudio/SidekickIO/Python")

import sidekickio
from time import sleep

sk = sidekickio.SidekickIO()


baudrate = 1000000
sk.config_layout_spim(sk.SPI_MODE_0, sk.SPI_DATA_ORDER_MSB, baudrate)

# need to initialize the chip select pin to output
cs = 10
sk.gpio_config(cs, sk.GPIO_CONFIG_DIR_OUT, sk.GPIO_CONFIG_PULL_NONE)


while True:

    print('Demo: transfer packet')

    # max size is the packet size minus the chip select byte
    buf = bytearray(sidekickio.MAX_PACKET_SIZE - 1)

    # write some data to the buffer so we can tell if any apis mutate it
    for k in range(len(buf)):
        buf[k] = k

    # transfer mutates buf
    sk.spim_transfer_packet(cs, buf)

    # we expect buf to be all zeros if there isn't a device connected
    # print(buf)

    # reset buf data
    for k in range(len(buf)):
        buf[k] = k

    # write has no effect on buf
    sk.spim_write(cs, buf);

    # we expect buf to have data in it
    # print(buf)

    # we can send more data by using ganging together multiple calls to
    # spim_transfer_packet by using the higher level api 'spim_transfer'
    buf = bytearray(128)
    for k in range(len(buf)):
        buf[k] = k

    sk.spim_transfer(cs, buf)

    # we expect buf to be all zeros if there isn't a device connected
    #print(buf)

    sleep(1)
