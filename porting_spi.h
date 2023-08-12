void delay_ms(uint32_t period, void *spi_h);

int8_t tx_rx(void *ps2_dualshock_h, uint8_t *tx_buf, uint8_t *rx_buf,
	    uint32_t transfer_size, const void *spi_h);
