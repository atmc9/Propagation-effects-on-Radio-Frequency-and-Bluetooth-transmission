#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 4, 5, 6, 7);

#define PACKET_HEADER_SIZE 3
#define PAYLOAD_SIZE 1
#define CHECKSUM_SIZE 1
#define PACKET_SIZE (PACKET_HEADER_SIZE + PAYLOAD_SIZE + CHECKSUM_SIZE)
#define ADDR 1
#define LED_PIN 13

byte temp = 0;
byte d1; // left digit
byte d2; // right digit
byte digitToggle = 0;

const byte packetHeader[PACKET_HEADER_SIZE] = {0x8F, 0xAA, ADDR};

void setup()
{
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  analogWrite(10, 128);
  // Print a message to the LCD.
  lcd.print("Temperature ");

  Serial.begin(1200);

  for (int i = 2; i <= 7; i++) {
    pinMode(i, OUTPUT);
  }
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(6, HIGH);
  digitalWrite(7, HIGH);

  // Disable the timer overflow interrupt
  TIMSK2 &= ~(1 << TOIE2);

  // Set timer2 to normal mode
  TCCR2A &= ~((1 << WGM21) | (1 << WGM20));
  TCCR2B &= ~(1 << WGM22);

  // Use internal I/O clock
  ASSR &= ~(1 << AS2);

  // Disable compare match interrupt
  TIMSK2 &= ~(1 << OCIE2A);

  // Prescalar is clock divided by 128
  TCCR2B |= (1 << CS22)  | (1 << CS20);
  TCCR2B &= ~(1 << CS21);

  // Start the counting at 0
  TCNT2 = 0;

  // Enable the timer2 overflow interrupt
  TIMSK2 |= (1 << TOIE2);
}

void loop() {
  digitalWrite(LED_PIN, HIGH);
  temp = readByte();
  setValue(temp); 
  lcd.setCursor(0, 1);
  int dig = (int)temp;  Serial.print("Data Read"); Serial.print(dig);
  lcd.clear();
  lcd.print("Temp: ");
  lcd.print(dig);
  digitalWrite(LED_PIN, LOW);
}

byte readByte() {
  int pos = 0;
  byte val;
  byte c = 0;
  while (pos < PACKET_HEADER_SIZE) {
    while (Serial.available() == 0);
    c = Serial.read();

    if (c == packetHeader[pos]) {
      if (pos == PACKET_HEADER_SIZE - 1) {
        byte checksum;

        // Wait until something is available
        while (Serial.available() < PAYLOAD_SIZE + CHECKSUM_SIZE);
        val =  Serial.read();
        checksum = Serial.read();

        if (checksum != (packetHeader[0] ^ packetHeader[1] ^ packetHeader[2] ^ val)) {
          // Checksum failed
          pos = -1;
        }

      }
      pos++;
    } else {
      if (c == packetHeader[0]) {
        pos = 1;
      } else {
        pos = 0;
      }
    }
  }
  return val;
}


void setValue(byte n) {
  d1 = n / 10;
  d2 = n % 10;
  int dig = (int)n;
  Serial.write(n); // writing on to TX of A2 -> available for RX of bluetooth module
  Serial.print("Data Read");
  Serial.print(dig);
}

void setOutput(byte d) {
  // This is more complex because the 74LS247 inputs are on
  // nonconsecutive Arduino output pins (for ease of soldering).
  PORTD &= ~0x3C; // turn off digital pins 2-5
  if ((d & 0x1) > 0) {
    // 74LS247 input A connected to Arduino pin 5
    PORTD |= (1 << PORTD5);
  }
  if ((d & 0x2) > 0) {
    // 74LS247 input B connected to Arduino pin 2
    PORTD |= (1 << PORTD2);
  }
  if ((d & 0x4) > 0) {
    // 74LS247 input C connected to Arduino pin 3
    PORTD |= (1 << PORTD3);
  }
  if ((d & 0x8) > 0) {
    // 74LS247 input D connected to Arduino pin 4
    PORTD |= (1 << PORTD4);
  }
}

// Interrupt service routine is invoked when timer2 overflows.
ISR(TIMER2_OVF_vect) {
  TCNT2 = 0;
  if (digitToggle == 0) {
    PORTD |= (1 << PORTD7);  // turn off digit 2
    setOutput(d1);
    PORTD &= ~(1 << PORTD6); // turn on digit 1
  } else {
    PORTD |= (1 << PORTD6);  // turn off digit 1
    setOutput(d2);
    PORTD &= ~(1 << PORTD7); // turn on digit 2
  }
  digitToggle = ~digitToggle;
}

