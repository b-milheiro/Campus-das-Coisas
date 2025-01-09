/**********************************************************************************************************************************************
 * Code for a LoRa development sensor device designed for testing LoRa communication and applications
 * Used in the "Campus of Things" and "ReSens" projects linked to "Escola Superior de Tecnologia - Polytechnic Institute of Setúbal"
 * Sensor device composed by the following equipment: Arduino Uno, RFM95 (LoRa), DHT22 (Hum & Temp), FC-28 (Soil Hum) and DS18B20 (Soil Temp)
 * Copyright (c) August 2024 - Bernardo Milheiro e João José
**********************************************************************************************************************************************/
////////////////////////
// - Sensors         //
//////////////////////
#include <DHT.h>
#include <OneWire.h>
#include <DallasTemperature.h>
// Pin Mapping Sensors.
#define DHTPIN 4
#define DHTTYPE DHT22
#define FC28PIN A0
#define DS18B20PIN 7
// Object creation for digital sensors.
DHT dht(DHTPIN, DHTTYPE);
OneWire oneWire(DS18B20PIN);
DallasTemperature sensors(&oneWire);


////////////////////////
// - LoRa            //
//////////////////////
#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>

// Network Session Key, App Session Key and Device Adress Setup.
static const PROGMEM u1_t NWKSKEY[16] = { 0xA7, 0x0A, 0xE1, 0x18, 0xB6, 0x47, 0xB3, 0x45, 0xB9, 0x7C, 0x77, 0xC1, 0xFA, 0x93, 0x93, 0x9C };
static const u1_t PROGMEM APPSKEY[16] = { 0x88, 0x88, 0x05, 0x76, 0x6A, 0xCD, 0x6F, 0x47, 0xDF, 0x00, 0x60, 0x01, 0x38, 0x37, 0x78, 0xDC };
static const u4_t DEVADDR = 0x260BDAC7;

// Declares job variable.
static osjob_t sendjob;
// Sets size and initial message as "Hello".
uint8_t dataSend[26];
String dataString = "Hello";
// Sets interval of transmission in seconds.
const unsigned TX_INTERVAL = 10;

// Pin mapping RFM95.
const lmic_pinmap lmic_pins = {
  .nss = 10,
  .rxtx = LMIC_UNUSED_PIN,
  .rst = 5,
  .dio = { 2, 3, LMIC_UNUSED_PIN },
};

// Reading and Printing values function.
void getValues() {
  Serial.println(F("Next values to send:"));
  // Read DHT22 humidity value in percentage.
  float h = dht.readHumidity();
  // Read DHT22 temperature value in Celsius.
  float t = dht.readTemperature();
  // Checks if reading returned valid values and if so, prints the values.
  if (isnan(h) || isnan(t)) {
    h = 999.99;
    t = -999.99;
    Serial.print(F("Humidity or Temperature (DHT22) reading failure"));
  } else {
    Serial.print(F("Humidity: "));
    Serial.print(h);
    Serial.print(F(" %\t"));
    Serial.print(F("Temperature: "));
    Serial.print(t);
    Serial.println(F(" Celsius"));
  }
  // Read FC-28 humidity value in percentage.
  int soilHum = map((analogRead(FC28PIN)), 1023, 0, 0, 100);
  if (soilHum == 0) {
    Serial.print(F("Soil Humidity (FC-28) reading failure\t"));
  } else {
    Serial.print(F("Soil Humidity: "));
    Serial.print(soilHum);
    Serial.print(F(" %\t"));
  }
  // Read DS18B20 temperature value in Celsius.
  float soilTemp = sensors.getTempCByIndex(0);
  if (soilTemp < -55 || soilTemp > 125) {
    Serial.print(F("Soil Temperature (DS18B20) reading failure"));
  } else {
    Serial.print(F("Soil Temperature: "));
    Serial.print(soilTemp);
    Serial.print(F(" Celsius"));
  }
  // Assigns the read values to the variable "dataString".
  dataString = String(h) + "|" + String(t) + "|" + String(soilHum) + "|" + String(soilTemp);
}

// Data transmission function.
void do_send(osjob_t* j) {
  // Check if there is not a current TX/RX job running.
  if (LMIC.opmode & OP_TXRXPEND) {
    Serial.println(F("There is a current TX/RX job running"));
  } else {
    // Prepare upstream data transmission at the next possible time.
    dataString.toCharArray(dataSend, dataString.length() + 1);
    LMIC_setTxData2(1, dataSend, dataString.length(), 0);
    Serial.println(F("\n--------------------------------------"));
    Serial.println(F("Device name: sensorDevice"));
    Serial.print(F("Data Sent: "));
    Serial.println(dataString);
  }
}

// Event behaviour function.
void onEvent(ev_t ev) {
  // Prints time tag.
  Serial.print(os_getTime());
  Serial.print(": ");
  // Switch function for occored event. For development purposes, only "EV_TXCOMPLETE" event was considered for the system behaviour.
  switch (ev) {
    // If transmission is successful, gets values and checks if there's any data to recieve.
    case EV_TXCOMPLETE:
      Serial.println(F("EV_TXCOMPLETE (includes waiting for RX windows)\n"));
      getValues();
      if (LMIC.dataLen) {
        Serial.print(F("Data Received: "));
        Serial.write(LMIC.frame + LMIC.dataBeg, LMIC.dataLen);
      }
      // Schedule next transmission.
      os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(TX_INTERVAL), do_send);
      break;
    // In case other event occurs prints a message.
    default:
      Serial.println(F("Expected event didn't ocorred!"));
      break;
  }
}


////////////////////////
// - Setup & Loop    //
//////////////////////
void setup() {
  // Serial initialization.
  Serial.begin(9600);
  // Sensors initialization.
  dht.begin();
  pinMode(FC28PIN, INPUT);
  sensors.begin();
  // LMIC initialization.
  os_init();
  // Reset the MAC state. Session and pending data transfers will be discarded.
  LMIC_reset();
  // Set static session parameters.
  uint8_t appskey[sizeof(APPSKEY)];
  uint8_t nwkskey[sizeof(NWKSKEY)];
  memcpy_P(appskey, APPSKEY, sizeof(APPSKEY));
  memcpy_P(nwkskey, NWKSKEY, sizeof(NWKSKEY));
  LMIC_setSession(0x1, DEVADDR, nwkskey, appskey);
  // Set up the channels used by the Things Network.
  LMIC_setupChannel(0, 868100000, DR_RANGE_MAP(DR_SF12, DR_SF7), BAND_CENTI);  // g-band
  LMIC_setupChannel(1, 868300000, DR_RANGE_MAP(DR_SF12, DR_SF7), BAND_CENTI);  // g-band
  LMIC_setupChannel(2, 868500000, DR_RANGE_MAP(DR_SF12, DR_SF7), BAND_CENTI);  // g-band
  LMIC_setupChannel(3, 867100000, DR_RANGE_MAP(DR_SF12, DR_SF7), BAND_CENTI);  // g-band
  LMIC_setupChannel(4, 867300000, DR_RANGE_MAP(DR_SF12, DR_SF7), BAND_CENTI);  // g-band
  LMIC_setupChannel(5, 867500000, DR_RANGE_MAP(DR_SF12, DR_SF7), BAND_CENTI);  // g-band
  LMIC_setupChannel(6, 867700000, DR_RANGE_MAP(DR_SF12, DR_SF7), BAND_CENTI);  // g-band
  LMIC_setupChannel(7, 867900000, DR_RANGE_MAP(DR_SF12, DR_SF7), BAND_CENTI);  // g-band
  LMIC_setupChannel(8, 868800000, DR_RANGE_MAP(DR_FSK, DR_FSK), BAND_MILLI);   // g2-band
  // Disable link check validation.
  LMIC_setLinkCheckMode(0);
  // TTN uses SF9 for its RX2 window.
  LMIC.dn2Dr = DR_SF9;
  // Set data rate and transmit power for uplink.
  LMIC_setDrTxpow(DR_SF7, 20);
  // Start job.
  do_send(&sendjob);
}

void loop() {
  os_runloop_once();
}
