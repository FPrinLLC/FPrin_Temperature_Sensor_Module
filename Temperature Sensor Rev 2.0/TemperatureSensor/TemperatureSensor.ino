#include <Adafruit_MAX1704X.h>
#include <ArduinoBLE.h>
#include <Wire.h>


BLEService BLETemperatureSensor("181A");

// Bluetooth® Low Energy Battery Level Characteristics
BLEFloatCharacteristic Temperature("2A6E",  // standard 16-bit characteristic UUID
                                   BLERead | BLENotify); // remote clients will be able to get notifications if this characteristic changes

int SENSOR_ADDRESS = 0x48;
int oldBatteryLevel = 0;  // last battery level reading from analog input

float previousTemperature = 0;
float measuredTemperature = 0;

unsigned long currentMillis, previousMillis;

void setup() {
  byte error;
  Wire.begin();
  Serial.begin(115200);
  while (!Serial);

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


  pinMode(LED_BUILTIN, OUTPUT); // initialize the built-in LED pin to indicate when a central is connected

  // begin initialization
  if (!BLE.begin()) {
    Serial.println("starting BLE failed!");

    while (1);
  }

  /* Set a local name for the Bluetooth® Low Energy device
     This name will appear in advertising packets
     and can be used by remote devices to identify this Bluetooth® Low Energy device
     The name can be changed but maybe be truncated based on space left in advertisement packet
  */
  BLE.setLocalName("Temperature Sensor");
  BLE.setAdvertisedService(BLETemperatureSensor); // add the service UUID
  BLETemperatureSensor.addCharacteristic(Temperature); // add the battery level characteristic
  BLE.addService(BLETemperatureSensor); // Add the battery service
  Temperature.writeValue(previousTemperature); // set initial value for this characteristic

  /* Start advertising Bluetooth® Low Energy.  It will start continuously transmitting Bluetooth® Low Energy
     advertising packets and will be visible to remote Bluetooth® Low Energy central devices
     until it receives a new connection */

  // start advertising
  BLE.advertise();

  Serial.println("Bluetooth® device active, waiting for connections...");
  previousMillis = millis();
}

void loop() {
  // wait for a Bluetooth® Low Energy central
  BLEDevice central = BLE.central();

  // if a central is connected to the peripheral:
  if (central) {
    Serial.print("Connected to central: ");
    // print the central's BT address:
    Serial.println(central.address());
    // turn on the LED to indicate the connection:
    digitalWrite(LED_BUILTIN, HIGH);

    // update temperature value every second
    // while the central is connected:
    while (central.connected()) {
      measuredTemperature = measureTemperature(SENSOR_ADDRESS);
      Temperature.writeValue(measuredTemperature);
      printTemperature();
      delay(1000);
    }
    // when the central disconnects, turn off the LED:
    digitalWrite(LED_BUILTIN, LOW);
    Serial.print("Disconnected from central: ");
    Serial.println(central.address());
  }
  delay(1000);
}

float measureTemperature(int address) {
  float Temperature;
  Wire.requestFrom(address, 2);
  byte buff[2];
  Wire.readBytes(buff, 2);
  //Serial.print(buff[0],BIN);
  //Serial.print("  ");
  //Serial.println(buff[1],BIN);
  if (bitRead(buff[0], 7) == 1) { // If the first bit is a 1
    Temperature = (~((buff[0] << 8) + buff[1]) + 1) * -0.0078125; // Compute Negative Temperature
  } else { // If the first bit is a 0
    Temperature = ((buff[0] << 8) + buff[1]) * 0.0078125; // Compute Positive Temperature
  }
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
