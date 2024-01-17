#pragma once


typedef enum GPIO_DIRECTION {INPUT, OUTPUT} GPIO_DIRECTION;
typedef enum GPIO_LEVEL {GPIO_LOW, GPIO_HIGH} GPIO_LEVEL;

int gpio_load(unsigned int gpio_nr, GPIO_DIRECTION direction);
void gpio_bank_init(void);
void gpio_set_level(unsigned int gpio_nr, GPIO_LEVEL level);
void gpio_set_high(unsigned int gpio_nr);
void gpio_set_low(unsigned int gpio_nr);
void gpio_unload(unsigned int gpio_nr);