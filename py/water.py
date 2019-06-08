#!/usr/bin/env python3

import sys
import time
import serial



def main():
    func = "water"
    try:
        valve  = sys.argv[1]
        amount = sys.argv[2]
    except:
        func="stop"
    #
    # ser = serial.Serial(
    #     port="/dev/ttyUSB0",
    #     baudrate=19200
    # )
    if func == "stop":
        cmd = b'WTR0 0\n'
        print(cmd.decode())
        ser.write(b'WTR0 1\n')
        sys.exit(0)
    else:

        cmd = f"WTR1 {valve} {amount}\0"
        print(cmd)
        ser.write(command.encode())
        sys.exit(0)


if __name__ == '__main__':
    main()
