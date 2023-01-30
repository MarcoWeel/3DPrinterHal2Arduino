from logging import exception
import sys, string
from time import sleep
import hal # LinuxCNC must be up and running prior to loading this.
c = hal.component("Hal2Arduino")
hostCheckOld=range(0,550)

def PinResetter():
    command = 510
    hostCheck510=c["StopPin"]
    hostCheck514=c["ProbeInputPin"]
    if hostCheck510 == 1:
        c["StopPin"] = 0
    if hostCheck514 == 1:
        c["ProbeInputPin"] = 0

try:
    while(True):
        PinResetter()
        sleep(0.25)

except Exception:
    raise SystemExit

