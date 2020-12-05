# LED Patterns
A list of patterns to implement.

Pixel order is clockwise, starting at front drivers corner front LED strip by default.

Use a pallette for storing colors. This allows for blending.

User defines up to four colors and the pallete adjusts for that

Special modes and effects are patterns, but don't utilize the data the same way.

Current test LEDS:
- Front/Back LED count is 18 pixels in groups of three
- Side LED count is 30 Pixels in groups of three.

## Patterns
Basic patterns, user programmable

1. 1 Wide: Two colors, odd pixels are color 1, evens color 2
2. 2 wide: every two pixels alternates color 1 and 2
3. 3 Wide: every three pixels...
4. 6 wide: every six pixels...
5. Split 50/50
6. Split 25/25/25/25
7. Dot(s) (with trail when animated)

## Animations
Can be applied to basic patterns

1. Cycle clockwise (Sawtooth?)
2. Cycle counterclockwise
3. Fade in and out
4. Fade in and out, alternating
5. Fade, random
6. Sinewave shift (Cylon)
7. Dot shift
8. Color pop - random color appears at random location. Spreads out until next pixel covers it up
9. Splatter - color(s) fades as it gets closer to back of vehicle
10. Drip - color(s) randomly, suddenly appears and fades

## Special patterns
Preset patterns, more complex than the "simple" patterns

1. Pacifica - Looks like water flowing 
2. Rising flames - Kind of like a candle
3. Twinklefox - Snow?
4. Police - red and blue alternating with variable timings
5. MURICA - Blue and white in the front, red and white down sides and rear. Flutters?
6. Valentine's - Pink, white and purple
7. Shamrock - Green shifting pattern
8. Halloween - Orange, green and purple
9. Christmas - Red and green lights, white twinkles

## Colors
### Built in palettes
1. RainbowColors_p
2. RainbowStripeColors_p
3. OceanColors_p
4. CloudColors_p
5. LavaColors_p
6. ForestColors_p
7. PartyColors_p

### Pre-defined colors
1. Red
2. Green
3. Blue
4. White
5. Pink
6. Purple
8. Cyan
9. Yellow

More need added...

## Gauges
### Monochrome
1. Back to forwards (Front and back strips point inwards)
2. Forwards to back (Front and back strips point outwards)
3. Current gear (Brightness, color, or number of illuminated pixels)

### Multicolor
1. Back to forwards with "red" zone
2. Forwards to back with "red" zone
3. Layered gauges (Tracking two stats, overlaid)
4. Equalizer, with 'local maximum'

## Turn signal and brake hardwires
### Brakes
1. Off
2. Off when not pressed, lit when pressed
3. Dim when not pressed, lit when pressed

### Turn signals
1. Off
2. Only sides of car, solid yellow
3. Sides of car in solid yellow, Outer front and rear LEDs flash yellow as well
4. Side of car in solid yellow, rear leds flash red on/off
5. Only sides of car, chasing yellow
6. Sides of car in chasing yellow, Outer front and rear LEDs flash yellow as well
7. Side of car in chasing yellow, rear leds flash red on/off
