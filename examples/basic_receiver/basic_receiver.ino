/* basic_receiver.ino
 *  
 *  initializes an ESP32 as a receiver and prints the received button inputs
 *  to the serial terminal so that we can view them. Demonstrates all the controller
 *  inputs that can be read remotely. 
 * 
 *  See Github page for more information about the library https://github.com/Makers-Oakland-University/Makers-Unified-Controller-Arduino
 */


#include <makers_controller.h>

//create receiver object that is going to be our way to get data
MakersController receiver = MakersController();

void setup() {
  //enable serial 
  Serial.begin(115200);

  //start the receiver, this opens the device up to begin receiving data from a controller
  //this is the only bit of startup code required, any controller that desires to connect to the 
  //MAC address of this ESP32 will automatically do so. 
  receiver.startReceiver();
}
void loop() {

  //Read joysticks and print to serial terminal
  //right joystick
  Serial.print("Right Joystick ");
  Serial.print(receiver.readRightJoystickX());
  Serial.print(",");
  Serial.println(receiver.readRightJoystickY());

  //left joystick
  Serial.print("Left Joystick ");
  Serial.print(receiver.readLeftJoystickX());
  Serial.print(",");
  Serial.println(receiver.readLeftJoystickY());

  //now lets read all the buttons, see github page for placement of switches
  //on the receiver
  Serial.print("Switches: ");

  //left hand up
  Serial.print(receiver.readSW1());
  Serial.print(" ");

  //left hand right
  Serial.print(receiver.readSW2());
  Serial.print(" ");

  //left hand down
  Serial.print(receiver.readSW3());
  Serial.print(" ");  

  //left hand left 
  Serial.print(receiver.readSW4());
  Serial.print(" ");

  //right hand up 
  Serial.print(receiver.readSW5());
  Serial.print(" ");

  //right hand right
  Serial.print(receiver.readSW6());
  Serial.print(" ");

  //right hand down 
  Serial.print(receiver.readSW7());
  Serial.print(" ");

  //right hand left
  Serial.print(receiver.readSW8());
  Serial.print(" ");

  //read trigger buttons
  Serial.print(receiver.readRightTrigger());
  Serial.print(" ");

  Serial.print(receiver.readLeftTrigger());
  Serial.print(" ");

  //joystick switches
  Serial.print(receiver.readRightJoystickSwitch());
  Serial.print(" ");

  Serial.print(receiver.readLeftJoystickSwitch());
  Serial.println(); //done reading switches

  //delay for this example, but you can call these functions as fast as you would like
  delay(250);
}
