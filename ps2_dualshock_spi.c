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


/* Function implementation */
void ps2_init(ps2_dualshock_dev *dev, void* spi_h)
{
	dev->spi_h = spi_h;
	dev->tx_rx = tx_rx;
	dev->delay_ms = delay_ms;
	dev->is_analog_on = 1;
	dev->is_button_pressure_on = 1;
	dev->is_motor_on = 1;
}

uint8_t ps2_main_exchange(ps2_dualshock_dev *dev)
{
	uint8_t MAIN_EXCHANGE_RX[MAIN_EXCHANGE_SIZE] = {0};
	dev->tx_rx(dev->spi_h, MAIN_EXCHANGE_TX, MAIN_EXCHANGE_RX,
		   MAIN_EXCHANGE_SIZE, 20);

	//parse the response

	dev->delay_ms(10, NULL);

	return 0;
}

