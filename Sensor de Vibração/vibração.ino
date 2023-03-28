

const int pinoLed = 2; //PINO DIGITAL UTILIZADO PELO LED
const int pinoSensor = 5; //PINO DIGITAL UTILIZADO PELO SENSOR

void setup(){
  pinMode(pinoSensor, INPUT); //DEFINE O PINO COMO ENTRADA
  pinMode(pinoLed, OUTPUT); //DEFINE O PINO COMO SAÍDA
  digitalWrite(pinoLed, LOW); //LED INICIA DESLIGADO
  Serial.begin(9600);
}

void loop(){
  if(digitalRead(pinoSensor) == HIGH){ //SE A LEITURA DO PINO FOR IGUAL A HIGH, FAZ
      Serial.println("Tem vibração");
      digitalWrite(pinoLed, HIGH); //ACENDE O LED
      delay(2000);  
  }else{ //SENÃO, FAZ
    digitalWrite(pinoLed, LOW); //APAGA O LED
    Serial.println("Sem vibração");
    
  }
}