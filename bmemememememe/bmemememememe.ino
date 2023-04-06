#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <SSD1306.h>

//Deixe esta linha descomentada para compilar o Master
//Comente ou remova para compilar o Slave
#define MASTER

#define RST 14  // GPIO14 RESET
#define DI00 26 // GPIO26 IRQ(Interrupt Request)

#define BAND 915E6 //Frequência do radio - exemplo : 433E6, 868E6, 915E6

//Constante para informar ao Slave que queremos os dados
const String GETDATA = "getdata";
//Constante que o Slave retorna junto com os dados para o Master
const String SETDATA = "setdata=";

//Estrutura com os dados do sensor
typedef struct {
  double temperature;
  double pressure;
  double humidity;
}Data;

//Variável para controlar o display
SSD1306 display(0x3c, SDA, SCL);

void setupDisplay(){
  //O estado do GPIO16 é utilizado para controlar o display OLED
  pinMode(16, OUTPUT);
  //Reseta as configurações do display OLED
  digitalWrite(16, LOW);
  //Para o OLED permanecer ligado, o GPIO16 deve permanecer HIGH
  //Deve estar em HIGH antes de chamar o display.init() e fazer as demais configurações,
  //não inverta a ordem
  digitalWrite(16, HIGH);

  //Configurações do display
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
}

//Configurações iniciais do LoRa
void setupLoRa(){ 
  //Inicializa a comunicação
  SPI.begin(SCK, MISO, MOSI, SS);
  LoRa.setPins(SS, RST, DI00);

  //Inicializa o LoRa
  if (!LoRa.begin(BAND, true)){
    //Se não conseguiu inicializar, mostra uma mensagem no display
    display.clear();
    display.drawString(0, 0, "Erro ao inicializar o LoRa!");
    display.display();
    while (1);
  }

  //Ativa o crc
  LoRa.enableCrc();
  //Ativa o recebimento de pacotes
  LoRa.receive();
}

//Compila apenas se MASTER estiver definido no arquivo principal
#ifdef MASTER

//Intervalo entre os envios
#define INTERVAL 500

//Tempo do último envio
long lastSendTime = 0;

void setup(){
  Serial.begin(115200);
  //Chama a configuração inicial do display
  setupDisplay();
  //Chama a configuração inicial do LoRa
  setupLoRa();

  display.clear();
  display.drawString(0, 0, "Master");
  display.display();
}

void loop(){
  //Se passou o tempo definido em INTERVAL desde o último envio
  if (millis() - lastSendTime > INTERVAL){
    //Marcamos o tempo que ocorreu o último envio
    lastSendTime = millis();
    //Envia o pacote para informar ao Slave que queremos receber os dados
    send();
  }

  //Verificamos se há pacotes para recebermos
  receive();
}

void send(){
  //Inicializa o pacote
  LoRa.beginPacket();
  //Envia o que está contido em "GETDATA"
  LoRa.print(GETDATA);
  //Finaliza e envia o pacote
  LoRa.endPacket();
}

void receive(){
  //Tentamos ler o pacote
  int packetSize = LoRa.parsePacket();
  
  //Verificamos se o pacote tem o tamanho mínimo de caracteres que esperamos
  if (packetSize > SETDATA.length()){
    String received = "";
    //Armazena os dados do pacote em uma string
    for(int i=0; i<SETDATA.length(); i++){
      received += (char) LoRa.read();
    }

    //Se o cabeçalho é o que esperamos
    if(received.equals(SETDATA)){
      //Fazemos a leitura dos dados
      Data data;
      LoRa.readBytes((uint8_t*)&data, sizeof(data));
      //Mostramos os dados no display
      showData(data);
    }
  }
}

void showData(Data data){
  //Tempo que demorou para o Master criar o pacote, enviar o pacote,
  //o Slave receber, fazer a leitura, criar um novo pacote, enviá-lo
  //e o Master receber e ler
  String waiting = String(millis() - lastSendTime);
  //Mostra no display os dados e o tempo que a operação demorou
  display.clear();
  display.drawString(0, 0, String(data.temperature) + " C");
  display.drawString(0, 16, String(data.pressure) + " Pa");
  display.drawString(0, 32, String(data.humidity) + "%");
  display.drawString(0, 48, waiting + " ms");
  display.display();
}

#endif