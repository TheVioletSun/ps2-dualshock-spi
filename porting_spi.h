#include <stdint.h>

void delay_ms(uint32_t period, void *any_ptr);

void tx_rx(const void *spi_h, uint8_t *tx_buf, uint8_t *rx_buf,
	    uint32_t transfer_size, uint32_t timeout);
