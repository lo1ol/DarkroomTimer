# Powerfull darkroom timer with a lot of features

> [Инструкция на русском](./README_RU.md)

> This project is inspired by [this one](https://github.com/nikonov1101/kafstop-timer), but improve it and extend functions

![PlatformIO](https://github.com/lo1ol/DarkroomTimer/actions/workflows/platform-io.yml/badge.svg)
![clang-format](https://github.com/lo1ol/DarkroomTimer/actions/workflows/clang-format.yml/badge.svg)

## Manual

1. Click view btn to turn on/off a lamp
2. Hold mode btn and turn encoder, to switch mode
3. Click mode btn to navigate inside mode
4. Hold extra btn to reset current printing at any mode (inside mask mode you can fast travel between masks)
5. Turn encoder inside during run to scroll time table
6. Click start btn to start printing at any mode
7. Hold view btn and change mode btn to go in set settings mode

> **_Main idea:_** You prepare everything in light and click only Start btn diring printing

## Settings

1. Time of lamp turning on (Lag time)
2. LED Backlight
3. Beep volume
4. Time of auto turn off a lamp in view mode
5. Start with settings or not
6. Confirmation of notification -- you have to click start btn, to confirm notification

## Modes

1. F Stop test mode: ability to print tones and part tones of prints (decreasing f stop step). After that you can see the log
2. Linear test mode: simple mode for linear testing
3. Print mode: ability to stop printing at any time. Usefull for getting fast masking values
4. Mask mode: for printing with accurate masks. Click extra btn to go to the next mask. Hold extra btn and turn encoder to fast travel between masks. Click start btn when set mask to play melody after printing this mask -- usefull, when you don't want to forgot to do something. For example, change filter.
5. Rel mask mode: the same as mask mode, but operates with relative times: you set base time and relative times (in stops) to it. Notification system is disabled. To see real time in seconds: click on encoder during run
6. Split grade test modes: the same as fstop and linear test modes, but allows to set base time. After printing base, the melody is playing to not forget change filter
7. Split mask and rel mask modes: the same as mask and rel mask modes, but allows to set mask for two several filtres (0 and 5). Notification system is disabled.

## Killer features:

1. Ability to stop any mode, while it is running, by holding extra button. Usefull to get stop process if you forget something
2. Ability to set lag time: time of turning on a lamp. It's could be usefull to get accurate results. To set it hold view and change mode buttons. To detect lag time. Go to lag time set mode, choose minimum time and click start btn. Increase the time till the moment of seeing that lamp is actually turn on
3. Ability to set time of automatical stopping view. It could preserve lamp on your enlarger if you forgot to turn off it
4. Buzzer that beep one time per 1 second. Usefull to print fast mask at dark
5. Simple architecture of the project allows easily extend functionality

## How to assamble

### Circuit diagram

![Circuit](./Circuit.jpg)

### Hardware and body

My friend just create the coolest version of body for this project!!! You can find it [here](https://www.thingiverse.com/thing:6683466)

#### Components

1. [Encoder EC11](https://sl.aliexpress.ru/p?key=1wDHs4W)
2. [Buttons](https://sl.aliexpress.ru/p?key=SoBfsmF)
3. [LED](https://sl.aliexpress.ru/p?key=tgDHsiN)
4. [Buzzer](https://sl.aliexpress.ru/p?key=XcDHsAE)
5. [SSR 5V](https://sl.aliexpress.ru/p?key=AWDHsN6)
6. [Switch](https://sl.aliexpress.ru/p?key=f3PfsRC)
7. 5V Transformer -- just disassamble any USB phone adapter (5V, 2.4A is ok)
8. Arduino nano: It's better to take Arduino with ATmega 328P processor. Arduino with ATmega 168 doesn't have anought memory to store this firmware
9. Two resistos are used. First one to set brightness of backlight (160 Om) and another one to set contrast on screen (2.2 kOm). Yours values could differ.
10. You could buy any sustain pedal. I bought "Cherub wtb-006" and it's more than ok

This body has not enought space to keep transformer and SSR inside. You should put in special box for electronic. You could connect it with the body using, for example, XLR.

### Another versions of body

I saw bodies created via electrical enclosure, pupular buttons and switches. You are free to create your version of body and use another components according to your budget

## Build and upload code to Arduino

If you want just upload code to your arduino without modification, then the simpliest way to do it is using of Arduino IDE
1. Just install Arduino IDE
2. Install dep libreries: EncButton, LiquidCrystals and CRC32
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
