#ifndef __ESP_USART_H
#define __ESP_USART_H


#include <stdint.h>


typedef void(*esp_usart_receive_callback_t)(uint8_t data);


void esp_usart_init(void);
void esp_usart_write_data(uint8_t *data, uint16_t length);
void esp_usart_write_string(const char *str);
void esp_usart_receive_register(esp_usart_receive_callback_t callback);


#endif /* __ESP_USART_H */
