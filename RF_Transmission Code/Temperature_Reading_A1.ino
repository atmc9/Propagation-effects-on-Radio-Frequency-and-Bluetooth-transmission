#define SENSOR_ANALOG_PIN 0
#define LED_PIN 13
#define PACKET_HEADER_SIZE 3
#define ADDR 1
const byte packetHeader[PACKET_HEADER_SIZE] = {0x8F, 0xAA, ADDR};   /*10001111 , 10101010, 00000001 */
byte temp;

void setup() {
  Serial.begin(1200);
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  digitalWrite(LED_PIN, HIGH);
  readTemp();
  writeByte(temp);
  digitalWrite(LED_PIN, LOW);
  delay(1000);
}

void readTemp() {
  byte reading = analogRead(SENSOR_ANALOG_PIN);
  int mv = ((float)(reading/1023.0)) * 5000;
  temp = mv / 10;
  int cel = (int)temp; 
  int left = temp/10;
  int right = temp%10;
  Serial.print("Data sent");
  Serial.print(cel);
}

// Sends an unsigned int over the RF network                                                            
void writeByte(byte val) {
  byte checksum = (packetHeader[0] ^ packetHeader[1] ^ packetHeader[2] ^ val);
  Serial.write(packetHeader, PACKET_HEADER_SIZE);
  Serial.write(val);
  Serial.write(checksum);
}
