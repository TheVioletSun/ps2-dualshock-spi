#include <stdint.h>
#include <stddef.h>
#include "ps2_dualshock_spi.h"
#include "porting_spi.h"

/* Buffers for transfer commands */
#define MAIN_EXCHANGE_SIZE 21
uint8_t MAIN_EXCHANGE_TX[MAIN_EXCHANGE_SIZE] = {
	0x01, 0x42, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

#define ENTER_CONFIG_MODE_SIZE 5
uint8_t ENTER_CONFIG_MODE_TX[ENTER_CONFIG_MODE_SIZE] = {
	0x01, 0x43, 0x00, 0x01, 0x00
};

#ifdef TURN_ON_ANALOG
#define ENABLE_ANALOG_MODE_SIZE 9
uint8_t ENABLE_ANALOG_MODE_TX[ENABLE_ANALOG_MODE_SIZE] = {
	0x01, 0x43, 0x00, 0x01, 0x03, 0x00, 0x00, 0x00, 0x00
};
#endif

#ifdef TURN_ON_MOTORS
#define TURN_ON_MOTORS_SIZE 9
uint8_t TURN_ON_MOTORS_TX[TURN_ON_MOTORS_SIZE] = {
	0x01, 0x4D, 0x00, 0x00, 0x01, 0xFF, 0xFF, 0xFF, 0xFF
};
#endif

#ifdef TURN_ON_BUTTON_PRESSURE
#define ENABLE_BUTTON_PRESSURE_SIZE 9
uint8_t ENABLE_BUTTON_PRESSURE_TX[ENABLE_BUTTON_PRESSURE_SIZE] = {
	0x01, 0x4F, 0x00, 0xFF, 0xFF, 0x03, 0x00, 0x00, 0x00
};
#endif

#define EXIT_CONFIG_MODE_SIZE 9
uint8_t EXIT_CONFIG_MODE_TX[EXIT_CONFIG_MODE_SIZE] = {
	0x01, 0x4F, 0x00, 0xFF, 0xFF, 0x03, 0x00, 0x00, 0x00
};

static void ps2_to_idle_state(ps2_dualshock_dev *dev);

/* Function implementation */
void ps2_init(ps2_dualshock_dev *dev, void* spi_h)
{
	dev->spi_h = spi_h;
	dev->tx_rx = tx_rx;
	dev->delay_ms = delay_ms;
	dev->is_analog_on = 1;
	dev->is_button_pressure_on = 1;
	dev->is_motor_on = 1;

	ps2_to_idle_state(dev);
}

uint8_t ps2_main_exchange(ps2_dualshock_dev *dev)
{
	uint8_t MAIN_EXCHANGE_RX[MAIN_EXCHANGE_SIZE] = {0};
	if(dev->tx_rx(dev->spi_h, MAIN_EXCHANGE_TX, MAIN_EXCHANGE_RX,
		   MAIN_EXCHANGE_SIZE, 20) != 0) {
		/* Something went wrong */
		ps2_to_idle_state(dev);
		return 1;
	}

	/* parse the response */
	dev->state = MAIN_EXCHANGE_RX[3] << 8 | MAIN_EXCHANGE_RX[4];

	dev->delay_ms(10, NULL);

	return 0;
}

static void ps2_to_idle_state(ps2_dualshock_dev *dev)
{
	dev->state = 0xFFFF;
}

/**
 * The second arg, buttons, taken from headers can be 'OR'ed, like 'BUTTON1 | BUTTON2'
 * Returns >0 number if any passed button was pressed, 0 otherwise
 */
uint8_t ps2_any_button_pressed(ps2_dualshock_dev *dev, uint16_t buttons)
{
	/**
	 * Reverse state bits and mask with button values.
	 * An example: if state is 1011111111111101, it becomes 0100000000000010
	 * and then:
	 * 1. For button value like 0000000100000000 it is masked to 0.
	 * 2. For button values 0000000000000110 it is masked to
	 * 0000000000000010.
	 *
         * Two exclamation marks are to cast some >0 number to 1.
	 */
	return !!((dev->state ^ 0xFFFF) & buttons);
}

/**
 * The second arg, buttons, taken from headers can be 'OR'ed, like 'BUTTON1 | BUTTON2'
 * Returns 1 if all the passed buttons were pressed, 0 otherwise
 */
uint8_t ps2_all_buttons_pressed(ps2_dualshock_dev *dev, uint16_t buttons)
{
	/**
	 * Mask state bits with button values and negate.
	 * An example: if state is 1011111111111101, then:
	 * 1. For button value like 0000000100000000 it is masked to
	 * 0000000100000000 and then negated to 0.
	 * 2. For button values 0100000000000010 it is masked to 0 and then
	 * negated to 1.
	 */
	return !(dev->state & buttons);
}

uint8_t ps2_is_idle(ps2_dualshock_dev *dev)
{
	return !(dev->state ^ 0xFFFF);
}

