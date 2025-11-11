#ifndef __SWI2C_H__
#define __SWI2C_H__


#include <stdbool.h>
#include <stdint.h>


void swi2c_init(void);
bool swi2c_write(uint8_t addr, uint8_t reg, uint8_t *data, uint16_t length);
bool swi2c_read(uint8_t addr, uint8_t reg, uint8_t *data, uint16_t length);


#endif /* __SWI2C_H__ */
