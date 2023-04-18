//
// Created by long.nguyenviet on 03/01/2023.
//

#ifndef ESP8266_ESP12_BUTTON_H
#define ESP8266_ESP12_BUTTON_H
enum ButtonState {
    NONE,
    DOWN,
    UP
};
enum ButtonMode {
    B_CHANGE,
    B_HIGH,
    B_LOW,
    B_BOTH
};
struct Button {
    unsigned int pin;
    ButtonState state;
    unsigned long pressedDownAt = 0;
    ButtonMode mode;
};
#endif //ESP8266_ESP12_BUTTON_H
