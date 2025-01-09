/********************************************************************************************************************************
 * Code for a LoRa development field test device designed for testing LoRa communication and applications
 * Used in the "Campus of Things" project linked to "Escola Superior de Tecnologia - Polytechnic Institute of Setúbal"
 * Field test device composed by the following equipment: Arduino Uno, RFM95 (LoRa), HC-06 (Bluetooth) and DS18B20 (Temperature)
 * Copyright (c) 2024 - Bernardo Milheiro
********************************************************************************************************************************/
////////////////////////
// - Bluetooth       //
//////////////////////
#include <SoftwareSerial.h>
// Sets the RX and TX pins connected to HC-06.
SoftwareSerial bluetooth(8, 9);  // (RX, TX)

////////////////////////
// - Temperature     //
//////////////////////
#include <OneWire.h>
#include <DallasTemperature.h>
// DS18B20 Temperature Sensor Setup.
OneWire oneWire(4);
DallasTemperature temperatureObj(&oneWire);

////////////////////////
// - LoRa            //
//////////////////////
#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>
// Network Session Key, App Session Key and Device Adress Setup.
static const PROGMEM u1_t NWKSKEY[16] = { 0xFE, 0x09, 0xA6, 0xE3, 0xA4, 0x89, 0x87, 0x66, 0x8C, 0xE8, 0x06, 0xCF, 0xAA, 0xCF, 0x39, 0x36 };
static const u1_t PROGMEM APPSKEY[16] = { 0x77, 0x63, 0xF5, 0xF5, 0x9A, 0x6D, 0x9D, 0x23, 0x8C, 0xDE, 0xAE, 0x97, 0x58, 0x57, 0x74, 0x1A };
static const u4_t DEVADDR = 0x260BD6F8;
// Declares job variable.
static osjob_t sendjob;
// Sets message max size and initial message as "Hello".
uint8_t dataSend[5];
String dataString = "Hello";
// Sets interval of transmission in seconds.
const unsigned TX_INTERVAL = 20;
// Declares rssi variable.
int16_t rssi = 0;

// Pin mapping RFM95.
const lmic_pinmap lmic_pins = {
  .nss = 10,
  .rxtx = LMIC_UNUSED_PIN,
  .rst = 5,
  .dio = { 2, 3, LMIC_UNUSED_PIN },
};

// RSSI Calculation Function.
int16_t getRssi() {
// Returns correct RSSI (dBm) value of the last received packet.
// Calculation per SX1276 datasheet rev.7 §5.5.5.
#define RSSI_OFFSET 64
#define SX1276_FREQ_LF_MAX 525000000
#define SX1272_RSSI_ADJUST -139
#define SX1276_RSSI_ADJUST_LF -164
#define SX1276_RSSI_ADJUST_HF -157
  int16_t snr = (LMIC.snr * 10) / 40;
  int16_t rssiTemp = LMIC.rssi - RSSI_OFFSET;
  int16_t rssiAdjust;
  if (LMIC.freq > SX1276_FREQ_LF_MAX) {
    rssiAdjust = SX1276_RSSI_ADJUST_HF;
  } else {
    rssiAdjust = SX1276_RSSI_ADJUST_LF;
  }
  int16_t packetRssi = LMIC.rssi + 125 - RSSI_OFFSET;
  if (snr < 0) {
    rssiTemp = rssiAdjust + packetRssi + snr;
  } else {
    rssiTemp = rssiAdjust + (16 * packetRssi) / 15;
  }
  return rssiTemp;
}

// Temperature Reading Function.
void getTemp() {
  // Get temperature value from sensor and assigns that data.
  temperatureObj.requestTemperatures();
  float temperature = temperatureObj.getTempCByIndex(0);
  // If temperature is within valid measurements.
  if (temperature > -55 && temperature < 125) {
    // Turns float temperature value into String.
    dataString = String(temperature);
  } else {
    // Prints and assigns "Failure" as data.
    Serial.println(F("Temperature (DS18B20) reading failure."));
    dataString = "Failure";
  }
}

// Data Transmission Function.
void do_send(osjob_t* j) {
  digitalWrite(13, LOW);  // Turns the LED off
  // Check if there's any current TX/RX job running.
  if (LMIC.opmode & OP_TXRXPEND) {
    Serial.println(F("There is a current TX/RX job running"));
  } else {
    // Upstream data transmission at the next possible time.
    dataString.toCharArray(dataSend, dataString.length() + 1);
    LMIC_setTxData2(1, dataSend, dataString.length(), 0);
  }
}

// Event Behaviour Function.
void onEvent(ev_t ev) {
  // Gets time tag.
  os_getTime();
  // Switch function for occored event. For development purposes, only "EV_TXCOMPLETE" event was considered for the system behaviour.
  switch (ev) {
    // If transmission is successful.
    case EV_TXCOMPLETE:
    // Prints sent data.
      Serial.println(F("\n--------------------------------"));
      Serial.println(F("Device name: fieldTester"));
      Serial.print(F("Data Sent: "));
      Serial.println(dataString);
      Serial.println(F("EV_TXCOMPLETE (includes waiting for RX windows)"));
      // Prints recieved data if its size is different than 0.
      if (LMIC.dataLen) {
        digitalWrite(13, HIGH);  // Turns the LED on
        rssi = getRssi();        // Gets last downlink RSSI value.
        Serial.print(F("Data Received: "));
        Serial.write(LMIC.frame + LMIC.dataBeg, LMIC.dataLen);
        Serial.print(F("\n"));
      }
      // Prints RSSI value if it's between 0 and -150 (valid values) and sends it via Bluetooth.
      if (rssi < 0 && rssi > -150) {
        Serial.print(F("Last downlink RSSI: "));
        Serial.print(rssi);
        bluetooth.print(rssi);
      }
      // Gets current temperature value.
      getTemp();
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
  // Serial, Bluetooth and Temperature Sensor initialization.
  Serial.begin(9600);
  bluetooth.begin(9600);
  temperatureObj.begin();
  // Initialize digital pin 13 as an output.
  pinMode(13, OUTPUT);
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
  LMIC_setupChannel(0, 868100000, DR_RANGE_MAP(DR_SF12, DR_SF7), BAND_CENTI);
  LMIC_setupChannel(1, 868300000, DR_RANGE_MAP(DR_SF12, DR_SF7), BAND_CENTI);
  LMIC_setupChannel(2, 868500000, DR_RANGE_MAP(DR_SF12, DR_SF7), BAND_CENTI);
  LMIC_setupChannel(3, 867100000, DR_RANGE_MAP(DR_SF12, DR_SF7), BAND_CENTI);
  LMIC_setupChannel(4, 867300000, DR_RANGE_MAP(DR_SF12, DR_SF7), BAND_CENTI);
  LMIC_setupChannel(5, 867500000, DR_RANGE_MAP(DR_SF12, DR_SF7), BAND_CENTI);
  LMIC_setupChannel(6, 867700000, DR_RANGE_MAP(DR_SF12, DR_SF7), BAND_CENTI);
  LMIC_setupChannel(7, 867900000, DR_RANGE_MAP(DR_SF12, DR_SF7), BAND_CENTI);
  LMIC_setupChannel(8, 868800000, DR_RANGE_MAP(DR_FSK, DR_FSK), BAND_MILLI);
  // Disable link check validation.
  LMIC_setLinkCheckMode(0);
  // Gateway is configured to use SF9 and SF12 for its RX2 window.
  LMIC.dn2Dr = DR_SF12;
  // Set data rate and transmit power for uplink.
  LMIC_setDrTxpow(DR_SF9, 7);
  // Start job.
  do_send(&sendjob);
}

void loop() {
  os_runloop_once();
}