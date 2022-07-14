/* basic_receiver.ino
 *  
 *  initializes a makers controller with the most basic functionality and sends 
 *  controller data to a connected ESP32. 
 * 
 *  See Github page for more information about the library https://github.com/Makers-Oakland-University/Makers-Unified-Controller-Arduino
 */

//import library
#include <makers_controller.h>

//create controller object
MakersController controller = MakersController(); 

void setup() {
  //init serial
  Serial.begin(115200);

  //In order to connect to a device with the controller the controller 
  //must have the device's MAC address, when startReceiver() is called for the target device
  //the MAC address will be printed to the serial terminal, that can be coppied here. 
  //the MAC address should be in a similar format to below 
  String targetDeviceMAC = "94:B9:7E:C4:A4:CC";

  //start the controller and give it the address of the target 
  controller.startController(targetDeviceMAC);
}
void loop() {
  //read the buttons and the joysticks then send that to 
  //the target device. 
  controller.readAndSend();
}
