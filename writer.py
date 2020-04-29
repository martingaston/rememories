import serial
import time
import struct

ser = serial.Serial(port='/dev/cu.usbmodem14201', baudrate=38400)
time.sleep(3) # Arduino restarts when serial connection is made, very important else program will just hang forever :D

def read_frame(frame_number):
    frame_as_bytes = struct.pack('>H', frame_number)
    ser.write(b'\x00' + frame_as_bytes)

    data = ser.read(128)
    checksum = ser.read(1)
    status = ser.read(1)

    if status != b'\x47':
        raise IOError(f"Memory card end byte on frame {frame_number} not G; {status.decode('utf-8')} returned")

    return data

with open('memorycard.mcr', 'wb') as f:
    for frame_number in range(1024):
        print(f"{frame_number} / 1024 frames read to file")
        data = read_frame(frame_number)
        f.write(data)

ser.close()
