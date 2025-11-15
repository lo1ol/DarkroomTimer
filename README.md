# Advanced darkroom timer (KAF Red_Ray Timer RR-1)

> [Инструкция на русском. С более удобными ссылками на компоненты](./README_RU.md)

> This project is inspired by [this one](https://github.com/nikonov1101/kafstop-timer), but improve it and extend functions

> [3D Model of the body](https://www.thingiverse.com/thing:6683466)

> [Youtube playlist with assembling manual (in Russian)](https://www.youtube.com/watch?v=XjmDx0XmCiA&list=PLLB8YXu4D8PvZlZFQPj_stn8mG01y7iiN&pp=gAQB)

> **Main idea:** You prepare everything in light and click only Start btn during printing

> [Online simulator](https://wokwi.com/projects/447704861114129409). Note, you can use keyboard to control timer

## Manual

1. Click view btn to turn on/off a lamp
2. Hold mode btn and turn encoder, to switch mode
3. Click mode btn to navigate inside mode
4. Hold encoder btn to reset current printing at any mode (inside mask mode you can fast travel between masks)
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
4. Mask mode: for printing with accurate masks. Click encoder btn to go to the next mask. Hold encoder btn and turn encoder to fast travel between masks. Click start btn when set mask to play melody after printing this mask -- usefull, when you don't want to forgot to do something. For example, change filter.
5. Rel mask mode: the same as mask mode, but operates with relative times: you set base time and relative times (in stops) to it. Notification system is disabled. To see real time in seconds: click on encoder during run
6. Split grade test modes: the same as fstop and linear test modes, but allows to set base time. After printing base, the melody is playing to not forget change filter
7. Split mask and rel mask modes: the same as mask and rel mask modes, but allows to set mask for two filtres (0 and 5). Notification system is disabled.
8. Localized test modes the same as fstop and linear test modes, but used to test the same region on photo. The time you see -- it's whole printing test time. [3D model of the test-strip maker](https://www.thingiverse.com/thing:7048981)
9. Expert tests modes (disabled by default): include functionality of all test modes. Allows to set custom time of first exposure (preflash. Part of split-grade). Specify type of the test (localized or generic). You could use them instead of all standard modes. Only one disadvantage of this modes is a bug number of sum menues

## Killer features:

1. Ability to stop any mode, during printing. Just hold encoder button. Usefull to stop process, if you forgot something
2. Ability to set lag time: lamp. It's could be usefull to get accurate results. To find it go to settings, choose minimum lag time and click start btn. Increase it till the moment of seeing that lamp is actually turn on
3. Autoturn off in view mode. It could preserve lamp on your enlarger if you forgot to turn off it
4. Buzzer that beep every second. Usefull to control the process, not looking at display
5. Simple architecture of the project allows easily extend functionality

## How to assamble

### Circuit diagram

#### Main part

> Note: Dashed part is power supply block. It's not a part of main body. The datailed scheme of power supply is shown lower

![Main body circuit](./docs/MainBodyCircuit.jpg)

#### I2C version of LCD

> Note: this circuit shows only how to connect I2C adapter to arduino. Everything else is connected the same way

> Note: Not forget to set `LCD_VERSION` macro to `LCD_VERSION_I2C` at [UserConfig.h](./DarkroomTimer/UserConfig.example.h)

![I2C LCD circuit](./docs/I2C_LcdCircuit.jpg)

#### Power supply

![Power supply circuit](./docs/PowerSupplyCircuit.jpg)


### Hardware and body

My friend just create the coolest version of body for this project!!! You can find it [here](https://www.thingiverse.com/thing:6683466)

#### Components

> Note: This is a list of items that I used during building. You could try other similar offers if they are chipper for you, but I couldn't garantee they fit.

1. [Encoder EC11 module](https://aliexpress.com/item/1005007038297328.html)
2. [Buttons 3 pcs](https://aliexpress.com/item/1005005783406100.html)
3. [Active Buzzer 5V](https://aliexpress.com/item/1005004207766097.html)
4. [Toggle switch SS12F15G5 2 pcs](https://aliexpress.com/item/33001964763.html)
5. [5V Transformer 700mA](https://aliexpress.com/item/33011812383.html) (still don't check it properly). Also you can just disassamble any 5V USB phone adapter
6. [Arduino Nano ATMEGA328P/Type-C or Mini USB/WITHOUT WELDING](https://aliexpress.com/item/32915861640.html)
7. [Jack 6.35 (mono)](https://www.aliexpress.com/item/3256805643675386.html)
8. SSR:
    * If you want to use timer with any lamp (LED, halogen). You can buy any of it:
        - [D3803HK/D3805HK/D3808HK](https://aliexpress.com/item/1005003616598178.html)
        - [BERM BRM-D480-5A](https://aliexpress.com/item/1005005177941225.html)
        - [SSR-41FDA](https://aliexpress.com/item/1005007299427443.html)
    * If you don't want to use timer with low wattage lamps (LED): you can buy [this popular 5V SSR](https://aliexpress.com/item/33005799623.html). This one has leakage in turned off state, and it's enough to power on the lamp
9. Red LCD 1602:
    * [LCD without I2C adapter](https://aliexpress.com/item/1709948156.html). Has really good angle of view compare to next one, but harder to solder. Requires additional resistor 2.2kOm (value could be differ from mine). You could solder with own I2C adapter
    * [LCD with I2C adapter](https://aliexpress.com/item/32906948449.html). Most convinient option, but not so good contrast. Need to soldure just 5 pins. You can adjust contrst on the I2C module
10. [Sustain pedal as footswitch](https://aliexpress.com/item/1005008834402930.html) (Optional). You could buy other one. It has to be normally-open or has polarity switch.

> **WARNING**: Don't use solenoid relays. It can lead to hieroglyphs on the display and even kill Arduino

This body has not enought space to keep transformer and SSR inside. You should put in special box for electronic. You could connect it with the body using, for example, XLR.

Body extra components:

1. [XLR male and female](https://aliexpress.com/item/1005006112577152.html) -- is used to connect main part with power block
2. [3mm black transparent acrilic glass](https://aliexpress.com/item/1005002538243937.html) -- cover display
3. [Power block sockets](https://aliexpress.com/item/2043773958.html)
4. [Rocket switcher for power block](https://aliexpress.com/item/1005004320737354.html)
5. [M2 6mm screws 10 pcs](https://aliexpress.com/item/1005003194617253.html)
6. M3 screw and nuts also needed but you could buy it everywhere you want
7. [3 wire cabel 0.75 with AC Plug](https://aliexpress.com/item/1005004840824679.html) (Not tested)
8. [24AWG cabel 6 color set (5 meters each)](https://aliexpress.com/item/1005006397237788.html)
9. [Heat shrink tube 2 meters 4.8mm](https://aliexpress.com/item/1005001954843758.html) (Not tested)
10. [Transparent two sides tape 3mm](https://aliexpress.com/item/4000282953023.html)

### Low-budget versions of device

You can made body cheeper if
1. Don't install acrilic black glass. Just dim display in settings
2. Connect body to power supply directly without XLR
3. Use single color filament printed with 0.4 nozzle
4. Don't print text on body
5. Not use our version of power supply. Just put SSR and transformer into simple electrical enclosure and connect AC socket using wire to it

## Build and upload code to Arduino

If you want just upload code to your arduino without modification, then the simpliest way to do it is using of Arduino IDE
1. Install Arduino IDE
2. Clone this git project to any folder
3. Open ino file from Arduino IDE (`File`->`Open...`->`path to .ino` file)
4. Install dep libreries (`Tools` -> `Manage Libraries...`):
   * EncButton
   * CRC32 (by Crystopher Baker)
5. Choose your board -- Arduino Nano and processor -- ATmega328P
6. Click Upload button

If you want to modify this code, then I recomment to use PlatfromIO:

```bash
# build from command line
pio run -t build

# upload to device
pio run -t upload
```

### Firmware configuration

You can configure the firmware by your needs. For this, you have to create a file UserConfig.h in near to 'DarkroomTimer.ino' file and define needed macroses. All possiable values and description specified in [UserConfig.example.h](DarkroomTimer/UserConfig.example.h). Example configuration:

```cpp
// UserConfig.h

// Change SSR input PIN
// You need to specify it, if assamble Timer by guide of 0.5.1 version or lower
#define RELAY_PIN A4

// Change encoder direction
#define ENCODER_DT_PIN 3
#define ENCODER_CLK_PIN 2

// make encoder move faster
#define ENCODER_FAST_TIMEOUT 34
#define ENCODER_FAST_FAST_TIMEOUT 9

// Dim LCD if we not install acrylic cover
#define LCD_BACKLIGHT_STEP 2

// Make beeper quieter
#define BEEPER_VOLUME_MIN 10

// Fix beeper make a noise in turned off state
#define BEEPER_VOLUME_SILENT 0

// Switch to I2C implementation for LCD
#define LCD_VERSION LCD_VERSION_I2C

// 1. Disable local and split grade test modes and rel mask modes
// 2. Enable expert test modes
// 3. Change standard order of modes
#define TIMER_MODES       \
    fStopTest,            \
    linearTest,           \
    print,                \
    mask,                 \
    splitGradeMask,       \
    expertFStopTest,      \
    expertLinearTest,     \
    last_,                \
    localizedFStopTest,   \
    localizedLinearTest,  \
    splitGradeFStopTest,  \
    splitGradeLinearTest, \
    relMask,              \
    splitGradeRelMask
```

We will try not change name of config macroses. So, you can use this file for different versions of firmware

## FAQ

> The beeper sounds continiously

Define this at UserConfig.h:
```cpp
#define BEEPER_VOLUME_SILENT 0
```

> I updated  firmware and timer stoped to control enlarger

After 0.5.1 version default Relay pin was moved from  A4 to D4 pin. You can add this line to UserConfig.h to fix it:
```cpp
#define RELAY_PIN A4
```

## Support

If you have any questions, problems or suggestions, you are welcome to write us:)

Bogdan (design, 3d pringing):<br>
tg: [@bogdanbogvzyan](http://t.me/bogdanbogvzyan)<br>
email: [panasyukbv@yandex.ru](mailto:panasyukbv@yandex.ru?subject=Darkroom%20timer)

Petr (code, assembling):<br>
github: create issue on github if you have any questions and suggestions
tg: [@lo1ol](http://t.me/lo1ol)<br>
email: [myprettycapybara@gmail.com](mailto:myprettycapybara@gmail.com?subject=Darkroom%20timer)
