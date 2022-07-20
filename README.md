# Makers-Unified-Controller-Arduino

Companion library for the ESP32 based [Makers Unified Controller](https://github.com/Makers-Oakland-University/Makers-Unified-Controller). 
The library simplifes the interfacing of the makers controller to a remote device, see the examples under **File > Examples > Makers-Unified-Controller-Arduino**

## Table of Contents
- [Makers-Unified-Controller-Arduino](#makers-unified-controller-arduino)
  - [Table of Contents](#table-of-contents)
- [Installation](#installation)
- [Functions](#functions)
  - [Obtaining Receiver MAC Address](#obtaining-receiver-mac-address)
- [Input Identifiers](#input-identifiers)
- [Utilizing the OLED Display](#utilizing-the-oled-display)

# Installation 
Click the **Code > Download ZIP** and install to the Arduino IDE by navigating to **Sketch > Include Library > Add .ZIP Library...** and select the downloaded zip file. 
This library requires the ESP32 Arduino Core to be installed, a guide for installation can be found [Here](https://randomnerdtutorials.com/installing-the-esp32-board-in-arduino-ide-windows-instructions/).

## Utilizing the OLED Display

This library does not support the LCD directly, for using the LCD you will need to install the [SSD1306 Adafruit Library](https://github.com/adafruit/Adafruit_SSD1306) and [Adafruit GFX Library](https://github.com/adafruit/Adafruit-GFX-Library)
The Makers Unified Controller OLED is attached to **IO21 for SDA** and **IO22 for SCL**. 
A simple example of using the OLD is supplied in the **basic_transmitter_oled.ino** example. 

# Functions

This section shows the available functions of the library with a description.
See the provided library examples for more information about their use. 

| Function | Description |
| ----- | ---- |
|MakersController()|Class constructor |
|void startController(<br />&nbsp;&nbsp;&nbsp;&nbsp;String peer_address)|Starts library in controller (transmitter) mode, used for controller to communicate to a robot. ```peer_address``` is the MAC address of the target receiver. |
|void readAndSend()|reads all buttons and joysticks then transmits the data to the receiver. **Only for use in controller mode**|
|void startReceiver()|Starts library in receiver mode, allowing for messages to be received|
|String getPeerAddressString()|Prints the target peer address when in receiver mode|
|void registerButtonCallback(<br />&nbsp;&nbsp;&nbsp;&nbsp;int button, <br />&nbsp;&nbsp;&nbsp;     void (*cb)(int))|Registeres a callback function of form ```void func(int)``` that will be called when a transition is read on either the controller or receiver. ```int button``` is the identifier for the desired input on the controller, see the **Input Identifiers** section of this readme for more information.|
|float getSuccessfulTransmissionPercentage()|Returns a value from 0-100 representing the ratio of successful transmissions to failed transmissions|
|boolean isConnected()|Returns True if the **controller** is connected to the receiver.|
|int readSW1()|Returns 0 or 1 (HIGH or LOW) depending on the state of SW1|
|int readSW2()|Returns 0 or 1 (HIGH or LOW) depending on the state of SW2|
|int readSW3()|Returns 0 or 1 (HIGH or LOW) depending on the state of SW3|
|int readSW4()|Returns 0 or 1 (HIGH or LOW) depending on the state of SW4|
|int readSW5()|Returns 0 or 1 (HIGH or LOW) depending on the state of SW5|
|int readSW6()|Returns 0 or 1 (HIGH or LOW) depending on the state of SW6|
|int readSW7()|Returns 0 or 1 (HIGH or LOW) depending on the state of SW7|
|int readSW8()|Returns 0 or 1 (HIGH or LOW) depending on the state of SW8|
|int readRightJoystickSwitch()|Returns 0 or 1 (HIGH or LOW) depending on the state of the tactile switch of the left joystick|
|int readLeftJoystickSwitch()|Returns 0 or 1 (HIGH or LOW) depending on the state of the tactile switch of the right joystick|
|int readRightTrigger()|Returns 0 or 1 (HIGH or LOW) depending on the state of the right trigger|
|int readLeftTrigger()|Returns 0 or 1 (HIGH or LOW) depending on the state of the right trigger|
|float readLeftJoystickX()|Returns a value between -1.0 and 1.0 depending on the position of the left joystick x-axis (left/right)|
|float readLeftJoystickY()|Returns a value between -1.0 and 1.0 depending on the position of the left joystick y-axis (up/down)|
|float readRightJoystickX()|Returns a value between -1.0 and 1.0 depending on the position of the left joystick y-axis (left/right)|
|float readRightJoystickY()|Returns a value between -1.0 and 1.0 depending on the position of the right joystick y-axis (up/down)|

## Obtaining Receiver MAC Address

The controller must have a target device it's attempting to connect to. 
Each ESP32 has a unique MAC address which is specific to that board. 
To find this MAC address flash any of the receiver examples and open the serial terminal. 
The MAC address will be printed when **startReceiver()** is called. 
This can then be copied into the controller code to make the controller target that ESP32. 

If the MAC address does not appear when the serial terminal is open hit the reset button (EN) on the ESP32 board. 

![Serial output of the receiver, showing how to obtain the receiver MAC address](https://github.com/Makers-Oakland-University/readme-images/blob/main/makers_controller_getting_mac_address.png?raw=true)


# Input Identifiers 

The Makers Unified Controller has 12 total tactile buttons onboard that can be used for any purpose. 
The **BOOT** and **Reset** pins cannot be read directly by the ESP32. 
The switches are labeled as shown in the diagram below, the joysticks also contain a tactile switch which can be activated by pressing on the button, these are not labeled in the below diagram. 

![diagram of switches for the makers unified controller](https://github.com/Makers-Oakland-University/readme-images/blob/main/makers_controller%20switch%20diagram.png?raw=true)

For the **registerButtonCallback()**  the first parameter is the button label, for each of the buttons the labels are as follows: 
| Switch | Label |
| ----- | ---- |
|Switch 1|MAKERS_CONTROLLER_SW1|
|Switch 2|MAKERS_CONTROLLER_SW2|
|Switch 3|MAKERS_CONTROLLER_SW3|
|Switch 4|MAKERS_CONTROLLER_SW4|
|Switch 5|MAKERS_CONTROLLER_SW5|
|Switch 6|MAKERS_CONTROLLER_SW6|
|Switch 7|MAKERS_CONTROLLER_SW7|
|Switch 8|MAKERS_CONTROLLER_SW8|
|Left Joy Button|MAKERS_CONTROLLER_SW_L_JOY|
|Right Joy Button|MAKERS_CONTROLLER_SW_R_JOY|
|Left Trigger|MAKERS_CONTROLLER_SW_L_TRIG|
|Right Trigger|MAKERS_CONTROLLER_SW_R_TRIG|


