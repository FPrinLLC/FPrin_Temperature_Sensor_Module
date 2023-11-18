#include <Adafruit_NeoPixel.h>
#include <bluefruit.h>
#include <Wire.h>
#include "IEEE11073float.h"

BLEService BLETemperatureSensor = BLEService(0x181A);
BLECharacteristic Temperature = BLECharacteristic(0x2A6E);
BLEDis bledis;
                              
int SENSOR_ADDRESS = 0x48;
uint8_t tempData[2];
float measuredTemperature = 0.0;

void setup() {
  byte error;
  Wire.begin();
  Serial.begin(115200);

  Serial.println("BLE Temperature Sensor Module v1.01");

  // Check that Temperature Sensor is connected
  Wire.beginTransmission(SENSOR_ADDRESS);
  error = Wire.endTransmission();

  Serial.print("Attempting to Initialize with I2C Device at address 0x");
  Serial.println(SENSOR_ADDRESS, HEX);

  if (error != 0) {
    Serial.print("Error - Could not interface with I2C device at address 0x");
    Serial.println(SENSOR_ADDRESS, HEX);
    while (1);
  }
  Serial.println("Temperature Sensor Initialization successful!");
  // begin initialization
  
  Bluefruit.begin();
  Bluefruit.setName("Temperature Sensor");

  bledis.setModel("Temperature Sensor Module");
  bledis.setManufacturer("FPrin LLC");
  bledis.begin();

  setupTempSensorBLE();
  
  Bluefruit.Advertising.addService(BLETemperatureSensor);
  Bluefruit.Advertising.addName();
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.start(0);

  Serial.println("Bluetooth® device active, waiting for connections...");
  digitalWrite(LED_BLUE, LOW);
}

void loop() {

  // if a central is connected to the peripheral:
  if (Bluefruit.connected()) {
    // update temperature value every second
    // while the central is connected:
    measuredTemperature = measureTemperature(SENSOR_ADDRESS);
    Temperature.notify(tempData,sizeof(tempData));
    printTemperature();
  }
  delay(1000);
}

float measureTemperature(int address) {
  float Temperature;
  Wire.requestFrom(address, 2);
  byte buff[2];
  Wire.readBytes(buff, 2);
  //Serial.print(buff[0],DEC);
  //Serial.print("  ");
  //Serial.println(buff[1],DEC);
  if (bitRead(buff[0], 7) == 1) { // If the first bit is a 1
    Temperature = (~((buff[0] << 8) + buff[1]) + 1) * -0.0078125; // Compute Negative Temperature
  } else { // If the first bit is a 0
    Temperature = ((buff[0] << 8) + buff[1]) * 0.0078125; // Compute Positive Temperature
  }
  tempData[0] = buff[0];
  tempData[1] = buff[1];
  return Temperature;
}

void printTemperature() {
  Serial.println(measuredTemperature, 2);
}

void readConfigRegister(int address) {
  Serial.println("Press any key to fetch config byte");
  while (!Serial.available()) {}
  Serial.read();
  Serial.println("Configuration bytes: ");

  Wire.beginTransmission(address);
  Wire.write(B00000001);
  Wire.endTransmission();

  Wire.requestFrom(address, 2);
  byte buff[2];
  Wire.readBytes(buff, 2);

  Wire.beginTransmission(address);
  Wire.write(B00000000);
  Wire.endTransmission();
  Serial.println(buff[0], BIN);
  Serial.println(buff[1], BIN);
}

void setupTempSensorBLE() {
  // Start Temperature Sensor Service
  BLETemperatureSensor.begin();

  // Setup Temperature Measurement Characteristic
  Temperature.setProperties(CHR_PROPS_NOTIFY);
  Temperature.setPermission(SECMODE_OPEN, SECMODE_NO_ACCESS);
  Temperature.setFixedLen(2);
  Temperature.begin();
  Temperature.write(tempData,sizeof(tempData));
}
