# Hardware
Hardware descriptions and pinouts

## Critical components
| Part Name | Part number | Description |
| --------- | ----------- | ----------- |
| ESP 32 | ESP-WROOM-32E | Microcontroller |
| Can Transceiver | SN65HVD233 | Handles data shifting from CANBUS to ESP |
| 9-pin Connector | Male DB9 connector | Standard connector from OBD2 |
| OLED 0.96" | SSD1306, SPI | Screen for displaying information |
| Joystick | Generic Joystick w/ pushbutton | Used for navigation of the menus |
| Pushbutton | Generic Pushbutton | Used for navigation of the menus |
| Underglow | WS2811 | Blinky lights |
| Power regulator | TBD | Shift from 12v to 3.3v for microcontroller's use |
| Level shifter | 74HCT14 | 3.3v to 5v, used for LED control signal (~5v) |
| Voltage divider | TBD | For brakes and turn signals |
| USB to UART | TBD | To allow for programming device |

## Possible additional components
| Part Name | Part number | Description |
| --------- | ----------- | ----------- |
| 12v plug | - | Used to provide power if not using OBD2 port to poer device |
| Relay | - | Used for cutting power to LED strips in emergency |
| OBD2 Cable | OBD2 16 pin to DB9 Serial | Cable from car to device |

## Additional parts and things
- 3D printed case
- PCB
- Programming interface (If used, don't need USB to UART chip)
- Support components

## Pinout
Needs sorting
| Item | Pin | Restrictions | Vetted |
| ---- | --- | ------------ | ------ |
| OLED DC | 2 | No | Yes |
| OLED CS | 4 | No | Yes |
| OLED Reset | 5 | No | Yes |
| Relay signal | 12 | Boot fails if pulled high | No |
| LED Data (left) | 13 | No | Yes |
| LED SPARE 2 | 14 | No | Yes |
| LED SPARE 1 | 15 | No | Yes |
| CAN TX | 16 | No | No |
| CAN RX | 17 | No | No |
| OLED CLK | 18 | Hardware locked | Yes |
| SPI MISO | 19 | Hardware locked | Yes |
| Joystick Button | 21 | Needs internal pullup | Yes |
| Button 0 | 22 | Needs internal pullup | Yes |
| SPI MOSI | 23 | Hardware locked | Yes |
| LED Data (right) | 25 | No | Yes |
| LED Data (rear) | 26 | No | Yes |
| LED Data (front) | 27 | No | Yes |
| Joystick X | 32 | Analog | Yes |
| Joystick Y | 33 | Analog | Yes |
| Turn signal input Left | 34 | Input only | No |
| Turn Signal input Right | 35 | Input only | No |c
| Brake Input | 39 | Input only | No |

## Power
Vehicle power systems are generously described as "Hostile". So, there needs to be a system in place to get the power to everything. THe voltage ranges from 11-14V and 10A typically, but there's spikes.

Using a MAX20008E chip to go from whatever the battery decides to provide to 5V, 8A. It's rated for automotive use as well.

Then, 2 step up chips (MIC2251 or TPS61085A) to get back to 12v. Also need a MIC23050-SYML-TR for 3.3v

Power supply circuitry alone account for like $10, and I don't even have inductors picked yet...

## Notes
Logic shifter is actually hex buffer; need a voltage divider on Controller side, WS2811 seems to take 5v input without issue.

Screen D0 is CLK, D1 is MOSI.

Joystick is oriented with headers up.
- Y Axis pot is HIGH when DOWN, LOW when UP
- X Axis pot is HIGH when LEFT, LOW when RIGHT

May want to reorient for ease of mounting.

Pin 33 spazzes on software load. 
