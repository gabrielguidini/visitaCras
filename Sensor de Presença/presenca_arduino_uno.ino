const int pinoPIR = 3; 
const int pinoLED = 7; 
int contadorSensor = 0;


void setup(){
  Serial.begin(9600);
  pinMode(pinoLED, OUTPUT); 
  pinMode(pinoPIR, INPUT); 
}
void loop(){
  byte statusSensor = digitalRead(pinoPIR);
  if(statusSensor == 1){
    digitalWrite(pinoLED, HIGH);
    Serial.println(contadorSensor);
    contadorSensor = contadorSensor + 1;
  }
  else{
    digitalWrite(pinoLED, LOW);
  }
  delay(1000);
}