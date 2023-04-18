//
// Created by long.nguyenviet on 02/01/2023.
//

#ifndef ESP8266_ESP12_BUTTONPRESSTRIGGER_H
#define ESP8266_ESP12_BUTTONPRESSTRIGGER_H
#include <unordered_map>
#include <vector>
#include "Button.h"
#include <Arduino.h>

#define MIN_TIME_PRESS_INTERRUPT 300
#define MIN_TIME_HOLD_INTERRUPT 3000

typedef std::function<void(const unsigned long ms)> PressCallback;
class ButtonPressTrigger {
private:
    std::unordered_map<uint, Button> _map;
    std::vector<unsigned  int> _exceptionPins = {16};
    std::unordered_map<unsigned  int, unsigned long> _registeredPins;
    std::unordered_map<unsigned  int, PressCallback> _callbacks;
    void _checkButtons();
public:
    void registerButtonPin(unsigned int pin, PressCallback callback, const ButtonMode& mode);
    void registerButtonPin(unsigned int pin, PressCallback callback);
    void onPinTriggered(unsigned int pin);
    void run();
};


#endif //ESP8266_ESP12_BUTTONPRESSTRIGGER_H
