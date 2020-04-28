import serial
import time

ser = serial.Serial(port='/dev/cu.usbmodem14201', baudrate=38400)
time.sleep(3) # Arduino restarts when serial connection is made, very important else program will just hang forever :D
ser.write(b'\x00')
ser.read(128)
ser.read(1)
print(ser.read(1))
ser.close()
