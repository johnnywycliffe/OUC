# LED Patterns
A list of patterns to implement.

Front/Back LED count is 18 pixels in groups of three
Side LED count is 30 Pixels in groups of three.

Use a pallette for storing colors. This allows for blending.

User defines up to four colors and the pallette adjusts for that

Special modes and effects are patterns, but don't utilize the data the same way.

## Struct parts
- preset ID
- saved pallette
- Animation
- Brightness
- Blend
- Offset (pixel offset for static patterns)
- Pattern

## Patterns
Basic patterns, user programmable

1. 1 Wide: Two colors, odd pixels are color 1, evens color 2
2. 2 wide: every two pixels alternates color 1 and 2
3. 3 Wide: every three pixels...
4. 6 wide: every six pixels...
5. Split forwards/back (subdivide into chunks?)
6. Split left/right (subdivide into chunks?)
7. Gradient f/b
8. Gradient l/r

## Animations
Can be applied to basic patterns

1. Cycle clockwise
2. Cycle counterclockwise
3. Fade in and out
4. Fade in and out, alternating
5. Fade, random
6. Sinewave shift
7. Sawtooth shift

## Special patterns
Preset patterns, more complex than 

1. Pacifica - Looks like water flowing 
2. Rising flames - Kind of like a candle
3. Twinklefox - Snow?
4. Police - red and blue alternating with variable timings
5. MURICA - Blue and white in the front, red and white down sides and front
6. Color pop - random color appears at random location. Spreads out until next pixel covers it up
7. Splatter - color(s) fades as it gets closer to back of vehicle
8. Drip - color(s) randomly appears and fades

## Colors
### Built in palettes
1. RainbowColors_p
2. RainbowStripeColors_p
3. OceanColors_p
4. CloudColors_p
5. LavaColors_p
6. ForestColors_p
7. PartyColors_p

### Custom palettes

## Gauges
### Monochrome
1. Back to forwards (Front and back strips point inwards)
2. Forwards to back (Front and back strips point outwards)
3. Current gear (Brightness, color, or number of illuminated pixels)

### Multicolor
1. Back to forwards with "red" zone
2. Forwards to back with "red" zone
3. Layered gauges (Tracking two stats, overlaid)

## Turn signal and brake hardwires
### Turn signal and brakes 1
Left and right are completely yellow when signal engaged
Rear is dimly red unless brake is pressed, then gets brighter.
### Turn signal and brakes 2
Left and right are completely yellow when signal engaged. 
Rear is dimly red unless brake is pressed, then gets brighter. 
If turn signal is active, front and rear strip  outer two sets of LEDs also flash yellow
