/*  receiver_with_callbacks.ino

    initializes an ESP32 as a receiver and utilizes callbacks to print button states to the terminal
    callbacks allow for actions to occur asynchronously when the buttons are pressed on the transmitter
    simplifying other code. This is the recommended method for most robotics projects.

    the callbacks present on the controller DO NOT have to match the callbacks on the receiver. 
    The controller could have 0 callbacks while the receiver has many. Or vice versa 

    See Github page for more information about the library https://github.com/Makers-Oakland-University/Makers-Unified-Controller-Arduino
*/


#include <makers_controller.h>

//create receiver object that is going to be our way to get data
MakersController receiver = MakersController();


//joystick callback, this can be any function name as long as the function has the parameters (float, float, float, float).
//the joystick callback will trigger when the joystick has moved by the threshold amount.
void joystick_callback(float left_x, float left_y, float right_x, float right_y) {

  //print the new joystick out to the
  Serial.print("Joystick Values: Left - (");
  Serial.print(left_x);
  Serial.print(",");
  Serial.print(left_y);
  Serial.print(") Right - (");
  Serial.print(right_x);
  Serial.print(",");
  Serial.print(right_y);
  Serial.println(")");
  Serial.flush();
}

//swtich 1 callback, this function will be called whenever the state of switch 1 changes
//the 'state' indicates whether the button is HIGH or LOW. 
void sw1_callback(int state) {
  if (state == HIGH)
    Serial.println("Switch 1 pressed");
  else
    Serial.println("Switch 1 released");
}

//right trigger callback, this function will be called whenever the state of the right trigger changes
//the 'state' indicates whether the button is HIGH or LOW. 
void rtrig_callback(int state) {
  if (state == HIGH)
    Serial.println("right trigger pressed");
  else
    Serial.println("right trigger released");
}


//left trigger callback, this function will be called whenever the state of the left trigger changes
//the 'state' indicates whether the button is HIGH or LOW. 
void ltrig_callback(int state) {
  if (state == HIGH)
    Serial.println("left trigger pressed");
  else
    Serial.println("left trigger released");
}


void setup() {
  //enable serial
  Serial.begin(115200);

  //start the receiver, this opens the device up to begin receiving data from a controller
  //this is the only bit of startup code required, any controller that desires to connect to the
  //MAC address of this ESP32 will automatically do so.
  receiver.startReceiver();

  //register a callback for the joystick, see the joystick callback function above.
  receiver.registerJoystickCallback(joystick_callback);

  //we can set the threshold of the joystick callback also, the default value is 0.03
  //set lower for better responsiveness, or higher to prevent the callback being spammed
  //0.03 is a good value to ensure noise in the joystick movement isn't transmitted.
  receiver.setJoystickCallbackThreshold(0.03);

  //register a callback for switch 1
  receiver.registerButtonCallback(MAKERS_CONTROLLER_SW1, sw1_callback);

  //register callback for left trigger
  receiver.registerButtonCallback(MAKERS_CONTROLLER_SW_L_TRIG, ltrig_callback);

  //register callback for right trigger 
  receiver.registerButtonCallback(MAKERS_CONTROLLER_SW_R_TRIG, rtrig_callback);

  //all buttons support callbacks, simply create a function with the parameter (int) 
  //and pass it into registerButtonCallback, the identifiers for each button are as follows: 
  //  Switch 1          -   MAKERS_CONTROLLER_SW1
  //  Switch 2          -   MAKERS_CONTROLLER_SW2
  //  Switch 3          -   MAKERS_CONTROLLER_SW3
  //  Switch 4          -   MAKERS_CONTROLLER_SW4
  //  Switch 5          -   MAKERS_CONTROLLER_SW5
  //  Switch 6          -   MAKERS_CONTROLLER_SW6
  //  Switch 7          -   MAKERS_CONTROLLER_SW7
  //  Switch 8          -   MAKERS_CONTROLLER_SW8
  //  Left Joy Button   -   MAKERS_CONTROLLER_SW_L_JOY
  //  Right Joy Button  -   MAKERS_CONTROLLER_SW_R_JOY
  //  Left Trigger      -   MAKERS_CONTROLLER_SW_L_TRIG
  //  Right Trigger     -   MAKERS_CONTROLLER_SW_R_TRIG
  // a diagram of which buttons are where on the controller can be found 
  // on github: https://raw.githubusercontent.com/Makers-Oakland-University/readme-images/main/makers_controller%20switch%20diagram.png
}

void loop() {
  //you can do whatever you want in the loop now :)
}
