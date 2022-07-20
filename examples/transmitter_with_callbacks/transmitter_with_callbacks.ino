/* transmitter_with_callbacks.ino
 *  
 *  initializes a makers controller  and uses callbacks in order to print button states to the serial terminal
 *  callbacks allow for actions to occur asynchronously when the buttons are pressed on the transmitter
 *  simplifying other code. This is the recommended method for most robotics projects.
 *  On the controller end this is less useful, but the feature is supported none-the-less
 * 
 *  the callbacks present on the controller DO NOT have to match the callbacks on the receiver. 
 *  The controller could have 0 callbacks while the receiver has many. Or vice versa 
 * 
 * 
 *  See Github page for more information about the library https://github.com/Makers-Oakland-University/Makers-Unified-Controller-Arduino
 */

//import library
#include <makers_controller.h>

//create controller object
MakersController controller = MakersController(); 

//joystick callback, this can be any function name as long as the function has the parameters (float, float, float, float). 
//the joystick callback will trigger every time a message is received from the controller. On the controller end
//this will only be tiggerred if readAndSend() is called and the joystick has changed position
void joystick_callback(float left_x, float left_y, float right_x, float right_y) {
  
  //avoid printing large amounts of data in the callbacks, this can result in the 
  //ESP32 crashing, the amount shown here should only be printed with the controller set to transmit
  //only once every 100ms. 
  //if the controller is set to transmit at too high a speed the Serial buffer will overflow resulting in 
  //a crash. 
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
  //init serial
  Serial.begin(115200);

  //In order to connect to a device with the controller the controller 
  //must have the device's MAC address, when startReceiver() is called for the target device
  //the MAC address will be printed to the serial terminal, that can be coppied here. 
  //the MAC address should be in a similar format to below 
  String targetDeviceMAC = "78:E3:6D:10:DB:40";

  //start the controller and give it the address of the target 
  controller.startController(targetDeviceMAC);

  //register a callback for the joystick, see the joystick callback function above. 
  controller.registerJoystickCallback(joystick_callback);

  //we can set the threshold of the joystick callback also, the default value is 0.03
  //set lower for better responsiveness, or higher to prevent the callback being spammed
  //0.03 is a good value to ensure noise in the joystick movement isn't transmitted. 
  controller.setJoystickCallbackThreshold(0.03);

  //register a callback for switch 1
  controller.registerButtonCallback(MAKERS_CONTROLLER_SW1, sw1_callback);

  //register callback for left trigger
  controller.registerButtonCallback(MAKERS_CONTROLLER_SW_L_TRIG, ltrig_callback);

  //register callback for right trigger 
  controller.registerButtonCallback(MAKERS_CONTROLLER_SW_R_TRIG, rtrig_callback);

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
  //read the buttons and the joysticks then send that to the target device. 
  controller.readAndSend();
}
