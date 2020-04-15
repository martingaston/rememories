import serial

ser = Serial.serial('/dev/ttyUSB0')
ser.write(b'88')
connection_check = ser.read(4)

print(connection_check.decode('utf-8')
