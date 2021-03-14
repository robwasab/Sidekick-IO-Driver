import sys
sys.path.append(r"..\..\Driver\VisualStudio\SidekickIO\Python")

from sidekickio import SidekickIO
from time import sleep

sk = SidekickIO()

on = False
while True:
    sleep(1)
    print('led: %s'%on)
    sk.gpio_set_led(on)
    on ^= True
