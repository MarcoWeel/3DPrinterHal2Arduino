from logging import exception
import sys, string
from time import sleep
import hal # LinuxCNC must be up and running prior to loading this.
c = hal.component("Hal2Arduino")
hostCheckOld=range(0,550)

def StopPinResetter():
    command = 510
    hostCheck=c["StopPin"]
    if hostCheck == 1:
        c["StopPin"] = 0

try:
    while(True):
        StopPinResetter()
        sleep(0.25)

except Exception:
    raise SystemExit

