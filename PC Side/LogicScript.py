#!/usr/bin/python
from logging import exception
from thread import start_new_thread, exit
import sys, string
from time import sleep
import hal # LinuxCNC must be up and running prior to loading this.
#c = 
c = hal.component("LogicScript")
hal.component_exists("Hal2Arduino")
hostCheckOld=range(0,550)

def PinResetter():
	try:
        while(True):
            command = 510
            print("RESETTEST")
            sleep(0.25)
            hostCheck510=hal.get_value("Hal2Arduino.StopPin")
            hostCheck514=hal.get_value("Hal2Arduino.ProbeInputPin")
            if hostCheck510 == 1:
                hal.set_p("Hal2Arduino.StopPin", 0)
            if hostCheck514 == 1:
                hal.set_p("Hal2Arduino.ProbeInputPin", 0)
    except Exception:
        raise exit()
#def PinResetter():
#    command = 510
#    hostCheck510=c["StopPin"]
#    hostCheck514=c["ProbeInputPin"]
#    if hostCheck510 == 1:
#        c["StopPin"] = 0
#    if hostCheck514 == 1:
#        c["ProbeInputPin"] = 0
#c.newpin("motion_debug-float-0",hal.HAL_FLOAT,hal.HAL_IN)
try:
    start_new_thread(PinResetter,())
    #while(True):
     #   print("RESETTEST")
      #  PinResetter()
       # 

except Exception:
    raise SystemExit

sleep(1)
