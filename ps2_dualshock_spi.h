#include "ps2_dualshock_spi_defs.h"

#ifndef PS2_DEFINITIONS
#define PS2_DEFINITIONS

/* Function type definitions */
typedef void (*tx_rx_func_ptr) (const void *spi_h, uint8_t *tx_buf,
	uint8_t *rx_buf, uint32_t transfer_size, uint32_t timeout);
typedef void (*delay_ms_func_ptr) (uint32_t period, void *any_ptr);

/* Structures */
typedef struct {
	uint8_t is_analog_on;
	uint8_t is_button_pressure_on;
	uint8_t is_motor_on;
	void *spi_h;
	tx_rx_func_ptr tx_rx;
	delay_ms_func_ptr delay_ms;
} ps2_dualshock_dev;

/* Functions */
void ps2_init(ps2_dualshock_dev *dev, void* spi_h);
uint8_t ps2_main_exchange(ps2_dualshock_dev *dev);

#endif

