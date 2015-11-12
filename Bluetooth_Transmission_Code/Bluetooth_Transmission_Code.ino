#define SENSOR_ANALOG_PIN 0 
int led=13;

int tx=1;
int rx=0;
char inSerial[15];

void setup(){
  Serial.begin(9600);
  pinMode(led,OUTPUT);
  pinMode(tx, OUTPUT);
  pinMode(rx,INPUT);
}

void loop(){
  int i=0;
  delay(500);
  while(Serial.available() >0){
    inSerial[i] = Serial.read();
    i++;
  }
  inSerial[i] = '\0';
  if(i>0) Check_Protocol(inSerial);
}

void Check_Protocol(char inStr[]){
  int m=0;
  Serial.println(inStr);
  if(strcmp(inStr, "off") == 0)
  {
    digitalWrite(led, LOW);
    Serial.println("LED OFF");
    for(m=0; m<15; m++){
      inSerial[m] =0; inStr[m] =0; 
    }
   }
   else if(strcmp(inStr, "on") == 0)
   {
    digitalWrite(led, HIGH);
    Serial.println("LED ON");
    for(m=0; m<15; m++){
      inSerial[m] =0; inStr[m] =0; 
    }
   }
   else if(strcmp(inStr, "temp") == 0)
   {
    byte reading = analogRead(SENSOR_ANALOG_PIN);
    int mv = ((float)(reading/1023.0)) * 5000;
    int temp = mv/10;
    int cel = (int)temp;
    Serial.println(cel);
   }
   else{
    for(m=0; m< 15; m++){
      inSerial[m] =0;
      inStr[m] =0;
      }
   }
}

