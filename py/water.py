#!/usr/bin/env python3

import sys
import time
import serial

ADDRESS = 12
PORT    = "/dev/ttyUSB0"
BAUD    = 57600


def main():
    func = "water"
    try:
        valve  = sys.argv[1]
        amount = sys.argv[2]
    except:
        func="stop"

    ser = serial.Serial(
        port=PORT,
        baudrate=BAUD
    )
    if func == "stop":
        cmd = f'WTR0 {ADDRESS} 0;'
        print(cmd)
        ser.write(cmd.encode())
        sys.exit(0)

    else:
        cmd = f"WTR1 {ADDRESS} {valve} {amount};"
        print(cmd)
        ser.write(cmd.encode())
        sys.exit(0)


if __name__ == '__main__':
    main()
