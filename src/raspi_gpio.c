#include "raspi_gpio.h"
#include "logger.h"


#include <linux/gpio.h>
#include <linux/kernel.h>

#define GPIO_PORT_COUNT 28 // 0 -- 27
#define GPIO_PORT_OFFSETT 512 // see "cat /sys/kernel/debug/gpio"

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
        // logger_info("GPIO-BANK-INIT", label);
        GPIO_PORTS.bank[i] = (gpio_port) {
            .gpio_nr = GPIO_PORT_OFFSETT + i,
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
    if ((retval = gpio_request(port.gpio_nr, port.label)))
    {
        char msg[50];
        snprintf(msg, sizeof(msg), "CANNOT REQUEST GPIO PORT %d, %d", port.gpio_nr, retval);
        logger_info("GPIO", msg);
        return -1;
    }

    switch (direction)
    {
        case OUTPUT:
            retval = gpio_direction_output(port.gpio_nr, 0);
            break;
        case INPUT:
            retval = gpio_direction_input(port.gpio_nr);
            break;
        default:
            break;
    }

    if (retval)
    {
        logger_info("GPIO", "CANNOT SET DIRECTION");
        return -1;
    }
    return gpio_test(gpio_nr);
}

void gpio_unload(unsigned int gpio_nr)
{
    gpio_port port = GPIO_PORTS.bank[gpio_nr];
    
    gpio_set_value(port.gpio_nr, 0);
    gpio_free(port.gpio_nr);
}

void gpio_set_level(unsigned int gpio_nr, GPIO_LEVEL level)
{
    gpio_port port = GPIO_PORTS.bank[gpio_nr];
    
    gpio_set_value(port.gpio_nr, (int)level);
}

void gpio_set_high(unsigned int gpio_nr)
{
    gpio_set_level(gpio_nr, GPIO_HIGH);
    
}

void gpio_set_low(unsigned int gpio_nr)
{
    gpio_set_level(gpio_nr, GPIO_LOW);
}

int gpio_test(unsigned int gpio_nr)
{
    
    gpio_set_high(gpio_nr);
    gpio_set_low(gpio_nr);
    return 0;
}