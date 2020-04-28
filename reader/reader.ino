#include <SPI.h>

// 250 kHz, clock idle when high, reading LSB first on falling edge
SPISettings settings(250000, LSBFIRST, SPI_MODE3);

#define BAUDRATE 38400
#define CLK 13
#define DATA 12
#define CMD 11
#define ATT 10
#define ACK 2

volatile int ackState = HIGH;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(BAUDRATE);
  while (!Serial) {
    ; // wait for serial port to connect
  }

  setPins();
  SPI.begin(); // this overwrites some pin settings - one not needed?
}

void setPins() {
   // DATA from card (MISO)
   pinMode(DATA, INPUT_PULLUP);
   // CMD to card (MOSI)
   pinMode(CMD, OUTPUT);
   // ATT to card (SS)
   pinMode(ATT, OUTPUT);
   // CLK signal from PSX to card. 250KHz. (SCK)
   pinMode(CLK, OUTPUT);
   // ACK from card to PSX
   pinMode(ACK, INPUT_PULLUP);

   digitalWrite(CMD, LOW);
   digitalWrite(ATT, HIGH);
   digitalWrite(CLK, HIGH);

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
  command(0x81, 500); // memory card access
  command(0x52, 500); // send read 'R' command
  command(0x00, 500); // receive memory card ID1
  command(0x00, 500); // receive memory card ID2
  command(addressMSB, 500); // send address MSB
  command(addressLSB, 500); // send address LSB
  command(0x00, 2800); // receive command acknowledge 1
  command(0x00, 2800); // receive command acknowledge 2
  command(0x00, 2800); // receive confirmed address MSB
  command(0x00, 2800); // receive confirmed address LSB

  // receive data sector (128 bytes)
  for (int i = 0; i < 128; i++) {
    Serial.write(command(0x00, 150));
  }

  Serial.write(command(0x00, 500)); // checksum (MSB xor LSB xor data)
  Serial.write(command(0x00, 500)); // memory card end byte, should be 0x47
  
  SPI.endTransaction();

  digitalWrite(ATT, HIGH);
}


void loop() {
  // put your main code here, to run repeatedly:

  if(Serial.available() > 0)
  {
    // read the incoming byte
    byte incomingByte = Serial.read();    
    delay(10);
    readFrame(0); // hardcode test first frame on card
  }

}
