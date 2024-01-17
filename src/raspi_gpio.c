#include "raspi_gpio.h"
#include "logger.h"

#include <linux/gpio.h>

#define GPIO_PORT_COUNT 28 // 0 -- 27

typedef struct gpio_port {
    unsigned int gpio_nr;
    char* label;
} gpio_port;

typedef struct {
    struct gpio_port bank[GPIO_PORT_COUNT];
} GPIO_BANK;

GPIO_BANK GPIO_PORTS;

void gpio_bank_init(void)
{
    static int was_created = 0;
    if (was_created) return;

    was_created = 1;
    for (unsigned int i = 0; i < GPIO_PORT_COUNT; i++)
    {
        char label[20];
        sprintf(label, "rpi-gpio-%d", i);
        GPIO_PORTS.bank[i] = (gpio_port) {
            .gpio_nr = i,
            .label = label
        };
    }
}

int gpio_load(unsigned int gpio_nr, GPIO_DIRECTION direction)
{
    int retval = 0;
    gpio_port port;

    gpio_bank_init();

    port = GPIO_PORTS.bank[gpio_nr];
    if (gpio_request(port.gpio_nr, port.label))
    {
        char tmp[50];
        sprintf(tmp, "CANNOT REQUEST GPIO PORT %d", port.gpio_nr);
        logger_info("GPIO", tmp);
        return -1;
    }

    switch (direction)
    {
        case OUTPUT:
            retval = gpio_direction_output(gpio_nr, 0);
            break;
        case INPUT:
            retval = gpio_direction_input(gpio_nr);
            break;
        default:
            break;
    }

    if (retval)
    {
        logger_info("GPIO", "CANNOT SET DIRECTION");
        return -1;
    }
    return 0;
}

void gpio_unload(unsigned int gpio_nr)
{
    gpio_set_value(gpio_nr, 0);
    gpio_free(gpio_nr);
}

void gpio_set_level(unsigned int gpio_nr, GPIO_LEVEL level)
{
    gpio_set_value(gpio_nr, (int)level);
}

void gpio_set_high(unsigned int gpio_nr)
{
    gpio_set_level(gpio_nr, GPIO_HIGH);
}

void gpio_set_low(unsigned int gpio_nr)
{
    gpio_set_level(gpio_nr, GPIO_LOW);
}
