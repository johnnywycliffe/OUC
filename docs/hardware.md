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
| Underglow | WS2812B | Blinky lights |
| Power regulator | - | Shift from 12v to 3.3v for microcontroller's use |

## Secondary components
| Part Name | Part number | Description |
| --------- | ----------- | ----------- |
| 12v plug | - | Used to provide power if not using OBD2 port to poer device |
| Relay | - | Used for cutting power to LED strips in emergency |
| Level shifter | - | 12v to 3.3v, used for Brake and turn signal input signals |

## Functions
- Underglow control
- Underglow relay signal (For hard disconnection/emergencies)
- OBD2 connecter
- Turn signal / brake light wires (Need level shifter)
- Can get power from 12v socket or OBD2 port

## Pinout
| Item | Pin | Restrictions |
| ---- | --- | ------------ |
| Underglow data 1 |
| LED Data 1 |
| LED Data 2 |
| LED Data 3 |
| Brake wire |
| Turn signal 1 | 
| Turn Signal 2 |
| OLED CS | 4 | No |
| OLED DC | 2 | No |
| OLED Reset | 5 | No |
| OLED CLK | 18 | Hardware locked |
| SPI MOSI | 23 | Hardware locked |
| SPI MISO | 19 | Hardware locked |
| Joystick X | | Analog |
| Joystick Y | | Analog |
| Joystick Button |
| Button 1 |
| Relay signal |
| CAN high |
| CAN low |

## Additional parts
- 3D printed case
- PCB
- Programming interface
- Various capacitors and resistors to run board
