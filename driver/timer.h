#ifndef __TIMER_H__
#define __TIMER_H__


#include <stdint.h>
#include <stdbool.h>


typedef void (*timer_elapsed_callback_t)(void);


void timer_init(uint32_t period_us);
void timer_start(void);
void timer_stop(void);
void timer_elapsed_register(timer_elapsed_callback_t callback);


#endif /* __TIMER_H */
