#!/usr/bin/env python3

import sys
import time
import serial

ADDRESS = 12
PORT    = "/dev/ttyUSB0"
BAUD    = 57600
VALVE_ACTIVE_TIME=5 #sec

def main():

    try:
        sleep_time = float(sys.argv[1])
    except:
        sleep_time = VALVE_ACTIVE_TIME

    ser = serial.Serial(
        port=PORT,
        baudrate=BAUD
    )

    cmd_stop = f"WTR0 {ADDRESS} 0;"
    for i in range(1,5):
        cmd = f"WTR1 {ADDRESS} {i} 30;"
        print(cmd)
        ser.write(cmd.encode())
        time.sleep(sleep_time)
        print(cmd_stop)
        ser.write(cmd_stop.encode())
        time.sleep(0.1)


if __name__ == '__main__':
    main()
