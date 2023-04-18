//
// Created by long.nguyenviet on 02/01/2023.
//

#include "ButtonAction.h"
static ButtonPressTrigger* selfButtonTrigger;
static bool buttonBlocking = false;
ICACHE_RAM_ATTR static void classIsr(void *p) {
    uint pin = static_cast<int>(reinterpret_cast<std::uintptr_t>(p));;
    selfButtonTrigger->onPinTriggered(pin);
}
void ButtonPressTrigger::registerButtonPin(unsigned int pin, PressCallback callback) {
    registerButtonPin(pin, callback, B_CHANGE);
}
void ButtonPressTrigger::registerButtonPin(unsigned int pin, PressCallback callback, const ButtonMode& mode) {
    //Check for special PINs which can not use interrupt feature
    if (std::find(_exceptionPins.begin(), _exceptionPins.end(), pin) != _exceptionPins.end()) {
        if (_registeredPins.find(pin) == _registeredPins.end()) {
            pinMode(pin, INPUT);
            digitalWrite(pin, HIGH);
            _registeredPins.emplace(pin, 0);
            _callbacks.emplace(pin, callback);
        }
    } else if (_map.find(pin) == _map.end()) {
        pinMode(pin, INPUT);
        _map.emplace(pin, Button {pin, ButtonState::NONE, 0, mode});
        _callbacks.emplace(pin, callback);
        if (mode == B_LOW) {
            attachInterruptArg(digitalPinToInterrupt(pin), classIsr, reinterpret_cast<void *>(pin), FALLING);
        } else if (mode == B_HIGH) {
            attachInterruptArg(digitalPinToInterrupt(pin), classIsr, reinterpret_cast<void *>(pin), RISING);
        } else {
            attachInterruptArg(digitalPinToInterrupt(pin), classIsr, reinterpret_cast<void *>(pin), CHANGE);
        }
    }
}

void ButtonPressTrigger::onPinTriggered(unsigned int pin) {
    if (buttonBlocking) {
        return;
    }
    buttonBlocking = true;
    auto vState = digitalRead(pin);
    //logging::info("Triggered interrupt on PIN : %d Value: %d", pin, vState);
    auto it = _map.find(pin);
    if (it != _map.end()) {
        auto btn = &_map[pin];
        unsigned long currentTime = millis();
        unsigned long prevTime = btn->pressedDownAt;
        if (btn->mode != ButtonMode::B_CHANGE) {
            btn->state = ButtonState::NONE;
            btn->pressedDownAt = currentTime;
            buttonBlocking = false;
            _callbacks[pin](currentTime - prevTime);
            return;
        }
        //Check button pressed down
        if (vState == LOW) {
             //Reduce noise
            if (currentTime - prevTime < MIN_TIME_PRESS_INTERRUPT || btn->state == ButtonState::DOWN) {
                buttonBlocking = false;
                //logging::warning("Reduced noise on PIN : %d", pin);
                return;
            }
            if (btn->state != ButtonState::DOWN) {
                btn->state = ButtonState::DOWN;
                btn->pressedDownAt = millis();
            }
           
        } else {
            //Check button up - released
            if (btn->state == ButtonState::DOWN) {
                _callbacks[pin](currentTime - prevTime);
                //Reset button state
                btn->state = ButtonState::NONE;
                btn->pressedDownAt = currentTime;
            } else {
                // Wrong state
            }
            
        }
        _map[pin] = *btn;
    } else {
    }

    buttonBlocking = false;
}


void ButtonPressTrigger::run() {
    _checkButtons();
}

void ButtonPressTrigger::_checkButtons() {
    for(auto pinIt: _registeredPins) {
        unsigned int pin = pinIt.first;
        unsigned long pressedTime = pinIt.second;
        auto configButton = digitalRead(pin);
        if (configButton == LOW) {
            if (pressedTime == 0 || millis() - pressedTime > MIN_TIME_PRESS_INTERRUPT) {
                digitalWrite(pin, HIGH);
                _registeredPins[pin] = millis();
                _callbacks[pin](millis() - pressedTime);
            }
        }
    }
}
