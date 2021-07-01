# Software
Software description and some constructs

## Overview
The general use of this device is to drive underglow LEDs based on a variety of inputs, with an extra few channels for additional LEDs should they be installed.

The inputs to the device are:
- OBD2/CANBUS: Direct information from the car. RPM, Throttle position, speed, gear, and anything else available.
- Self/Animations: Pre-programmed animations to drive LEDs in a set pattern.
- Turn Signals/Brakes: When installed, can light up red when braking and flash yellow when using turn signals

To choose between these, there is bluetooth mode or a joystick/button combo, or can be operated by computer

## Classes (make seperate libraries?)
- Menu: Handles operation ot he menu
  - Screen: Handles screen manipulation for menu class
  - Input: Handles joystick and button presses
  - Parser: Handles input from a serial device
    - Computer: Loads settings in ardiuno terminal when attached to a computer
    - Bluetooth: Loads data in when attached to bluetooth
- LEDPreset: Sets LEDs to be displayed
  - LEDPattern: Only contains data for an individual pattern
  - PatternManager: Actually updates patterns, inits them based on LEDPattern fed
  - LEDManager: Handles all hardware interface functions
- CAN: handle back and forth info to and from car
- EEPROM manager: Saving data to eeprom
- Settings: Settings stuff

## LED setup
- 5 presets saved to EEPROM, can be recalled at any time
- 1 Active preset; preset that will run when settings are finalized/aborted
- 1 preset for currently being modified patterns (called test)
- 1 LED manager
- 1 Pattern Manager

### Breakdown of LED operational theory
LEDManager:
- Has reference to real LEDs, all hardware driving functions
- Can call update function
- Shared between presets

LEDPreset:
- Generates pattern string with given preset
- Each preset has a pattern defined for each of the three strings: Underglow, Spare1 and Spare2
- These subpatterns are built into an overarching pattern which is loaded into LEDmanager

LEDPatterns:
- Where the current pattern is defined. Contains data like offset, length, brightness, palette...
- Does NOT contain LED array. Just information needed to construct array
- Probably could be a struct.

PatternManager:
- Given a LEDPattern, an offset and an Array to operate on, updates LEDs accordingly
- Shared between presets
- This class is going to setup and update the LEDs based on confiuration handed to it

## Menu
### Actives
- Load a new menu, resets sel and runs setupmenu(State)
- Load a new menu with a limited number of options
- Load a new menu with a custom title
- Save active pattern to test pattern
- Save test pattern to current pattern - abstract to two?
- Load a new menu and throw an eror for caution (Wiring, OBD-II, etc)
- Load a new menu and update selected LED string
- Save RGB order into hardware struct
- Save LED count into hardware struct
- Save LED flip state into hardware struct
- Save turn signal pattern into device settings
- Save brake pattern into device settings
- Save autoshutoff state into device settings
- Bluetooth

### Passives
- Update color palette on active: Active is known, state and sel describe 
- Update screen brightness: deviceSettings is known, 

# OUTDATED
Anything below this needs to be revised/removed

### Menu
The menu has a few modes:
- Navigation: consists of menu and submenues
- Choose color/pallete: Used to set colors
- Choose text: Used to set text (if needed)
- Choose PID: Choose a valid PID to monitor for gauges
- Variable adjust: Brightness, speed, anything represented as 1 number

LED patterns will continue to run while running settings.
- Custom patterns will change in real time for ease of modification
- Preset patterns will display when selecting them
- When choosing direction, indicate clockwise and have a slow clockwise pattern
- When choosing RGB, get initialization parameters and adjust accordingly. This may be difficult.
- When setting brakes and turn signals, provide a preview in real time

### OLED
A monochrome OLED display. Generic, used to display information about settings. 

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
See Patterns document for LED implementation.

## Settings
Some settings:
- Reverse direction of LED bar (For incorrect instals
- Set LED bar length
- Bluetooth on/of
- Bluetooth password (if possible)
- Blackout mode

## Bluetooth
Bluetooth integration can be done with no additional hardware, but will require a bunch of work on the software end, including a companion app for Android and iPhone.

Bluetooth works similarly to serial communication (read: it works identically from the ESP's perspective), but the messages have to be parsed on either end and displayed to the user in a sane manner. This is handled by the App for most bluetooth controllers on the market.

A standard for setting the LEDs needs to be made.
