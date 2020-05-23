const char ADDR[] = {A4, A5, A2,'X', 'X', 'X', 'X', 'X', 'X', A3, 10, 11, 12, 13, A0, A1};
const char DATA[] = {9, 8, 7, 6 ,5 ,4, 3, A7};
#define CLOCK 2
#define READ_WRITE A6

void setup() {
  Serial.begin(57600);
  // put your setup code here, to run once:
  for (int i = 0; i < 16; i++) {
    if (ADDR[i] != 'X') {
      pinMode(ADDR[i], INPUT);
    }
  }
  for (int i = 0; i < 7; i++) {
    pinMode(DATA[i], INPUT);
  }
  pinMode(CLOCK, INPUT);
  attachInterrupt(digitalPinToInterrupt(CLOCK), onClock, RISING);
}


void onClock() {
  char output[15];
  unsigned int address = 0;
  for (int i = 0; i < 16; i++) {
    if (ADDR[i] != 'X') {
      int bit = digitalRead(ADDR[i]) ? 1: 0;
      Serial.print(bit);
      address  = (address << 1) + bit;
    } else { 
      Serial.print('X');
      address  = (address << 1) + 0;
    }
  }
  Serial.print("   ");
  unsigned int data = 0;
  for (int i = 0; i < 8; i++) {
      int bit = 0;
      if (i == 7) {
        bit = analogRead(DATA[i]) > 767 ? 1: 0;
      } else {
        bit = digitalRead(DATA[i]) ? 1: 0;
      }
      Serial.print(bit);
      data  = (data << 1) + bit;
  }
  sprintf(output, "    %04x %c %02x", address, analogRead(READ_WRITE) > 767 ? 'r': 'w' , data);
  Serial.println(output);
}

void loop() {
  // put your main code here, to run repeatedly:

}
