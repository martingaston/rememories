# rememories
playing around with recovering an original playstation memory card.

# install
- You'll need to upload the reader.ino file to your Arduino 
- You'll also need to install the pyserial library in a Python 3 environment (this is in requirements.txt)

when installed, you can run with: 
`python writer.py /location/of/arduino --file output_filename.mcr`

# pinout (source: http://problemkaputt.de/psx-spx.htm#pinoutscontrollerportsandmemorycardports)

  1 In  JOYDAT Data from joypad/card (data in)    _______________________
  2 Out JOYCMD Data to joypad/card (command out) |       |       |       |
  3 -   +7.5V  +7.5VDC supply (eg. for Rumble)   | 9 7 6 | 5 4 3 |  2 1  | CARD
  4 -   GND    Ground                            |_______|_______|_______|
  5 -   +3.5V  +3.5VDC supply (normal supply)     _______________________
  6 Out /JOYn  Select joypad/card in Slot 1/2    |       |       |       |
  7 Out JOYCLK Data Shift Clock                  | 9 8 7 | 6 5 4 | 3 2 1 | PAD
  8 In  /IRQ10 IRQ10 (Joy only, not mem card)     \______|_______|______/
  9 In  /ACK   IRQ7 Acknowledge (each eight CLKs)
  Shield       Ground (Joypad only, not memory card)

# soldering tips!
incoming :) 

# big thanks to the following
Memcarduino: https://github.com/ShendoXT/memcarduino
Nocash PSX Specification: http://problemkaputt.de/psx-spx.htm#memorycarddataformat
PlayStation Memory Card @ Hardware Book: http://www.hardwarebook.info/Sony_Playstation_Memory_Card
