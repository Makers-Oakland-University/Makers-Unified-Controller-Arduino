/*  basic_transmitter_oled.ino

    initializes a makers controller with the most basic functionality and sends
    controller data to a connected ESP32. This sketch shows how to use the on-board display
    to display text.

    This sketch requires the adafruit SSD1306 and GFX libraries which can be downloaded at the following links:
    https://github.com/adafruit/Adafruit_SSD1306
    https://github.com/adafruit/Adafruit-GFX-Library

    See the SSD1306 library for more advanced examples of interfacing with the OLED display.

    See Github page for more information about the library https://github.com/Makers-Oakland-University/Makers-Unified-Controller-Arduino
*/

//import library
#include <makers_controller.h>

//imports required for the SSD1306 Display
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


//screen parameters
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

//our display object
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, & Wire, -1);

//create controller object
MakersController controller = MakersController();

void setup() {
  //init serial
  Serial.begin(115200);

  //wait for the display to start up before we do anything else.
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.clearDisplay();


  //In order to connect to a device with the controller the controller
  //must have the device's MAC address, when startReceiver() is called for the target device
  //the MAC address will be printed to the serial terminal, that can be coppied here.
  //the MAC address should be in a similar format to below
  String targetDeviceMAC = "78:E3:6D:10:DB:40";

  //start the controller and give it the address of the target
  controller.startController(targetDeviceMAC);
}
void loop() {
  //read the buttons and the joysticks then send that to
  //the target device.
  controller.readAndSend();

  //clear display
  display.clearDisplay();

  display.setTextColor(WHITE);

  //set cursor at x=0 y=0 (upper left)
  display.setCursor(0, 0);

  //makers :)
  display.println("Makers Controller");

  //draw a horizontal line under the "makers controller" text 
  display.drawLine(0,10, SCREEN_WIDTH, 10, SSD1306_WHITE);

  //set cursor at x=0 y=15
  display.setCursor(0, 15);

  //lets go through all the buttons, if the button is pressed we will
  //print something to the display to show it.
  display.println("Buttons Pressed");

  //check switch 1
  if (controller.readSW1())
    display.print("S1 ");

  //check switch 2
  if (controller.readSW2())
    display.print("S2 ");

  //check switch 3
  if (controller.readSW3())
    display.print("S3 ");

  //check switch 4
  if (controller.readSW4())
    display.print("S4 ");

  //check switch 5
  if (controller.readSW5())
    display.print("S5 ");

  //check switch 6
  if (controller.readSW6())
    display.print("S6 ");

  //check switch 7
  if (controller.readSW7())
    display.print("S7 ");

  //check switch 8
  if (controller.readSW8())
    display.print("S8 ");

  //check right trigger
  if (controller.readRightTrigger())
    display.print("RT ");

  //check left trigger
  if (controller.readLeftTrigger())
    display.print("LT ");

  //check switch under right joystick
  if (controller.readRightJoystickSwitch())
    display.print("RJ ");

  //check switch under left joystick
  if (controller.readLeftJoystickSwitch())
    display.print("LJ ");

  
  //now lets print the right and left joysticks
  //set cursor at x=0 y=40
  display.setCursor(0, 40);

  //first the left joystick
  display.print("LJoy (");
  display.print(controller.readLeftJoystickX());
  display.print(",");
  display.print(controller.readLeftJoystickY());
  display.println(")"); 

  //now the right joystick
  display.print("RJoy (");
  display.print(controller.readRightJoystickX());
  display.print(",");
  display.print(controller.readRightJoystickY());
  display.println(")"); 

  
  //push to display.
  display.display();


}
