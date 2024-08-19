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
	uint8_t r_horizontal;	
	uint8_t r_vertical;
	uint8_t l_horizontal;
	uint8_t l_vertical;
} analog_sticks_state;


typedef struct {
	uint16_t state;
	analog_sticks_state sticks_state;
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
uint8_t ps2_buttons_idle(ps2_dualshock_dev *dev);	/* 0 for false */
uint8_t ps2_sticks_idle(ps2_dualshock_dev *dev);	/* 0 for false */
uint8_t ps2_r_stick_up(ps2_dualshock_dev *dev);		/* From 0 to 100 */
uint8_t ps2_r_stick_down(ps2_dualshock_dev *dev);	/* From 0 to 100 */
uint8_t ps2_r_stick_left(ps2_dualshock_dev *dev);	/* From 0 to 100 */
uint8_t ps2_r_stick_right(ps2_dualshock_dev *dev);	/* From 0 to 100 */
uint8_t ps2_l_stick_up(ps2_dualshock_dev *dev);		/* From 0 to 100 */
uint8_t ps2_l_stick_down(ps2_dualshock_dev *dev);	/* From 0 to 100 */
uint8_t ps2_l_stick_left(ps2_dualshock_dev *dev);	/* From 0 to 100 */
uint8_t ps2_l_stick_right(ps2_dualshock_dev *dev);	/* From 0 to 100 */

#endif

