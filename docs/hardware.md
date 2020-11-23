# Hardware
Hardware descriptions and pinouts

## Major components
| Part Name | Part number | Description |
| --------- | ----------- | ----------- |
| ESP 32 | ESP-WROOM-32E | Microcontroller |
| Can Transceiver | SN65HVD233 | Handles data shifting from CANBUS to ESP |
| 9-pin Connector | Male DB9 connector | Standard connector from OBD2 |
| OBD2 Cable | OBD2 16 pin to DB9 Serial | Cable from car to device |
| OLED 0.96" | SSD1306, SPI | Screen for displaying information |
| Joystick | Joystick w/ pushbutton | Used for navigation of the menus |
| Pushbutton | Pushbutton | Used for navigation of the menus |
| Underglow | WS2811 | Blinky lights |
| Power regulator | - | Shift from 12v to 3.3v for microcontroller's use |
| Level shifter | 74HCT14 | 12v to 3.3v, used for Brake and turn signal input signals and LED control signal (~5v) |

## Secondary components
| Part Name | Part number | Description |
| --------- | ----------- | ----------- |
| 12v plug | - | Used to provide power if not using OBD2 port to poer device |
| Relay | - | Used for cutting power to LED strips in emergency |


## Functions
- Underglow control
- Underglow relay signal (For hard disconnection/emergencies)
- OBD2 connecter
- Turn signal / brake light wires (Need level shifter)
- Can get power from 12v socket or OBD2 port

## Pinout
| Item | Pin | Restrictions |
| ---- | --- | ------------ |
| LED Data 1 | 33 | No |
| LED Data 2 | 32 | No |
| LED Data 3 | 27 | No |
| LED Data 4 | 26 | No |
| LED SPARE 1 | 15 | No |
| LED SPARE 2 | 14 | No |
| Brake wire | 16 | No |
| Turn signal 1 | 34 | No |
| Turn Signal 2 | 35 | No |
| OLED CS | 4 | No |
| OLED DC | 2 | No |
| OLED Reset | 5 | No |
| OLED CLK | 18 | Hardware locked |
| SPI MOSI | 23 | Hardware locked |
| SPI MISO | 19 | Hardware locked |
| Joystick X | 36 | Analog |
| Joystick Y | 39 | Analog |
| Joystick Button | 21 | Needs internal pullup |
| Button 1 | 22 | Needs internal pullup |
| Relay signal | 13 | No |
| CAN high | 25 | No |
| CAN low | 17 | No |

## Additional parts
- 3D printed case
- PCB
- Programming interface
- Various capacitors and resistors to run board

## Notes
Logic shifter is actually hex buffer; need a voltage divider on Controller side, WS2811 seems to take 5v input without issue.

Screen D0 is CLK, D1 is MOSI.

Joystick is oriented with headers up.
- Y Axis pot is HIGH when DOWN, LOW when UP
- X Axis pot is HIGH when LEFT, LOW when RIGHT
May want to reorient for ease of mounting.
