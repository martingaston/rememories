import serial
import time
import struct

FRAME_SIZE = 128
FRAME_COUNT = 1024
BAUDRATE = 38400

def read_frame(frame_number):
    frame_as_bytes = struct.pack('>H', frame_number)
    ser.write(b'\x00' + frame_as_bytes)

    data = ser.read(FRAME_SIZE)
    checksum = ser.read(1)
    status = ser.read(1)

    if status != b'\x47':
        raise IOError(f"Memory card end byte on frame {frame_number} not G; {status.decode('utf-8')} returned")

    return data

def parse_args():
    import argparse
    parser = argparse.ArgumentParser("A command-line interface for reading a PlayStation 1 memory card to a file")
    parser.add_argument("serialport", help=f"the serial port connected to the Arduino, e.g. /dev/cu.usbmodem14201 or /dev/ttyACM0")
    parser.add_argument("-f", "--file", help="the name of the file to write to, default to memcard.mcr", default="memcard.mcr")
    return parser.parse_args()

if __name__ == "__main__":
    args = parse_args()
    ser = serial.Serial(port=args.serialport, baudrate=BAUDRATE)
    time.sleep(3) # Arduino restarts when serial connection is made, very important else program will just hang forever :D

    with open(args.file, 'wb') as f:
        for frame_number in range(FRAME_COUNT):
            print(f"{frame_number} / {FRAME_COUNT} frames read to file")
            data = read_frame(frame_number)
            f.write(data)

    ser.close()
