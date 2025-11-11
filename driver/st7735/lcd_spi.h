#ifndef __SPI_H__
#define __SPI_H__


#include <stdbool.h>
#include <stdint.h>


typedef void (*lcd_spi_send_finish_callback_t)(void);


void lcd_spi_init(void);
void lcd_spi_write(uint8_t *data, uint16_t length);
void lcd_spi_write_async(uint8_t *data, uint16_t length);
void lcd_spi_send_finish_register(lcd_spi_send_finish_callback_t callback);


#endif /* __SPI_H__ */
