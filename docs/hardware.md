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
| Level shifter | 74HCT14 | 3.3v to 5v, used for LED control signal (~5v) |

## Secondary components
| Part Name | Part number | Description |
| --------- | ----------- | ----------- |
| 12v plug | - | Used to provide power if not using OBD2 port to poer device |
| Relay | - | Used for cutting power to LED strips in emergency |
| Voltage divider | - | For brakes and turn signals |


## Functions
- Underglow control
- Underglow relay signal (For hard disconnection/emergencies)
- OBD2 connecter
- Turn signal / brake light wires (Need level shifter)
- Can get power from 12v socket or OBD2 port

## Pinout
Needs sorting
| Item | Pin | Restrictions | Vetted |
| ---- | --- | ------------ | ------ |
| LED Data 1 | 33 | No | No |
| LED Data 2 | 32 | No | No |
| LED Data 3 | 27 | No | No |
| LED Data 4 | 26 | No | No |
| LED SPARE 1 | 15 | No | No |
| LED SPARE 2 | 14 | No | No |
| Brake wire | 16 | No | No |
| Turn signal 1 | 34 | No | No |
| Turn Signal 2 | 35 | No | No |
| OLED CS | 4 | No | Yes |
| OLED DC | 2 | No | Yes |
| OLED Reset | 5 | No | Yes |
| OLED CLK | 18 | Hardware locked | Yes |
| SPI MOSI | 23 | Hardware locked | Yes |
| SPI MISO | 19 | Hardware locked | Yes |
| Joystick X | 36 | Analog | Yes |
| Joystick Y | 39 | Analog | Yes |
| Joystick Button | 21 | Needs internal pullup | Yes |
| Button 1 | 22 | Needs internal pullup | Yes |
| Relay signal | 13 | No | No |
| CAN high | 25 | No | No |
| CAN low | 17 | No | No |

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
