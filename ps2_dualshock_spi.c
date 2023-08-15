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

ps2_state state;

static void ps2_to_idle_state(ps2_dualshock_dev *dev);
static void ps2_parse_response(uint8_t *resp, ps2_dualshock_dev *dev);


/* Function implementation */
void ps2_init(ps2_dualshock_dev *dev, void* spi_h)
{
	dev->spi_h = spi_h;
	dev->tx_rx = tx_rx;
	dev->delay_ms = delay_ms;
	dev->is_analog_on = 1;
	dev->is_button_pressure_on = 1;
	dev->is_motor_on = 1;

	dev->state = &state;

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
	ps2_parse_response(MAIN_EXCHANGE_RX, dev);

	dev->delay_ms(10, NULL);

	return 0;
}

static void ps2_to_idle_state(ps2_dualshock_dev *dev)
{
	dev->state->is_idle = 1;
	dev->state->up = 0;
	dev->state->down = 0;
	dev->state->left = 0;
	dev->state->right = 0;
	dev->state->start = 0;
	dev->state->select = 0;
	dev->state->cross = 0;
	dev->state->circle = 0;
	dev->state->square = 0;
	dev->state->triangle = 0;
	dev->state->l_trigger = 0;
	dev->state->r_trigger = 0;
	dev->state->l_shift = 0;
	dev->state->r_shift = 0;
	dev->state->l_stick_press = 0;
	dev->state->r_stick_press = 0;
}

static void ps2_parse_response(uint8_t *resp, ps2_dualshock_dev *dev)
{
	if(resp[3] == 0xFF && resp[4] == 0xFF) {
		ps2_to_idle_state(dev);
		return;
	}

	dev->state->is_idle = 0;
	dev->state->up = !(resp[3] & 0x10);
	dev->state->down = !(resp[3] & 0x40);
	dev->state->left = !(resp[3] & 0x80);
	dev->state->right = !(resp[3] & 0x20);
	dev->state->start = !(resp[3] & 0x8);
	dev->state->select = !(resp[3] & 0x1);
	dev->state->cross = !(resp[4] & 0x40);
	dev->state->circle = !(resp[4] & 0x20);
	dev->state->square = !(resp[4] & 0x80);
	dev->state->triangle = !(resp[4] & 0x10);
	dev->state->l_trigger = !(resp[4] & 0x1);
	dev->state->r_trigger = !(resp[4] & 0x2);
	dev->state->l_shift = !(resp[4] & 0x4);
	dev->state->r_shift = !(resp[4] & 0x8);
	dev->state->l_stick_press = !(resp[3] & 0x2);
	dev->state->r_stick_press = !(resp[3] & 0x4);

}

