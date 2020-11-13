# Software
Software description and some constructs

## Overview
THe general use of this device is to drive underglow LEDs based on a variety of inputs, with an extra few channels for additional LEDs should they be installed.

The inputs to the device are:
- OBD2/CANBUS: Direct information from the car. RPM, Throttle position, speed, gear, and anything else available.
- Self/Animations: Pre=programmed animations to drive LEDs in a set pattern.
- Turn Signals/Brakes: When installed, can light up red when braking and flash yellow when using turn signals

To choose between these, there is bluetooth or a joystick/button combo.

### Menu
 Structured like so:
 Mode
 - Car driven/gauge
 - - Choose PID
 - - - Gauge 1: One color, leds go from back to forward
 - - - Gauge 2: One color, LEDs go from forward to back
 - - - Gauge 3: Three colors, green bottom end, yellow middle and red for high
 - - - etc...
 - LED patterns
 - - Solid color
 - - Chasing colors
 - - Rainbow
 - - etc..
 Settings
 - Bluetooth
 - - Bluetooth password
 - - Pair new device
 - Automatic driving shutoff (cuts lights when driving for legal reasons)

Honestly, the most challenging part as this eats memory for breakfast and has dynamically allocated memory to boot.

### OLED
A monochrom OLED display. Generic, used to display information about settings. 

### CAN/OBD2
The CAN/OBD2 libraries in use handle most of the packet send and receive pattern, only requiring a PID to get the relevant data in the correct units.

There are also helper functions to determine if a PID is valid, show units, and find all valid PIDs on a network.

### Input (Joystick)
Input is managed through a 2-axis joystick with 2 buttons. One of the buttons is part of the joystick.

### EEPROM
The EEPROM is used to store settings between power cycles.

There is a struct used to save and load all of the settings. It always writes the entire structure to the EEPROM, so limit number of times settings are saved. The struct uses put() and get() to avoid complicated byte management.

The same struct is what the rest of the program uses to get settings.

There is also a reset function.

### FastLED


## Bluetooth
Bluettoth integration can be done with no additional hardware, but will require a bunch of work on the software end, including a companion app for Android and iPhone.

Bluetooth works similarly to serial communication (read: it works identically from the ESP's perspective), but the messages have to be parsed on either end and displayed to the user in a sane manner. This is handled by the App for must bluetooth controllers on the market.
