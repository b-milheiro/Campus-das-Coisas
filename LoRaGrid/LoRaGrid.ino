#include <SoftwareSerial.h>
#include <PZEM004Tv30.h>
#include <LowPower.h>
#define APPKEY "C53F64126BF6C5976155076CEF798775"
#define DATA_RATE "0"
#define AUTH_METHOD "LWOTAA"
#define PORT "1"
// LoRa Pin 10 RX e Pin 11 TX
SoftwareSerial Serial_Lora(10, 11);
// PZEM Pin 5 RX e Pin 6 TX
PZEM004Tv30 pzem(5, 6);
/** 
* Configurações iniciais 
* Serial - PZEM, SoftwareSerial - LoRa 
*/
void setup() {
  Serial.println("CONFIGURATION: Start.");
  Serial.println("CONFIGURATION: #### PZEM Module ####");
  if (pzem.readAddress() != NAN) {
    Serial.println("CONFIGURATION: PZEM Module Ready.");
  } else {
    Serial.println("CONFIGURATION: PZEM Module ERROR: No Available Address");
  }
  Serial.begin(9600);
  Serial.println("CONFIGURATION: #### LoRa-E5 Module ####");
  Serial_Lora.begin(9600);
  while (!setupLoraDevice())
    ;
  Serial.println("CONFIGURATION: LoRa-E5 Module Ready.");
  Serial.println("CONFIGURATION: Done.\n");
  pzem.resetEnergy();
}
4 void loop() {
  readPowerMetrics();
  LowPower.powerDown(SLEEP_4S, ADC_OFF, BOD_OFF);
}
/** 
* Esta função vai enviar comandos AT para o módulo LoRa 
* @request - este é o comando AT que o módulo de transmissão vai executar 
* @expectedResponse - é a resposta expectável do módulo depois de executar o comando 
AT 
*/
void sendATCommand(String request, String expectedResponse) {
  Serial_Lora.println(request);
  while (Serial_Lora.available()) {
    String response = Serial_Lora.readStringUntil('\n');
    Serial.println("Serial_Lora: " + response);
    if (response.indexOf(expectedResponse) != -1) {
      break;
    }
  }
}
/** 
* Esta função vai configurar o módulo LoRa-E5, garantido que fique operacional para 
comunicar com o servidor TTN 
* É feito um reset inicial para garantir que todas as definições sejam feitas corretamente, 
após isso é configurado 
* o modo de autenticação com servidor TTN para LWOTAA, a appkey da aplicação TTN 
e feito o pedido de join para a mesma. 
* Se o pedido de join falhar, o led vermelho é ligado 
*/
boolean setupLoraDevice() {
  A .5 sendATCommand("AT+RESET", "OK");
  delay(3000);  // Garantir 3s para que todas as configurações sejam limpas
  sendATCommand("AT+MODE=" + String(AUTH_METHOD), "OK");
  delay(1000);  // Garantir 1s para que a alteração seja aplicada
  sendATCommand("AT+KEY=APPKEY,\"" + String(APPKEY) + "\"", "OK");
  delay(1000);
  sendATCommand("AT+PORT=\"" + String(PORT) + "\"", "OK");  // Configurar Data
  Rate para SF9 BW125kHz
    delay(1000);
  sendATCommand("AT+ADR=OFF", "OK");  // Configurar Data Rate para SF9
  BW125kHz
    delay(1000);
  sendATCommand("AT+DR=" + String(DATA_RATE), "OK");  // Configurar Data Rate
  para SF9 BW125kHz
    delay(1000);
  sendATCommand("AT+ID", "OK");
  delay(1000);
  sendATCommand("AT+JOIN", "OK");
  if (isDeviceConnectedToTTN()) {
    Serial.println("CONFIGURATION: Device joined TTN");
    return true;
  } else {
    Serial.println("CONFIGURATION: Failed to join TTN");
    return false;
  }
}
/** 
* Esta função vai retornar um boolean a indicar se o sensor está conectado ao servidor TTN 
* Vai esperar 30s para receber uma mensagem de resposta do servidor +EVT:JOINED, se 
receber antes da janela de tempo terminar 
* então retorna true 
* Caso contrário retorna false 
6 
*/
bool isDeviceConnectedToTTN() {
  unsigned long startTime = millis();
  String response = "";
  while (millis() - startTime < 30000) {
    if (Serial_Lora.available()) {
      String response = Serial_Lora.readStringUntil('\n');
      if (response.indexOf("+JOIN: Network joined") != -1) {
        return true;
      }
    }
  }
  return false;
}
/** 
* Esta função vai enviar comandos para o medidor PZEM para ler cada uma das métricas 
* Se o valor obtido do sensor para uma das métricas for "NAN" então a variável dessa 
métrica é inicializada com o valor 
* de omissão "0" 
* Ao fim de obter todas as métricas, vai ser construido um payload em formato JSON para 
ser enviado para a aplicação TTN 
*/
void readPowerMetrics() {
  float voltage = pzem.voltage();
  if (!isnan(voltage)) {
    Serial.println(voltage);
    Serial.println("Tensão: " + String(voltage) + "V");
  } else {
    voltage = 0;
    Serial.println("Error reading voltage");
  }
  A .7 float current = pzem.current();
  if (!isnan(current)) {
    Serial.println("Corrente: " + String(current) + "A");
  } else {
    current = 0;
    Serial.println("Error reading current");
  }
  float power = pzem.power();
  if (!isnan(power)) {
    Serial.println("Potência: " + String(power) + "W");
  } else {
    power = 0;
    Serial.println("Error reading power");
  }
  float energy = pzem.energy();
  if (!isnan(energy)) {
    Serial.println("Energia: " + String(energy) + "kWh");
  } else {
    energy = 0;
    Serial.println("Error reading energy");
  }
  float frequency = pzem.frequency();
  if (!isnan(frequency)) {
    Serial.println("Frequencia: " + String(frequency, 1) + "Hz");
  } else {
    frequency = 0;
    Serial.println("Error reading frequency");
  }
  float pf = pzem.pf();
  if (!isnan(pf)) {
    8 Serial.println("Fator De Potência: " + String(pf, 1));
  } else {
    pf = 0;
    Serial.println("Error reading power factor");
  }
  float power_apparent = voltage * current;
  float power_reactive = power_apparent * sin(acos(pf));
  String measurement = String(voltage) + "," + String(current) + "," + String(power) + ","
                       + String(power_reactive) + "," + String(power_apparent) + "," + String(energy) + "," + String(frequency) + "," + String(pf);
  Serial.println(measurement);
  sendATCommand("AT+CMSG=\"" + measurement + "\"", "OK");
}