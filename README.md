# Powerfull darkroom timer with a lot of features

> [Инструкция на русском](./README_RU.md)

> This project is inspired by [this one](https://github.com/nikonov1101/kafstop-timer), but improve it and extend functions

![PlatformIO](https://github.com/lo1ol/DarkroomTimer/actions/workflows/platform-io.yml/badge.svg)
![clang-format](https://github.com/lo1ol/DarkroomTimer/actions/workflows/clang-format.yml/badge.svg)

## Manual

1. Click view btn to turn on/off a lamp
2. Hold mode btn and turn encoder, to switch mode
3. Click mode btn to see a log
4. Click extra btn to navigate inside modes
5. Hold extra btn to reset current printing at any mode
6. Click start btn to start printing at any mode
7. Hold view btn and change mode btn to go in set settings mode (set lag time, LED Backlight, beep volume, auto finish view time and start with settings)

## Modes

1. F Stop test mode: ability to print tones and part tones of prints (decreasing f stop step). After that you can see the log
2. Linear test mode: simple mode for linear testing
3. Print mode: ability to stop printing at any time. Usefull for getting fast masking values
4. Mask mode: for printing with accurate masks

## Killer features:

1. Ability to stop any mode, while it is running, by holding extra button. Usefull to get stop process if you forget something
2. Ability to set lag time: time of turning on a lamp. It's could be usefull to get accurate results. To set it hold view and change mode buttons. To detect lag time. Go to lag time set mode, choose minimum time and click start btn. Increase the time till the moment of seeing that lamp is actually turn on
3. Ability to set time of automatical stopping view. It could preserve lamp on your enlarger if you forgot to turn off it
4. Buzzer that beep one time per 1 second. Usefull to print fast mask at dark
5. Simple architecture of the project allows easily extend functionality

## Components

1. [Encoder EC11](https://sl.aliexpress.ru/p?key=1wDHs4W)
2. [Buttons 12x12](https://sl.aliexpress.ru/p?key=8sDHsMU)
3. [Button 6x6](https://sl.aliexpress.ru/p?key=TiDHsci)
4. [LED](https://sl.aliexpress.ru/p?key=tgDHsiN)
5. [Buzzer](https://sl.aliexpress.ru/p?key=XcDHsAE)
6. [SSR 5V](https://sl.aliexpress.ru/p?key=AWDHsN6)
7. [Switch](https://sl.aliexpress.ru/p?key=7LDHstq)
8. 5V Transformer -- just disassamble any USB phone adapter (5V, 2.4A is ok)
9. Arduino nano: It's better to take Arduino with ATmega 328P processor. Arduino with ATmega 168 doesn't have anought memory to store this firmware
10. Capacitor is used to prevent leakage from SSR. I used 1μF metal foil capacitor for 250V. If you are not planning to use LED lamps, just ignore it
11. Two resistos are used. First one to set brightness of backlight (160 Om) and another one to set contrast on screen (2.2 kOm). Yours values could differ.
12. Springs for the start btn could be taken from pen
13. You could buy any sustain pedal. I bought "Cherub wtb-006" and it's more than ok

## Circuit diagram

![Circuit](./Circuit.jpg)

## Box

[3D model of the body](./TimerBox.stl)

## Build and upload code to Arduino

If you want just upload code to your arduino without modification, then the simpliest way to do it is using of Arduino IDE
1. Just install Arduino IDE
2. Install dep libreries: EncButton and LiquidCrystals
3. Clone this git project to any folder
4. Open it from Arduino IDE
5. Choose your board -- Arduino Nano and processor -- ATmega328P
6. Click Upload button

If you want to modify this code, then I recomment to use PlatfromIO

```bash
# build from command line
pio run -t build

# upload to device
pio run -t upload
```
