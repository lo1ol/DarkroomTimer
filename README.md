# Advanced darkroom timer (KAF Red_Ray Timer RR-1)

> [Инструкция на русском](./README_RU.md)

> This project is inspired by [this one](https://github.com/nikonov1101/kafstop-timer), but improve it and extend functions

> [3D Model of the body](https://www.thingiverse.com/thing:6683466)

> **Main idea:** You prepare everything in light and click only Start btn during printing

> [Online simulator](https://wokwi.com/projects/432833785679137793). Note, you can use keyboard to control timer

![PlatformIO](https://github.com/lo1ol/DarkroomTimer/actions/workflows/platform-io.yml/badge.svg)
![clang-format](https://github.com/lo1ol/DarkroomTimer/actions/workflows/clang-format.yml/badge.svg)

## Manual

1. Click view btn to turn on/off a lamp
2. Hold mode btn and turn encoder, to switch mode
3. Click mode btn to navigate inside mode
4. Hold extra btn to reset current printing at any mode (inside mask mode you can fast travel between masks)
5. Turn encoder in time table to scroll it
6. Click start btn to start printing at any mode
7. Hold view and change mode btns to go to settings

## Settings

1. Lamp's heatup time (Lag time)
2. LED Backlight
3. Beep volume
4. Auto turn off lamp time in view mode
5. Start with settings or not

## Modes

1. F Stop test mode: increae testing time in stops
2. Linear test mode: simple mode for linear testing
3. Print mode: ability to pause printing at any time. It's creative mode and usefull for thoughtless printing
4. Mask mode: for printing with accurate masks. Click extra btn to go to the next mask. Hold extra btn and turn encoder to fast travel between masks. Click start btn when set mask to play melody after printing this mask -- usefull, when you don't want to forgot to do something. For example, change filter.
5. Rel mask mode: the same as mask mode, but operates with relative times: you set base time and relative times (in stops) to it. Notification system is disabled. To see real time in seconds: click on encoder during run
6. Split grade test modes: the same as fstop and linear test modes, but allows to set base time. After printing base, the melody is playing to not forget change filter
7. Split mask and rel mask modes: the same as mask and rel mask modes, but allows to set mask for two filtres (0 and 5). Notification system is disabled.
8. Localized test modes the same as fstop and linear test modes, but used to test the same region on photo. The time you see -- it's whole printing test time

## Killer features:

1. Ability to stop any mode, during printing. Just hold extra button. Usefull to stop process, if you forgot something
2. Ability to set lag time: lamp. It's could be usefull to get accurate results. To find it go to settings, choose minimum lag time and click start btn. Increase it till the moment of seeing that lamp is actually turn on
3. Autoturn off in view mode. It could preserve lamp on your enlarger if you forgot to turn off it
4. Buzzer that beep every second. Usefull to control the process, not looking at display
5. Simple architecture of the project allows easily extend functionality

## How to assamble

### Circuit diagram

#### Main part

> Note: Dashed part is power supply block. It's not a part of main body. The datailed scheme of power supply is shown lower

![Main body circuit](./MainBodyCircuit.jpg)

#### Power supply

![Power supply circuit](./PowerSupplyCircuit.jpg)


### Hardware and body

My friend just create the coolest version of body for this project!!! You can find it [here](https://www.thingiverse.com/thing:6683466)

#### Components

1. [Encoder](https://sl.aliexpress.ru/p?key=d3DhGvG)
2. [Buttons](https://sl.aliexpress.ru/p?key=SoBfsmF)
3. [LED](https://sl.aliexpress.ru/p?key=tgDHsiN)
4. [Buzzer](https://sl.aliexpress.ru/p?key=iXIeG78)
5. [Switch](https://sl.aliexpress.ru/p?key=f3PfsRC)
6. 5V Transformer -- just disassamble any USB phone adapter (5V, 2.4A is ok). Also you can [try this](https://sl.aliexpress.ru/p?key=5RdhGtS), but I still don't check it properly
7. [Arduino nano](https://sl.aliexpress.ru/p?key=daDhGj5): It's better to take Arduino with ATmega 328P processor. Arduino with ATmega 168 doesn't have anought memory to store this firmware. Notice, controller with mini usb socket is needed
8. Only one resistor is used to set contrast of screen: 2.2 kOm. Your value could differ.
9. You could buy sustain pedal as footswitch. It has to be normally-open or has polarity switch. We use "Cherub wtb-006" and it's more than ok
10. [Jack 6.35](https://sl.aliexpress.ru/p?key=W4aeGRp)
11. SSR:
    * If you want to use timer with any lamp (LED, halogen). You can buy any of it:
        - [D3803HK/D3805HK/D3808HK](https://sl.aliexpress.ru/p?key=xlW6GV3)
        - [BERM BRM-D480-5A](https://sl.aliexpress.ru/p?key=KyW6GRj)
        - [SSR-41FDA](https://sl.aliexpress.ru/p?key=oBW6G3x)
    * If you don't want to use timer with low wattage lamps (LED): you can buy [this popular SSR](https://sl.aliexpress.ru/p?key=wXDhGqU). This one has leakage in turned off state, and it's enough to power on the lamp

> **WARNING**: Don't use solenoid relays. It can lead to hieroglyphs on the display and even kill Arduino

This body has not enought space to keep transformer and SSR inside. You should put in special box for electronic. You could connect it with the body using, for example, XLR.

Body extra components:

1. [XLR](https://sl.aliexpress.ru/p?key=bFaeGJn) -- is used to connect main part with power block
2. [Acrilic black glass](https://sl.aliexpress.ru/p?key=tvaeGQb) -- cover display
3. [Power block sockets](https://sl.aliexpress.ru/p?key=xoaeGXb)
4. [Rocket switcher for power block](https://sl.aliexpress.ru/p?key=moaeG8T)
5. [M2 screws](https://sl.aliexpress.ru/p?key=rRaeGCg)
6. M3 screw and nuts also needed but you could buy it everywhere you want
7. Any AC plug you want
8. 3 wire cablels: you need thin (between power block and main part) and thick ones (between power block and socket)

### Other versions of body

I saw bodies created via electrical enclosure, pupular buttons and switches. You are free to create your version of body and use another components according to your budget

## Build and upload code to Arduino

If you want just upload code to your arduino without modification, then the simpliest way to do it is using of Arduino IDE
1. Install Arduino IDE
2. Clone this git project to any folder
3. Open ino file from Arduino IDE (`File`->`Open...`->`path to .ino` file)
4. Install dep libreries (`Tools` -> `Manage Libraries...`):
   * EncButton
   * LiquidCrystals
   * CRC32 (by Crystopher Baker)
5. Choose your board -- Arduino Nano and processor -- ATmega328P
6. Click Upload button

If you want to modify this code, then I recomment to use PlatfromIO:

```bash
# build from command line
cd DarkroomTimer/
pio run -t build

# upload to device
pio run -t upload
```

### Firmware configuration

You can configure the firmware by your needs. For this, you have to change macroses in [Config.h](DarkroomTimer/src/Config.h) file:
* `LCD_*` -- pin and arduino to `*` pin on display
* `MODE_SWITCH_BTN` -- pin to mode button
* `VIEW_BTN` -- pin to view button
* `START_BTN` -- pin to start button
* `EXTRA_BTN` -- pin to button on encoder
* `ENCODER_DT` -- pin to DT(S1) pin on encoder (UNRECOMMENDED TO CHANGE)
* `ENCODER_CLK` -- pin to CLK(S2) pin on encoder (UNRECOMMENDED TO CHANGE)
* `BEEPER` -- pin to buzzer (UNRECOMMENDED TO CHANGE)
* `BACKLIGHT` -- pin to backlight control pin on display (marked as `A` on display)
* `RELAY` -- pin to SSR
* `MAX_BACKLIGHT` -- maximum backlight value (maximum value is 25)
* `MIN_BEEP_VOLUME` -- minimum buzzer volume (can be different on different buzzers)
* `BEEP_VOLUME_STEP` -- step size of buzzer valume

> NB: Swapping of ENCODER_DT with ENCODER_CLK change direction of encoder

## Support

If you have any questions, problems or suggestions, you are welcome to write us:)

Bogdan (design, 3d pringing):<br>
tg: [@bogdanbogvzyan](http://t.me/bogdanbogvzyan)<br>
email: [panasyukbv@yandex.ru](mailto:panasyukbv@yandex.ru?subject=Darkroom%20timer)

Petr (code, assembling):<br>
tg: [@lo1ol](http://t.me/lo1ol)<br>
email: [myprettycapybara@gmail.com](mailto:myprettycapybara@gmail.com?subject=Darkroom%20timer)
