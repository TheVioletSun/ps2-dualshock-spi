# PS2 wireless gamepad interfacing library

## Connection overview

The gamepad and its receiver communicate via bluetooth. The receiver is interfaced via SPI.
SPI parameters:
```
Clock rate - 100-500 KHz (I personally prefer 250 KHz).
Order - Least Significant Bit First
Width - 8 bit (Standard)
CPOL - 1
CPHA - 1
SS enable - when line is low.
```

## PS2 receiver pinout:
``` _
/*|	DATA (SPI MOSI)
|*|	COMMAND (SPI MISO)
|*|	MOTOR POWER (not used with wireless gamepad)
---
|*|	GND
|*|	VCC (3.3V)
|*|	ATTENTION (SPI SS)
---
|*|	CLOCK (SPI CLK)
|*|	UNUSED
\*|	AKNOWLEDGE (not used in the library)
```

## Library overview
This library is written in C and is platform independent, meaning that SPI connection should be implemented by a programmer. As a matter of fact, this library, just maps protocol commands for the PS2 controller, leaving low level details for a user of the library.

To ease the porting of the library, the `porting-spi.h` file is provided, which describes functions, which are platform dependent, to be implemented. You could implement those functions anywhere, even in main.c, just make sure that the implementation is visible from the places where you call the library API.

## Response mapping
The main polling command results in the response from the gamepad receiver that contains control state. This response can be mapped as follows:

Control		Byte num	Byte content

Digital mode

Up		4		11101111
Down		4		10111111
Left		4		01111111
Right		4		11011111
Cross		5		10111111
Square		5		01111111
Circle		5		11011111
Triangle	5		11101111
Select		4		11111110
Start		4		11110111
L.Trigger	5		11111110
R.Trigger	5		11111101
L.Shift		5		11111011
R.Shift		5		11110111
L.Stick.press	4		11111101
R.Stick		4		11111011

Analog mode

TBD

