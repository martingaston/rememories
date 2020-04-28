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


void loop() {
  // put your main code here, to run repeatedly:

  if(Serial.available() > 0)
  {
    // read the incoming byte
    byte incomingByte = Serial.read();

    // say what you got:
    Serial.print("I received: ");
    Serial.println(incomingByte, DEC);
    
    delay(10);
  }

}
