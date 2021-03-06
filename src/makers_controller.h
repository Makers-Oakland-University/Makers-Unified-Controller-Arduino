#ifndef MAKERS_CONTROLLER_ARDUINO_H
#define MAKERS_CONTROLLER_ARDUINO_H

#ifndef ARDUINO_ARCH_ESP32
#error “This library only supports boards based on the ESP32”
#endif

// Imports
#include <Arduino.h>
#include "Arduino.h"
#include <WiFi.h>
#include <esp_now.h>
#include <stdlib.h>

// Structs
typedef struct makers_controller_message
{
    float left_joy_y;
    float left_joy_x;
    float right_joy_y;
    float right_joy_x;
    uint16_t buttons;
} makers_controller_message;

// bit locations for 'buttons' since these are all booleans
// we can save some space in the transmission by just putting them into
// a single integer.
#define MAKERS_CONTROLLER_NUM_BUTTONS 12
#define MAKERS_CONTROLLER_SW1 0
#define MAKERS_CONTROLLER_SW2 1
#define MAKERS_CONTROLLER_SW3 2
#define MAKERS_CONTROLLER_SW4 3
#define MAKERS_CONTROLLER_SW5 4
#define MAKERS_CONTROLLER_SW6 5
#define MAKERS_CONTROLLER_SW7 6
#define MAKERS_CONTROLLER_SW8 7
#define MAKERS_CONTROLLER_SW_R_JOY 8
#define MAKERS_CONTROLLER_SW_L_JOY 9
#define MAKERS_CONTROLLER_SW_R_TRIG 10
#define MAKERS_CONTROLLER_SW_L_TRIG 11

// Pin declarations for the controller
#define PIN_LEFT_JOY_Y 34
#define PIN_LEFT_JOY_X 35
#define PIN_LEFT_JOY_SW 4

#define PIN_RIGHT_JOY_X 32
#define PIN_RIGHT_JOY_Y 33
#define PIN_RIGHT_JOY_SW 14

#define PIN_SW1 17
#define PIN_SW2 16
#define PIN_SW3 19
#define PIN_SW4 18
#define PIN_SW5 25
#define PIN_SW6 13
#define PIN_SW7 36
#define PIN_SW8 26
#define PIN_LEFT_TRIG 23
#define PIN_RIGHT_TRIG 27

#define SENT_POLLING_WINDOW_BITS 25

// array for simplification of button reading code.
const int buttons[] = {
    PIN_SW1,
    PIN_SW2,
    PIN_SW3,
    PIN_SW4,
    PIN_SW5,
    PIN_SW6,
    PIN_SW7,
    PIN_SW8,
    PIN_LEFT_TRIG,
    PIN_RIGHT_TRIG,
    PIN_LEFT_JOY_SW,
    PIN_RIGHT_JOY_SW};

class MakersController
{
    makers_controller_message controller_data;
    esp_now_peer_info_t peerInfo;
    uint8_t broadcastAddress[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    void (*_callbacks[MAKERS_CONTROLLER_NUM_BUTTONS])(int);
    
    //in-order left-x, left-y, right-x, right-y
    float _last_joystick_values_triggered[4] = {0,0,0,0}; 
    int16_t joystick_offsets[4] = {0,0,0,0};
    unsigned long _last_message_received = 0; 

    void (*_joystickCallback)(float, float, float, float);
    float _joystick_update_threshold = 0.03;
    static MakersController *_reference;
    unsigned long data_sent_tracker = 0;
    int data_sent_tracker_index = 0;

    // Private constructor so that no objects can be created.
    static void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);
    static void onDataReceived(const uint8_t *mac, const uint8_t *incomingData, int len);
    void setPeerAddress(String address);
    void initIO();
    inline int readSwitch(int position);
    inline void triggerJoystickCallback(float lx, float ly, float rx, float ry);
    void checkJoystickTransition(float nlx, float nly, float nrx, float nry);
    void serviceCallback(int index, int button_state);
    void checkButtonTransitions(uint16_t previous_state, uint16_t current_state);
    void printMakersASCII();
    void trackDataSentStatus(int status);
    void setLastMessageReceived(unsigned long time);

public:
    MakersController();
    void startController(String peer_address);
    void startReceiver();
    String getPeerAddressString();
    void readAndSend();
    int readSW1();
    int readSW2();
    int readSW3();
    int readSW4();
    int readSW5();
    int readSW6();
    int readSW7();
    int readSW8();
    int readRightJoystickSwitch();
    int readLeftJoystickSwitch();
    int readRightTrigger();
    int readLeftTrigger();
    float readLeftJoystickX();
    float readLeftJoystickY();
    float readRightJoystickX();
    float readRightJoystickY();
    unsigned long getLastMessageAge();
    void registerButtonCallback(int button, void (*cb)(int));
    void registerJoystickCallback(void (*cb)(float, float, float, float));
    float getSuccessfulTransmissionPercentage();
    void setJoystickCallbackThreshold(float threshold); 
    boolean isConnected();
};

#endif