#include <stdint.h>
#include "stm32f10x.h"

void delay_us(uint32_t us)
{
    SysTick->LOAD = us * (SystemCoreClock / 1000000) - 1;
    SysTick->VAL = 0;
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;
    while ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) == 0);
    SysTick->CTRL = 0;
}

void delay_ms(uint32_t ms)
{
    while (ms --)
    {
        delay_us(1000);
    }
}
