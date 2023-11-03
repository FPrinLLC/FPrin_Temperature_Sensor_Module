# FPrin Temperature Sensor Module Development
Temperature measurement devices are ubiquitous in engineering and other scientific fields because temperature is a parameter that affects nearly all systems and is often essential to measure and control. Through recent work at FPrin involving temperature measurement and analysis we surveyed a series of temperature sensors and found that many common sensors have a surprisingly large rated accuracy specification. For example, perhaps the most used type of temperature measurement device, the thermocouple, typically has a range of ±1-2 °C. For some applications this is an acceptable level of accuracy, but often is insufficient. FPrin aims to develop a cost-effective, easy to build and program, highly accurate, and fully wireless temperature logging solution allowing for easily accessible, high confidence temperature measurement.
## Wireless Temperature Sensing Module
![Step 2-2-2-5a](https://github.com/FPrinLLC/FPrin_Temperature_Sensor_Module/assets/125088225/2417efe3-f823-4f97-89d9-5d4cc39765a2)

### Features:
- AS6221 Digital Temperature Sensor with rated accuracy of ±0.09 °C
- 3D Printed PLA Enclosure
- Assembly in under 1 hour
- Bluetooth Low Energy (BLE) Supported Data Logging
- High Capacity 2000 mAh Battery
- Companion MATLAB Application for visualization and Logging
- Programmable with Arduino IDE

### Datalogging Interface
To visualize and record data, we have written a MATLAB application that can easily connect with the temperature sensor module, and plot data at a desired frequency.

![image](https://github.com/FPrinLLC/FPrin_Temperature_Sensor_Module/assets/125088225/4d5cf51e-a791-4607-8e4b-985fe710dbf9)

We've also saved a version of the app as an executable file, this way you won't need a license of MATLAB to use the application.

### Where to get started:
To get started with the temperature sensor module, check out the documentation:
- User Manual: Guide for building, programming, and setting up the module
- Bill of Materials: A full list of everything you'll need to build the module
- MATLAB Application: A simple desktop app that allows you to connect to a temperature sensor module and log data
