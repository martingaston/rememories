import serial
import time
import struct
from enum import Enum

FRAME_SIZE = 128
FRAME_COUNT = 1024
BAUDRATE = 38400
INITIALISER = b'\x00'
DEFAULT_FILE_NAME = "memcard.mcr"

class MemoryCardStatus(Enum):
    GOOD = b'\x47'
    CARD_NOT_PRESENT = b'\xff'

class MemoryCard:
    def __init__(self, serialport):
        self.spi = serial.Serial(port=serialport, baudrate=BAUDRATE)
        time.sleep(3) # Arduino restarts when serial connection is made, very important else program will just hang forever :D

    def _read(self, number_of_bytes):
        return self.spi.read(number_of_bytes)

    def _write(self, data):
        return self.spi.write(data)

    def _validate_status(self, status):
        if MemoryCardStatus(status) != MemoryCardStatus.GOOD:
            raise IOError(f"Memory card end byte on frame {frame_number} not G; {status.decode('utf-8')} returned")

    def _validate_checksum(self, local_checksum, memory_card_checksum):
        if memory_card_checksum != local_checksum:
            raise IOError(f"Memory card checksum mismatch on frame {frame_number}: 0x{local_checksum:x} is not 0x{memory_card_checksum:x}")

    def _generate_checksum(self, data):
        checksum = 0
        for byte in data:
            checksum ^= byte
        return checksum.to_bytes(1, 'big')

    def close(self):
        self.spi.close()

    def read_frame(self, frame_number):
        frame_as_bytes = struct.pack('>H', frame_number)
        self._write(INITIALISER + frame_as_bytes)

        data = self._read(FRAME_SIZE)
        memory_card_checksum = self._read(1)
        status = self._read(1)

        self._validate_status(status)
        self._validate_checksum(self._generate_checksum(frame_as_bytes + data), memory_card_checksum)

        return data

def parse_args():
    import argparse
    parser = argparse.ArgumentParser("A command-line interface for reading a PlayStation 1 memory card to a file")
    parser.add_argument("serialport", help=f"the serial port connected to the Arduino, e.g. /dev/cu.usbmodem14201 or /dev/ttyACM0")
    parser.add_argument("-f", "--file", help=f"the name of the file to write to, default to {DEFAULT_FILE_NAME}", default=DEFAULT_FILE_NAME)
    return parser.parse_args()

if __name__ == "__main__":
    args = parse_args()
    memory_card = MemoryCard(args.serialport)

    with open(args.file, 'wb') as f:
        for frame_number in range(FRAME_COUNT):
            print(f"{frame_number + 1} / {FRAME_COUNT} frames read to file")
            data = memory_card.read_frame(frame_number)
            f.write(data)

    memory_card.close()
