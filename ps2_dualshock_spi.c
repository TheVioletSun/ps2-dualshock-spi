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
	0x01, 0x43, 0x00, 0x00, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A
};

uint32_t exchange_counter = 0;

static void ps2_to_idle_state(ps2_dualshock_dev *dev);

/* Function implementation */
void ps2_init(ps2_dualshock_dev *dev, void* spi_h)
{
	dev->spi_h = spi_h;
	dev->tx_rx = tx_rx;
	dev->delay_ms = delay_ms;

	ps2_to_idle_state(dev);
	ps2_configure(dev);
}

uint8_t ps2_configure(ps2_dualshock_dev *dev)
{
	/* Enter configuring mode */
	uint8_t ENTER_CONFIG_MODE_RX[ENTER_CONFIG_MODE_SIZE] = {0};
	if(dev->tx_rx(dev->spi_h, ENTER_CONFIG_MODE_TX, ENTER_CONFIG_MODE_RX,
				ENTER_CONFIG_MODE_SIZE, 20) != 0) {
		/* Something went wrong */
		ps2_to_idle_state(dev);
		return 1;
	}
	dev->delay_ms(8, NULL);

#ifdef TURN_ON_MOTORS
	/* Map motors for the main exchange command. 
	   4th byte to small motor (0x00), 5th byte to the big one (0x01). */
	uint8_t TURN_ON_MOTORS_RX[TURN_ON_MOTORS_SIZE] = {0};
	if(dev->tx_rx(dev->spi_h, TURN_ON_MOTORS_TX, TURN_ON_MOTORS_RX,
				TURN_ON_MOTORS_SIZE, 20) != 0) {
		/* Something went wrong */
		ps2_to_idle_state(dev);
		return 1;
	}
	dev->delay_ms(8, NULL);
#endif

#ifdef TURN_ON_ANALOG
	uint8_t ENABLE_ANALOG_MODE_RX[ENABLE_ANALOG_MODE_SIZE] = {0};
	if(dev->tx_rx(dev->spi_h, ENABLE_ANALOG_MODE_TX, ENABLE_ANALOG_MODE_RX,
				ENABLE_ANALOG_MODE_SIZE, 20) != 0) {
		/* Something went wrong */
		ps2_to_idle_state(dev);
		return 1;
	}
	dev->delay_ms(8, NULL);

#endif
	/* Exit configuring mode */
	uint8_t EXIT_CONFIG_MODE_RX[EXIT_CONFIG_MODE_SIZE] = {0};
	if(dev->tx_rx(dev->spi_h, EXIT_CONFIG_MODE_TX, EXIT_CONFIG_MODE_RX,
				EXIT_CONFIG_MODE_SIZE, 20) != 0) {
		/* Something went wrong */
		ps2_to_idle_state(dev);
		return 1;
	}
	dev->delay_ms(8, NULL);

	return 0;
}

/**
 * Small motor can be either powered on or off.
 * 0xFF on the byte the motor was mapped to turns it on.
 * All the other values turn it off.
 */
void ps2_small_motor(bool power)
{
	if(power) {
		MAIN_EXCHANGE_TX[3] = 0xFF;
	} else {
		MAIN_EXCHANGE_TX[3] = 0x00;
	}
}

/**
 * Large motor can vibrate weaker and stronger. 
 * For that, we supply the byte the motor was mapped to with a value 0x00 - 0xFF,
 * where 0x00 turns it off and 
 * 0xFF makes the strongest vibration.
 */
void ps2_large_motor(uint8_t power)
{
	MAIN_EXCHANGE_TX[4] = power;
}

/**
 * The default configuration exchange frequency is once per 50 normal exchanges.
 */
uint8_t ps2_main_exchange(ps2_dualshock_dev *dev)
{
	return ps2_main_exchange_with_config_freq(dev, 50);
}

/**
 * If analog mode, vibration motors or button pressures are enabled, 
 * we need to send configuration exchanges to the ps2 controller.
 * 
 * Here we decide to send such exchanges once per number of normal exchanges.
 * That is if `configuration_freq` is 50, we send 1 configuration exchange
 * after every 50 normal exchanges.
 */
uint8_t ps2_main_exchange_with_config_freq(ps2_dualshock_dev *dev, uint32_t configuration_freq)
{
#if defined(TURN_ON_MOTORS) || defined(TURN_ON_BUTTON_PRESSURE) || defined(TURN_ON_ANALOG)

	exchange_counter++;
	if(exchange_counter > configuration_freq) {
		if(ps2_configure(dev) != 0)
			return 1;
		exchange_counter = 0;
	}
#endif

	uint8_t MAIN_EXCHANGE_RX[MAIN_EXCHANGE_SIZE] = {0};
	if(dev->tx_rx(dev->spi_h, MAIN_EXCHANGE_TX, MAIN_EXCHANGE_RX,
				MAIN_EXCHANGE_SIZE, 20) != 0) {
		/* Something went wrong */
		ps2_to_idle_state(dev);
		return 1;
	}

	/* parse the response */
	dev->state = MAIN_EXCHANGE_RX[3] << 8 | MAIN_EXCHANGE_RX[4];

#ifdef TURN_ON_ANALOG
	/*
	 * Map analog sticks.
	 *
	 * If the joypad doesn't respond with 115 on the second byte, 
	 * its analog mode is turned off, physically, by button.
	 *
	 * Bytes EXCH[5-8] (128 - neutral):
	 * 5 - Right horizontal stick (0 - left, 255 - right);
	 * 6 - Right vertical stick (0 - up, 255 - down);
	 * 7 - Left horizontal stick (0 - left, 255 - right);
	 * 8 - Left vertical stick (0 - up, 255 - down); 
	 */
	if(MAIN_EXCHANGE_RX[1] == 115) {
		dev->sticks_state.r_horizontal = MAIN_EXCHANGE_RX[5];
		dev->sticks_state.r_vertical = MAIN_EXCHANGE_RX[6];
		dev->sticks_state.l_horizontal = MAIN_EXCHANGE_RX[7];
		dev->sticks_state.l_vertical = MAIN_EXCHANGE_RX[8];
	} else {
		dev->sticks_state.r_horizontal = 128;
		dev->sticks_state.r_vertical = 128;
		dev->sticks_state.l_horizontal = 128;
		dev->sticks_state.l_vertical = 128;
	}
#endif

	dev->delay_ms(20, NULL);

	return 0;
}

static void ps2_to_idle_state(ps2_dualshock_dev *dev)
{
	dev->state = 0xFFFF;
#if defined(TURN_ON_ANALOG)
	dev->sticks_state.r_horizontal = 128;
	dev->sticks_state.r_vertical = 128;
	dev->sticks_state.l_horizontal = 128;
	dev->sticks_state.l_vertical = 128;
#endif
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

uint8_t ps2_buttons_idle(ps2_dualshock_dev *dev)
{
	return !(dev->state ^ 0xFFFF);
}

uint8_t ps2_sticks_idle(ps2_dualshock_dev *dev)
{
	return dev->sticks_state.r_horizontal == 128 && dev->sticks_state.r_vertical == 128
		&& dev->sticks_state.l_horizontal == 128 && dev->sticks_state.l_vertical == 128;
}


uint8_t ps2_r_stick_up(ps2_dualshock_dev *dev)
{
	return (dev->sticks_state.r_vertical < 128)
		? (128 - dev->sticks_state.r_vertical) / 1.28f
		: 0;
}

uint8_t ps2_r_stick_down(ps2_dualshock_dev *dev)
{
	return (dev->sticks_state.r_vertical > 128)
		? (dev->sticks_state.r_vertical - 128) / 1.28f
		: 0;
}

uint8_t ps2_r_stick_left(ps2_dualshock_dev *dev)
{
	return (dev->sticks_state.r_horizontal < 128)
		? (128 - dev->sticks_state.r_horizontal) / 1.28f
		: 0;
}

uint8_t ps2_r_stick_right(ps2_dualshock_dev *dev)
{
	return (dev->sticks_state.r_horizontal > 128)
		? (dev->sticks_state.r_horizontal - 128) / 1.28f
		: 0;
}

uint8_t ps2_l_stick_up(ps2_dualshock_dev *dev)
{
	return (dev->sticks_state.l_vertical < 128)
		? (128 - dev->sticks_state.l_vertical) / 1.28f
		: 0;
}

uint8_t ps2_l_stick_down(ps2_dualshock_dev *dev)
{
	return (dev->sticks_state.l_vertical > 128)
		? (dev->sticks_state.l_vertical - 128) / 1.28f
		: 0;
}

uint8_t ps2_l_stick_left(ps2_dualshock_dev *dev)
{
	return (dev->sticks_state.l_horizontal < 128)
		? (128 - dev->sticks_state.l_horizontal) / 1.28f
		: 0;
}

uint8_t ps2_l_stick_right(ps2_dualshock_dev *dev)
{
	return (dev->sticks_state.l_horizontal > 128)
		? (dev->sticks_state.l_horizontal - 128) / 1.28f
		: 0;
}

