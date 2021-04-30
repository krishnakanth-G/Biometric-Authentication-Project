# Biometric-Authentication-Project
Biometric Authentication Project using Arduino IDE

prerequisite
1) Arduino IDE
2) Arduino Uno
3) Node MCU ESP8266
4) Finger print scanner TTL GT-5210F12
5) TFT LCD Touch screen (2.4)

The details of connection are mentioned in the Biometric Authentication Project Report

Basically this project contains two parts:
1) One is biometric authentication through a particular screen which is fixed to arduino board and connected to fingerprint scanner & ESP8266 board by serial communication.The files required for this module are in serial com folder which further contains two folders. First is Arduinoscreen folder which contains two INO files, among those upload ArduinoScreen.ino to Arduino Uno using Arduino IDE.Second one is ESP folder which contains three INO files, among those upload ESP.ino to ESP8266 board using Arduino IDE.
2) Later one is biometric authentication through any browser with fingerprint scanner and ESP8266 which is a Wi-Fi Microcontroller with TCP/IP protocol. The files required for this module are in Stores folder which further contains two folders namely stores admin and stores client. Inside stores admin folder we have INO files that will register clients and Sotres client folder contains the INO files that will authentication client.

