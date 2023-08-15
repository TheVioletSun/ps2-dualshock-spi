#include <stdint.h>

void delay_ms(uint32_t period, void *any_ptr);

/* Returning 0 if everything goes well */
uint8_t tx_rx(const void *spi_h, uint8_t *tx_buf, uint8_t *rx_buf,
	    uint32_t transfer_size, uint32_t timeout);
