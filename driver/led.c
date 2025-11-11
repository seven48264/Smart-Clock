#include "stm32f10x.h"                  // Device header
#include <stdbool.h>
#include "led.h"

#define LED_PORT    GPIOC
#define LED_PIN     GPIO_Pin_13



static bool led_state = false;


void led_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = LED_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(LED_PORT, &GPIO_InitStructure);
    GPIO_SetBits(LED_PORT, LED_PIN);
}

void led_set(bool on)
{
    led_state = on;
    GPIO_WriteBit(LED_PORT, LED_PIN, on ? Bit_RESET : Bit_SET);
}

void led_on(void)
{
    led_set(true);
}

void led_off(void)
{
    led_set(false);
}

void led_toggle(void)
{
    led_set(!led_state);
}
