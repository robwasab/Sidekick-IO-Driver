import sys
sys.path.append(r"../../Driver/VisualStudio/SidekickIO/Python")

from sidekickio import SidekickIO
from time import sleep

# start the sidekick up in dfu mode
sk = SidekickIO(SidekickIO.FW_MODE_DFU)

fwpath = r"../../../Firmware/Application/helloworld/Debug/serial_bridge.bin"

# update the firmware and reset into application mode
sk.firmware_update_from_file(fwpath, sk.FW_MODE_APP)
