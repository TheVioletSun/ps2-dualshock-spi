#include "ps2_dualshock_configuration.h"

/* 
Button mapping values (digital mode)
Button state is returned by the receiver in the 3d and 4th bytes
Thus, for convenience, we encode each button value as 2-byte value (3d + 4th byte)
That is if button value is returned in the 3d byte and looks like 00010000, we represent it as
[00010000][00000000]
    3d        4th
 */
#define UP_BUTTON	0x1000
#define DOWN_BUTTON	0x4000
#define LEFT_BUTTON	0x8000
#define RIGHT_BUTTON	0x2000
#define START_BUTTON	0x0800
#define SELECT_BUTTON	0x0100
#define CROSS_BUTTON	0x0040
#define CIRCLE_BUTTON	0x0020
#define SQUARE_BUTTON	0x0080
#define TRIANGLE_BUTTON	0x0010
#define L_TRIGGER_BUTTON	0x0001
#define R_TRIGGER_BUTTON	0x0002
#define L_SHIFT_BUTTON	0x0004
#define R_SHIFT_BUTTON	0x0008
#define L_STICK_BUTTON	0x0200
#define R_STICK_BUTTON	0x0400

