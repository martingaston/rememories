#include <SPI.h>

// 250 kHz, clock idle when high, reading LSB first on falling edge
SPISettings settings(250000, LSBFIRST, SPI_MODE3);

#define BAUDRATE 38400
#define CLK 13
#define DATA 12
#define CMD 11
#define ATT 10
#define ACK 2

#define MEMORY_CARD_ACCESS 0x81
#define READ_COMMAND 0x52
#define EMPTY_COMMAND 0x00

volatile int ackState = HIGH;

void setup() {
  Serial.begin(BAUDRATE);
  while (!Serial) {
    ; // wait for serial port to connect
  }

  setPins();
  SPI.begin();
}

void setPins() {
   // DATA from card (MISO)
   pinMode(DATA, INPUT_PULLUP);
   // ATT to card (SS)
   pinMode(ATT, OUTPUT);
   // ACK from card to PSX
   pinMode(ACK, INPUT_PULLUP);
   digitalWrite(ATT, HIGH);

   attachInterrupt(digitalPinToInterrupt(ACK), acknowledge, FALLING);
}

void acknowledge() {
  ackState = !ackState;
}

byte command(byte toSend, int wait) {
  ackState = HIGH;

  byte recieved = SPI.transfer(toSend);

  while(ackState == HIGH) {
    wait--;
    delayMicroseconds(1);
    if(wait == 0) {
      break;
    }
  }

  return recieved;
}

void readFrame(unsigned int address) {
  byte addressMSB = (address >> 8) & 0xFF;
  byte addressLSB = address & 0xFF;
  
  digitalWrite(ATT, LOW);

  SPI.beginTransaction(settings);
  command(MEMORY_CARD_ACCESS, 500); // memory card access
  command(READ_COMMAND, 500); // send read 'R' command
  command(EMPTY_COMMAND, 500); // receive memory card ID1
  command(EMPTY_COMMAND, 500); // receive memory card ID2
  command(addressMSB, 500); // send address MSB
  command(addressLSB, 500); // send address LSB
  command(EMPTY_COMMAND, 2800); // receive command acknowledge 1
  command(EMPTY_COMMAND, 2800); // receive command acknowledge 2
  command(EMPTY_COMMAND, 2800); // receive confirmed address MSB
  command(EMPTY_COMMAND, 2800); // receive confirmed address LSB

  // receive data sector (128 bytes)
  for (int i = 0; i < 128; i++) {
    byte result = command(EMPTY_COMMAND, 150);
    Serial.write(result);
  }

  Serial.write(command(EMPTY_COMMAND, 500)); // checksum (MSB xor LSB xor data)
  Serial.write(command(EMPTY_COMMAND, 500)); // memory card end byte, should be 0x47
  
  SPI.endTransaction();

  digitalWrite(ATT, HIGH);
}


void loop() {
  if(Serial.available() > 0)
  {
    // read the incoming byte
    byte incomingByte = Serial.read();  
    delay(10);
    unsigned int frameAddress = (Serial.read() << 8) + Serial.read();
    readFrame(frameAddress);
  }
}
