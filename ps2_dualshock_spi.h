#include <stdbool.h>
#include "ps2_dualshock_definitions.h"

#ifndef PS2_DEFINITIONS
#define PS2_DEFINITIONS

/* Function type definitions */
typedef uint8_t (*tx_rx_func_ptr) (const void *spi_h, uint8_t *tx_buf,
	uint8_t *rx_buf, uint32_t transfer_size, uint32_t timeout);
typedef void (*delay_ms_func_ptr) (uint32_t period, void *any_ptr);

/* Structures */
typedef struct {
	uint16_t state;
	void *spi_h;
	tx_rx_func_ptr tx_rx;
	delay_ms_func_ptr delay_ms;
} ps2_dualshock_dev;

/* Functions */
void ps2_init(ps2_dualshock_dev *dev, void* spi_h);
void ps2_configure(ps2_dualshock_dev *dev);
void ps2_small_motor(bool power);
void ps2_large_motor(uint8_t power);
uint8_t ps2_main_exchange(ps2_dualshock_dev *dev);
uint8_t ps2_main_exchange_with_config_freq(ps2_dualshock_dev *dev, uint32_t configuration_exchange_freq);
uint8_t ps2_any_button_pressed(ps2_dualshock_dev *dev, uint16_t buttons);
uint8_t ps2_all_buttons_pressed(ps2_dualshock_dev *dev, uint16_t buttons);
uint8_t ps2_is_idle(ps2_dualshock_dev *dev);

#endif

