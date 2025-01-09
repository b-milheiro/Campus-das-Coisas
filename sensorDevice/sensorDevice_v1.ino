#include <SoftwareSerial.h>
#define APPKEY "793CD4FC815AF14D969C585792FAC61C"

// LoRa Pin 10 RX and Pin 11 TX
SoftwareSerial Serial_Lora(2, 7);
String oi = "oi";

void setup() {
  Serial.begin(9600);
  Serial_Lora.begin(9600);
  Serial.println("CONFIGURATION: Start.");
  setupLoraDevice();
  Serial.println("CONFIGURATION: Done.\n");
}

void loop() {
  if (Serial.available()) {
    String request = Serial.readStringUntil('\n');
    sendATCommand(request, "OK");
  }

  if (Serial_Lora.available()) {
    String response = Serial_Lora.readStringUntil('\n');
    Serial.println("Serial_Lora: " + response);
  }

  sendATCommand("AT+CMSG=\"" + oi + "\"", "OK");

  //readPowerMetrics();
  delay(10000);
  //LowPower.idle(SLEEP_8S, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF, SPI_OFF, USART0_OFF, TWI_OFF);
}

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

void setupLoraDevice() {
  sendATCommand("AT+RESET", "OK");
  delay(2000); // Give 3s for E5 reset
  sendATCommand("AT+MODE=LWOTAA", "OK"); // Set authentication mode to 
  delay(1000); // Give 1s for mode set
  sendATCommand("AT+KEY=APPKEY,\"" + String(APPKEY) + "\"", "OK");
  delay(1000); // Give 1s for appkey set
  sendATCommand("AT+ID", "OK");
  delay(1000);
  sendATCommand("AT+JOIN", "OK"); // Join TTN

  if(waitForDeviceToJoinTTN()) {
    Serial.println("CONFIGURATION: Device joined TTN");
  } else {
    Serial.println("CONFIGURATION: Failed to join TTN");
  }
}

bool waitForDeviceToJoinTTN() {
  unsigned long startTime = millis();
  String response = "";

  while (millis() - startTime < 30000) {
    if (Serial_Lora.available()) {
      String response = Serial_Lora.readStringUntil('\n');

      if (response.indexOf("+JOIN: Network joined\n") != -1) {
        return true;
      }
    }
  }

  return false; // Retorna falso se não receber +EVT:JOINED
}
