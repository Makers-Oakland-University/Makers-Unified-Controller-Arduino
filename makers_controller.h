#ifndef MAKERS_CONTROLLER_ARDUINO_H
#define MAKERS_CONTROLLER_ARDUINO_H

// Imports
#include <Arduino.h>
#include "Arduino.h"
#include <WiFi.h>
#include <esp_now.h>
#include <stdlib.h>

// Structs
typedef struct makers_controller_message
{
    int16_t left_joy_y;
    int16_t left_joy_x;
    int16_t right_joy_y;
    int16_t right_joy_x;
    uint16_t buttons;
} makers_controller_message;

// bit locations for 'buttons' since these are all booleans
// we can save some space in the transmission by just putting them into
// a single integer.
#define S1_POS 0
#define S2_POS 1
#define S3_POS 2
#define S4_POS 3
#define S5_POS 4
#define S6_POS 5
#define S7_POS 6
#define S8_POS 7
#define R_JOY_S_POS 8
#define L_JOY_S_POS 9
#define R_TRIG_S_POS 10
#define L_TRIG_S_POS 11

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

//array for simplification of button reading code.
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
  PIN_RIGHT_JOY_SW
};

class MakersController
{
    makers_controller_message controller_data;
    esp_now_peer_info_t peerInfo;
    uint8_t broadcastAddress[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    
    // Private constructor so that no objects can be created.
    static void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);
    void setPeerAddress(String address);    
    void initIO();
    int readSwitch(int position);

public:
    MakersController();
    void begin(String peer_address);
    String getPeerAddressString();
    void readAndSend();
};

#endif