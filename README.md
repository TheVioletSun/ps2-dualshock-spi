# PS2 wireless gamepad interfacing library

## Connection overview

The gamepad and its receiver communicate via bluetooth. The receiver is interfaced via SPI.
SPI parameters:
Clock rate - 100-500 KHz (I personally prefer 250 KHz).
Order - Least Significant Bit First
Width - 8 bit (Standard)
CPOL - 1
CPHA - 1
SS enable - when line is low.

## PS2 receiver pinout:
 _
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

## Library overview
This library is written in C and is platform independent, meaning that SPI connection should be implemented by a programmer. As a matter of fact, this library, just maps protocol commands for the PS2 controller, leaving low level details for a user of the library.

To ease the porting of the library, the `porting-spi.h` file is provided, which describes functions, which are platform dependent, to be implemented. You could implement those functions anywhere, even in main.c, just make sure that the implementation is visible from the places where you call the library API.

