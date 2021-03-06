#include "makers_controller.h"

#ifndef MAKERS_CONTROLLER_ARDUINO_CPP
#define MAKERS_CONTROLLER_ARDUINO_CPP

MakersController *MakersController::_reference = nullptr;

MakersController::MakersController()
{
    _reference = this;
}

void MakersController::printMakersASCII()
{
    Serial.println("  __  __       _                     ____     ___  _   _ \n |  \\/  | __ _| | _____ _ __ ___    / __ \\   / _ \\| | | |\n | |\\/| |/ _` | |/ / _ \\ '__/ __|  / / _` | | | | | | | |\n | |  | | (_| |   <  __/ |  \\__ \\ | | (_| | | |_| | |_| |\n |_|  |_|\\__,_|_|\\_\\___|_|  |___/  \\ \\__,_|  \\___/ \\___/ \n                                    \\____/               ");
}

boolean MakersController::isConnected()
{
    // get the current number of peer devices, if its greater than 0 we know the device is connected
    // https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/network/esp_now.html#_CPPv416esp_now_peer_num
    esp_now_peer_num_t num;
    esp_now_get_peer_num(&num);
    return (num.total_num > 0);
}

void MakersController::setPeerAddress(String address)
{
    // Serial.println("Setting peer address");
    // address formatted as xx:xx:xx:xx:xx:xx
    broadcastAddress[0] = (uint8_t)strtoul(address.substring(0, 2).c_str(), NULL, 16);
    broadcastAddress[1] = (uint8_t)strtoul(address.substring(3, 5).c_str(), NULL, 16);
    broadcastAddress[2] = (uint8_t)strtoul(address.substring(6, 8).c_str(), NULL, 16);
    broadcastAddress[3] = (uint8_t)strtoul(address.substring(9, 11).c_str(), NULL, 16);
    broadcastAddress[4] = (uint8_t)strtoul(address.substring(12, 14).c_str(), NULL, 16);
    broadcastAddress[5] = (uint8_t)strtoul(address.substring(15, 17).c_str(), NULL, 16);
}

String MakersController::getPeerAddressString()
{
    String peer = "00:00:00:00:00:00";
    for (int a = 0; a < 18; a += 3)
    {
        String segment = String(broadcastAddress[a / 3], HEX);
        peer[a] = segment[0];
        peer[a + 1] = segment[1];
    }

    return peer;
}

void MakersController::startController(String peer_address)
{
    printMakersASCII();
    Serial.println("Starting Controller");

    initIO();
    setPeerAddress(peer_address);

    // Set device as a Wi-Fi Station
    WiFi.mode(WIFI_STA);

    Serial.print("Controller MAC Address: ");
    Serial.println(WiFi.macAddress());

    // Init ESP-NOW
    if (esp_now_init() != ESP_OK)
    {
        Serial.println("Error initializing ESP-NOW");
        return;
    }

    // Once ESPNow is successfully Init, we will register for Send CB to
    // get the status of Transmitted packet
    esp_now_register_send_cb(MakersController::onDataSent);

    // Register peer
    memcpy(peerInfo.peer_addr, broadcastAddress, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;

    // Add peer
    if (esp_now_add_peer(&peerInfo) != ESP_OK)
    {
        Serial.println("Failed to add peer");
        return;
    }

    Serial.println("Attempting to connect to peer @ " + getPeerAddressString());
}

void MakersController::startReceiver()
{
    printMakersASCII();
    Serial.println("Starting Receiver");

    // Set device as a Wi-Fi Station
    WiFi.mode(WIFI_STA);

    Serial.print("Receiver MAC Address: ");
    Serial.println(WiFi.macAddress());

    // Init ESP-NOW
    if (esp_now_init() != ESP_OK)
    {
        Serial.println("Error initializing ESP-NOW");
        return;
    }

    // Once ESPNow is successfully Init, we will register for Send CB to
    // get the status of Transmitted packet
    esp_now_register_recv_cb(MakersController::onDataReceived);
}

void MakersController::onDataReceived(const uint8_t *mac, const uint8_t *incomingData, int len)
{
    int previous_button_state = _reference->controller_data.buttons;

    float plx = _reference->controller_data.left_joy_x;
    float ply = _reference->controller_data.left_joy_y;
    float prx = _reference->controller_data.right_joy_x;
    float pry = _reference->controller_data.right_joy_y;

    memcpy(&_reference->controller_data, incomingData, sizeof(makers_controller_message));
    _reference->checkButtonTransitions(previous_button_state, _reference->controller_data.buttons);
    _reference->checkJoystickTransition(
        _reference->controller_data.left_joy_x,
        _reference->controller_data.left_joy_y,
        _reference->controller_data.right_joy_x,
        _reference->controller_data.right_joy_y);
    _reference->setLastMessageReceived(millis());
}

void MakersController::setLastMessageReceived(unsigned long time)
{
    _last_message_received = millis();
}

unsigned long MakersController::getLastMessageAge()
{
    return millis() - _last_message_received;
}

void MakersController::initIO()
{
    pinMode(PIN_LEFT_JOY_Y, INPUT);
    pinMode(PIN_LEFT_JOY_X, INPUT);
    pinMode(PIN_LEFT_JOY_SW, INPUT);

    pinMode(PIN_RIGHT_JOY_Y, INPUT);
    pinMode(PIN_RIGHT_JOY_X, INPUT);
    pinMode(PIN_RIGHT_JOY_SW, INPUT);

    // get joystick offsets.
    joystick_offsets[0] = analogRead(PIN_LEFT_JOY_X);
    joystick_offsets[1] = analogRead(PIN_LEFT_JOY_Y);
    joystick_offsets[2] = analogRead(PIN_RIGHT_JOY_X);
    joystick_offsets[3] = analogRead(PIN_RIGHT_JOY_Y);

    pinMode(PIN_SW1, INPUT);
    pinMode(PIN_SW2, INPUT);
    pinMode(PIN_SW3, INPUT);
    pinMode(PIN_SW4, INPUT);
    pinMode(PIN_SW5, INPUT);
    pinMode(PIN_SW6, INPUT);
    pinMode(PIN_SW7, INPUT);
    pinMode(PIN_SW8, INPUT);

    pinMode(PIN_LEFT_TRIG, INPUT);
    pinMode(PIN_RIGHT_TRIG, INPUT);
}

void MakersController::onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
    // Serial.print("\r\nLast Packet Send Status:\t");
    // Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
    int result = (status == ESP_NOW_SEND_SUCCESS) ? 1 : 0;
    _reference->trackDataSentStatus(result);
    // Serial.printf("percent successful %f%\n", _reference->getSuccessfulTransmissionPercentage());
}

void MakersController::trackDataSentStatus(int status)
{
    data_sent_tracker &= ~(1 << data_sent_tracker_index);
    data_sent_tracker |= status << data_sent_tracker_index;
    data_sent_tracker_index++;
    if (data_sent_tracker_index >= SENT_POLLING_WINDOW_BITS)
        data_sent_tracker_index = 0;
}

float MakersController::getSuccessfulTransmissionPercentage()
{
    float total_success = 0;
    for (int a = 0; a < SENT_POLLING_WINDOW_BITS; a++)
        total_success += ((data_sent_tracker >> a) & 0x01);

    // calculate percentage
    return (total_success / ((float)SENT_POLLING_WINDOW_BITS)) * 100.0;
}

void MakersController::readAndSend()
{

    float plx = controller_data.left_joy_x;
    float ply = controller_data.left_joy_y;
    float prx = controller_data.right_joy_x;
    float pry = controller_data.right_joy_y;

    // read and map left x joystick
    if (analogRead(PIN_LEFT_JOY_X) < joystick_offsets[0])
        controller_data.left_joy_x = map(analogRead(PIN_LEFT_JOY_X), joystick_offsets[0], 0, 0, -2048) / 2048.0;
    else
        controller_data.left_joy_x = map(analogRead(PIN_LEFT_JOY_X), joystick_offsets[0], 4096, 0, 2048) / 2048.0;

    // read and map left y joystick
    if (analogRead(PIN_LEFT_JOY_Y) < joystick_offsets[1])
        controller_data.left_joy_y = map(analogRead(PIN_LEFT_JOY_Y), joystick_offsets[1], 0, 0, -2048) / 2048.0;
    else
        controller_data.left_joy_y = map(analogRead(PIN_LEFT_JOY_Y), joystick_offsets[1], 4096, 0, 2048) / 2048.0;

    // read and map
    if (analogRead(PIN_RIGHT_JOY_X) < joystick_offsets[2])
        controller_data.right_joy_x = map(analogRead(PIN_RIGHT_JOY_X), joystick_offsets[2], 0, 0, -2048) / 2048.0;
    else
        controller_data.right_joy_x = map(analogRead(PIN_RIGHT_JOY_X), joystick_offsets[2], 4096, 0, 2048) / 2048.0;

    // read and map left y joystick
    if (analogRead(PIN_RIGHT_JOY_Y) < joystick_offsets[3])
        controller_data.right_joy_y = map(analogRead(PIN_RIGHT_JOY_Y), joystick_offsets[3], 0, 0, -2048) / 2048.0;
    else
        controller_data.right_joy_y = map(analogRead(PIN_RIGHT_JOY_Y), joystick_offsets[3], 4096, 0, 2048) / 2048.0;

    int previous_button_state = controller_data.buttons;

    controller_data.buttons = 0;
    controller_data.buttons |= digitalRead(PIN_SW1) << MAKERS_CONTROLLER_SW1;
    controller_data.buttons |= digitalRead(PIN_SW2) << MAKERS_CONTROLLER_SW2;
    controller_data.buttons |= digitalRead(PIN_SW3) << MAKERS_CONTROLLER_SW3;
    controller_data.buttons |= digitalRead(PIN_SW4) << MAKERS_CONTROLLER_SW4;
    controller_data.buttons |= digitalRead(PIN_SW5) << MAKERS_CONTROLLER_SW5;
    controller_data.buttons |= digitalRead(PIN_SW6) << MAKERS_CONTROLLER_SW6;
    controller_data.buttons |= digitalRead(PIN_SW7) << MAKERS_CONTROLLER_SW7;
    controller_data.buttons |= digitalRead(PIN_SW8) << MAKERS_CONTROLLER_SW8;
    controller_data.buttons |= digitalRead(PIN_RIGHT_JOY_SW) << MAKERS_CONTROLLER_SW_R_JOY;
    controller_data.buttons |= digitalRead(PIN_LEFT_JOY_SW) << MAKERS_CONTROLLER_SW_L_JOY;
    controller_data.buttons |= digitalRead(PIN_RIGHT_TRIG) << MAKERS_CONTROLLER_SW_R_TRIG;
    controller_data.buttons |= digitalRead(PIN_LEFT_TRIG) << MAKERS_CONTROLLER_SW_L_TRIG;

    // Send message via ESP-NOW
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&controller_data, sizeof(controller_data));
    checkButtonTransitions(previous_button_state, controller_data.buttons);
    _reference->checkJoystickTransition(
        _reference->controller_data.left_joy_x,
        _reference->controller_data.left_joy_y,
        _reference->controller_data.right_joy_x,
        _reference->controller_data.right_joy_y);
}

void MakersController::checkJoystickTransition(float nlx, float nly, float nrx, float nry)
{
    boolean joysticksChanged = false;
    boolean lx_changed = abs(_last_joystick_values_triggered[0] - nlx) >= _joystick_update_threshold;
    boolean ly_changed = abs(_last_joystick_values_triggered[1] - nly) >= _joystick_update_threshold;
    boolean rx_changed = abs(_last_joystick_values_triggered[2] - nrx) >= _joystick_update_threshold;
    boolean ry_changed = abs(_last_joystick_values_triggered[3] - nry) >= _joystick_update_threshold;

    if (lx_changed || ly_changed || rx_changed || ry_changed)
        triggerJoystickCallback(nlx, nly, nrx, nry);
}

void MakersController::setJoystickCallbackThreshold(float threshold)
{
    _joystick_update_threshold = threshold;
}

void MakersController::checkButtonTransitions(uint16_t previous_state, uint16_t current_state)
{
    // bitwise XOR to catch any differences between previous and current state
    uint16_t transitions = previous_state ^ current_state;

    for (int a = 0; a < MAKERS_CONTROLLER_NUM_BUTTONS; a++)
    {
        if ((transitions >> a) & 0x01)
            serviceCallback(a, (current_state >> a) & 0x01);
    }
}

void MakersController::serviceCallback(int index, int button_state)
{
    // Serial.printf("Button %d transitioned to %d \n", index, button_state);

    if (index >= MAKERS_CONTROLLER_NUM_BUTTONS)
    {
        Serial.printf("Error ServiceCallback: index %d is out of range\n", index);
        return;
    }

    // this is going to get called often for buttons with no callback
    // so no print this time
    if (_callbacks[index] == nullptr)
        return;

    _callbacks[index](button_state);
}

void MakersController::triggerJoystickCallback(float lx, float ly, float rx, float ry)
{
    // if no joystick callback then return from the function
    if (_joystickCallback == nullptr)
        return;

    _joystickCallback(lx, ly, rx, ry);

    _last_joystick_values_triggered[0] = lx;
    _last_joystick_values_triggered[1] = ly;
    _last_joystick_values_triggered[2] = rx;
    _last_joystick_values_triggered[3] = ry;
}

void MakersController::registerJoystickCallback(void (*cb)(float, float, float, float))
{
    if (cb != nullptr)
        _joystickCallback = cb;
    else
        Serial.println("failed to register joystick callback, callback is NULL");
}

inline int MakersController::readSwitch(int position)
{
    return (controller_data.buttons >> position) & 0x01;
}

int MakersController::readSW1()
{
    return readSwitch(MAKERS_CONTROLLER_SW1);
}

int MakersController::readSW2()
{
    return readSwitch(MAKERS_CONTROLLER_SW2);
}

int MakersController::readSW3()
{
    return readSwitch(MAKERS_CONTROLLER_SW3);
}

int MakersController::readSW4()
{
    return readSwitch(MAKERS_CONTROLLER_SW4);
}

int MakersController::readSW5()
{
    return readSwitch(MAKERS_CONTROLLER_SW5);
}

int MakersController::readSW6()
{
    return readSwitch(MAKERS_CONTROLLER_SW6);
}

int MakersController::readSW7()
{
    return readSwitch(MAKERS_CONTROLLER_SW7);
}

int MakersController::readSW8()
{
    return readSwitch(MAKERS_CONTROLLER_SW8);
}

int MakersController::readRightJoystickSwitch()
{
    return readSwitch(MAKERS_CONTROLLER_SW_R_JOY);
}

int MakersController::readLeftJoystickSwitch()
{
    return readSwitch(MAKERS_CONTROLLER_SW_L_JOY);
}

int MakersController::readRightTrigger()
{
    return readSwitch(MAKERS_CONTROLLER_SW_R_TRIG);
}

int MakersController::readLeftTrigger()
{
    return readSwitch(MAKERS_CONTROLLER_SW_L_TRIG);
}

float MakersController::readLeftJoystickX()
{
    return controller_data.left_joy_x;
}

float MakersController::readLeftJoystickY()
{
    return controller_data.left_joy_y;
}

float MakersController::readRightJoystickX()
{
    return controller_data.right_joy_x;
}

float MakersController::readRightJoystickY()
{
    return controller_data.right_joy_y;
}

void MakersController::registerButtonCallback(int button, void (*cb)(int))
{
    if (button < MAKERS_CONTROLLER_NUM_BUTTONS)
    {
        if (cb != nullptr)
        {
            _callbacks[button] = cb;
            Serial.printf("Registered callback for button %d\n", button);
        }
        else
            Serial.printf("Failed to register callback for button %d, callbaclk is NULL\n", button);
    }
    else
        Serial.printf("Button of index %d does not exist\n", button);
}

#endif