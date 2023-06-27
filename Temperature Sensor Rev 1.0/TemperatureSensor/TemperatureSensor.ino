#include <Wire.h>

#define SENSOR_ADDRESS 72
#define LED_R 22
#define LED_G 23
#define LED_B 24

#define DEBUGG 0

float Temperature, TemperatureFiltered;
unsigned long currentTime, startTime, sampleTime;

void setup() {
  // put your setup code here, to run once:
  byte error;
  Wire.begin();
  Wire.setClock(400000UL); // initialize at 400 kHz
  Serial.begin(115200);
  
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);
  digitalWrite(LED_R, HIGH);
  digitalWrite(LED_B, HIGH);
  digitalWrite(LED_G, HIGH);
  while(!Serial) {}

  Serial.println("Temperature Sensor Module v1.03");
  digitalWrite(LED_G, LOW);
  
  Wire.beginTransmission(SENSOR_ADDRESS);
  error = Wire.endTransmission();

  Serial.print("Attempting to Initialize with I2C Device at address 0x");
  Serial.println(SENSOR_ADDRESS,HEX);

  if (error != 0) {
    Serial.print("Error - Could not interface with I2C device at address 0x");
    Serial.println(SENSOR_ADDRESS,HEX);
    while(1);
  }
  Serial.println("Temperature Sensor Initialization successful!");

  if(DEBUGG) {
    readConfigRegister(SENSOR_ADDRESS);
  }
  
  Serial.println("Enter Sampling interval in seconds:");
  while(!Serial.available()) {}
  sampleTime = Serial.parseInt();
  sampleTime = constrain(sampleTime,1,86400);
  Serial.print("Sample Time set for ");
  Serial.print(sampleTime);
  Serial.println(" seconds");
  digitalWrite(LED_G, HIGH);
  
  startTime = millis();
  Temperature = measureTemperature(SENSOR_ADDRESS);
  TemperatureFiltered = Temperature;
  sampleTime = sampleTime * 1000; // Convert to ms
}

void loop() {
  // put your main code here, to run repeatedly:
  currentTime = millis();
  Temperature = measureTemperature(SENSOR_ADDRESS);
  updateFilteredTemperature(0.05);
  if((currentTime - startTime) >= sampleTime) {
    printTemperature();
    startTime = currentTime;
  }
  delay(250);
}

float measureTemperature(byte address) {
  float Temperature;
  Wire.requestFrom(address, 2);
  byte buff[2];
  Wire.readBytes(buff, 2);
  //Serial.print(buff[0],BIN);
  //Serial.print("  ");
  //Serial.println(buff[1],BIN);
  if (bitRead(buff[0],7) == 1) { // If the first bit is a 1
    Temperature = (~((buff[0] << 8) + buff[1]) + 1) * -0.0078125; // Compute Negative Temperature
  } else { // If the first bit is a 0
    Temperature = ((buff[0] << 8) + buff[1]) * 0.0078125; // Compute Positive Temperature
  }
  return Temperature;
}

void printTemperature() {
  digitalWrite(LED_B, LOW);
  Serial.println(Temperature,2);
  digitalWrite(LED_B, HIGH);
}

void updateFilteredTemperature(float alpha) {
  alpha = constrain(alpha, 0.0, 1.0);
  float previousTemperatureFiltered = TemperatureFiltered;
  TemperatureFiltered = previousTemperatureFiltered + alpha*(Temperature - previousTemperatureFiltered);
}

void readConfigRegister(byte address) {
  Serial.println("Press any key to fetch config byte");
  while(!Serial.available()) {}
  Serial.read();
  Serial.println("Configuration bytes: ");
  
  Wire.beginTransmission(address);
  Wire.write(B00000001);
  Wire.endTransmission();

  Wire.requestFrom(address,2);
  byte buff[2];
  Wire.readBytes(buff,2);

  Wire.beginTransmission(address);
  Wire.write(B00000000);
  Wire.endTransmission();
  Serial.println(buff[0],BIN);
  Serial.println(buff[1],BIN);
}
