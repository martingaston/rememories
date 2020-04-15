#define BAUDRATE 38400

void setup() {
  // put your setup code here, to run once:
  Serial.begin(BAUDRATE);
}

void loop() {
  // put your main code here, to run repeatedly:

  if(Serial.available() > 0)
  {
    // read the incoming byte
    incomingByte = Serial.read();

    // say what you got:
    Serial.print("I received: ");
    Serial.println(incomingByte, DEC);
    
    delay(10);
  }

}
