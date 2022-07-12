#include "makers_controller.h"

#ifndef MAKERS_CONTROLLER_ARDUINO_CPP
#define MAKERS_CONTROLLER_ARDUINO_CPP

MakersController::MakersController()
{
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

void MakersController::begin(String peer_address)
{
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

void MakersController::initIO()
{
    pinMode(PIN_LEFT_JOY_Y, INPUT);
    pinMode(PIN_LEFT_JOY_X, INPUT);
    pinMode(PIN_LEFT_JOY_SW, INPUT);

    pinMode(PIN_RIGHT_JOY_Y, INPUT);
    pinMode(PIN_RIGHT_JOY_X, INPUT);
    pinMode(PIN_RIGHT_JOY_SW, INPUT);

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
}

void MakersController::readAndSend()
{
    controller_data.left_joy_y = analogRead(PIN_LEFT_JOY_Y);
    controller_data.left_joy_x = analogRead(PIN_LEFT_JOY_X);

    controller_data.right_joy_y = analogRead(PIN_RIGHT_JOY_Y);
    controller_data.right_joy_x = analogRead(PIN_RIGHT_JOY_X);

    controller_data.buttons = 0;
    controller_data.buttons |= digitalRead(PIN_SW1) << S1_POS;
    controller_data.buttons |= digitalRead(PIN_SW2) << S2_POS;
    controller_data.buttons |= digitalRead(PIN_SW3) << S3_POS;
    controller_data.buttons |= digitalRead(PIN_SW4) << S4_POS;
    controller_data.buttons |= digitalRead(PIN_SW5) << S5_POS;
    controller_data.buttons |= digitalRead(PIN_SW6) << S6_POS;
    controller_data.buttons |= digitalRead(PIN_SW7) << S7_POS;
    controller_data.buttons |= digitalRead(PIN_SW8) << S8_POS;
    controller_data.buttons |= digitalRead(PIN_RIGHT_JOY_SW) << R_JOY_S_POS;
    controller_data.buttons |= digitalRead(PIN_LEFT_JOY_SW) << L_JOY_S_POS;
    controller_data.buttons |= digitalRead(PIN_RIGHT_TRIG) << R_TRIG_S_POS;
    controller_data.buttons |= digitalRead(PIN_LEFT_TRIG) << L_TRIG_S_POS;

    // Send message via ESP-NOW
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&controller_data, sizeof(controller_data));
}

int MakersController::readSwitch(int position)
{
    return (controller_data.buttons >> position) & 0x01;
}

#endif