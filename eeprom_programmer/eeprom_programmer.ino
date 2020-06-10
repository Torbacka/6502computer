  #define SHIFT_DATA 2
#define SHIFT_CLK 3
#define SHIFT_LATCH 4
#define EEPROM_D0 5
#define EEPROM_D7 12
#define WRITE_EN 13
#define OUTPUT_EN A0

/*
   Output the address bits and outputEnable signal using shift registers.
*/
void setAddress(int address, bool outputEnable) {
  digitalWrite(OUTPUT_EN, !outputEnable);
  shiftOut(SHIFT_DATA, SHIFT_CLK, MSBFIRST, (address >> 8));
  shiftOut(SHIFT_DATA, SHIFT_CLK, MSBFIRST, address);

  digitalWrite(SHIFT_LATCH, LOW);
  digitalWrite(SHIFT_LATCH, HIGH);
  digitalWrite(SHIFT_LATCH, LOW);
}


/*
   Read a byte from the EEPROM at the specified address.
*/
byte readEEPROM(int address) {
  for (int pin = EEPROM_D0; pin <= EEPROM_D7; pin += 1) {
    pinMode(pin, INPUT);
  }
  setAddress(address, /*outputEnable*/ true);

  byte data = 0;
  for (int pin = EEPROM_D7; pin >= EEPROM_D0; pin -= 1) {
    data = (data << 1) + digitalRead(pin);
  }
  return data;
}


/*
   Write a byte to the EEPROM at the specified address.
*/
void writeEEPROM(int address, byte data) {
  setAddress(address, /*outputEnable*/ false);
  for (int pin = EEPROM_D0; pin <= EEPROM_D7; pin += 1) {
    pinMode(pin, OUTPUT);
  }
  delay(1);
  digitalWrite(WRITE_EN, LOW);
  delay(1);
  for (int pin = EEPROM_D0; pin <= EEPROM_D7; pin += 1) {
    digitalWrite(pin, data & 1);
    data = data >> 1;
  }

  digitalWrite(WRITE_EN, HIGH);
  delay(50);
}


/*
   Read the contents of the EEPROM and print them to the serial monitor.
*/
void printContents() {
  delay(50);
  for (int base = 0; base <= 512; base += 16) {
    byte data[16];
    for (int offset = 0; offset <= 15; offset += 1) {
      data[offset] = readEEPROM(base + offset);
    }

    char buf[80];
    sprintf(buf, "%03x:  %02x %02x %02x %02x %02x %02x %02x %02x   %02x %02x %02x %02x %02x %02x %02x %02x",
            base, data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7],
            data[8], data[9], data[10], data[11], data[12], data[13], data[14], data[15]);

    Serial.println(buf);
  }
}


void setup() {
  // put your setup code here, to run once:
  pinMode(SHIFT_DATA, OUTPUT);
  pinMode(SHIFT_CLK, OUTPUT);
  pinMode(SHIFT_LATCH, OUTPUT);
  pinMode(WRITE_EN, OUTPUT);
  pinMode(OUTPUT_EN, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(WRITE_EN, HIGH);
  Serial.begin(57600);

  /* Erase entire EEPROM
    Serial.print("Erasing EEPROM");
    for (int address = 0; address <= 512; address += 1) {
    writeEEPROM(address, 0xff);

    if (address % 64 == 0) {
      Serial.print(".");
    }
    }
    Serial.println(" done");


    // Program data bytes
    Serial.print("Programming EEPROM");
    for (int address = 0; address < sizeof(data); address += 1) {
    writeEEPROM(address, data[address]);

    if (address % 64 == 0) {
      Serial.print(".");
    }
    }
    Serial.println(" done");
    digitalWrite(WRITE_EN, HIGH);
    delay(100);*/
  Serial.println("Reading EEPROM");
  printContents();
  setAddress(0xfffd, true);

}
byte message[1024];
int message_pointer = 0;
int message_timer = 0;
bool message_recived = false;
void loop() {
  if (Serial.available() > 0) {
    // read the incoming byte:
    char incomingByte = Serial.read();
    message[message_pointer++] = incomingByte;

    message_timer = 0;
    message_recived = true;
  }
  message_timer++;
  if (message_timer > 2000 && message_recived) {
    delay(50);
    Serial.print("Programming EEPROM");
    for (int address = 0; address < message_pointer; address += 1) {
      writeEEPROM(address, message[address]);
      if (address % 64 == 0) {
        Serial.print(".");
      }
    }

    writeEEPROM(0xfffc, 0x00);
    writeEEPROM(0xfffd, 0x80);
    Serial.println(" done");
    Serial.println(readEEPROM(0xfffc));
    Serial.println(readEEPROM(0xfffd));
    Serial.println(" done");
    Serial.println("Reading EEPROM");
    printContents();
    message_pointer = 0;
    message_timer = 0;
    message_recived = false;
    memset(message, 0, sizeof message);
  }

}
