#include <stdbool.h>
#include <stdint.h>
#include "stm32f10x.h"                  // Device header
#include "timer.h"


static timer_elapsed_callback_t timer_elapsed_callback;


void timer_init(uint32_t period_us)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Prescaler = 72 - 1;
    TIM_TimeBaseStructure.TIM_Period = period_us - 1;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void timer_start(void)
{
    TIM_Cmd(TIM2, ENABLE);
}

void timer_stop(void)
{
    TIM_Cmd(TIM2, DISABLE);
}

void timer_elapsed_register(timer_elapsed_callback_t callback)
{
    timer_elapsed_callback = callback;
}

void TIM2_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
        if (timer_elapsed_callback)
        {
            timer_elapsed_callback();
        }
    }
}
