#include "raspi_rs485.h"
#include "raspi_gpio.h"
#include "logger.h"

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>
#include <linux/mod_devicetable.h>
// #include <linux/proberty.h>
#include <linux/platform_device.h>
#include <linux/of_device.h>



#define DRIVER_NAME "RASPI_RS485"
#define DRIVER_CLASS "DRIVER_CLASS"


static char buffer[255];
static size_t buffer_pointer;

struct raspi_rs485 CHANNEL_A = {
    .TX_PIN = 14,
    .RX_PIN = 15,
    .DE_PIN = 27,
    .RE_N_PIN = 22,
    .device_name = "RASPI_RS485_CHANNEL_A",
    .class_name = "RASPI_RS485_CLASS_A",
    .device_number = 0,
    .driver_class = NULL,
    .device = {}
};

struct raspi_rs485 CHANNEL_B = {
    .TX_PIN = 0, // 0 5
    .RX_PIN = 1, // 1 26
    .DE_PIN = 5, // 5 16
    .RE_N_PIN = 6,
    .device_name = "RASPI_RS485_CHANNEL_B",
    .class_name = "RASPI_RS485_CLASS_B",
    .device_number = 0,
    .driver_class = NULL,
    .device = {}
};

static int dt_probe(struct platform_device *pdev);
static int dt_remove(struct platform_device *pdev);

static struct of_device_id my_driver_ids[] = {
	{
		.compatible = "brightlight,RASPI_RS485",
	}, { /* sentinel */ }
};

static struct platform_driver my_driver = {
	.probe = dt_probe,
	.remove = dt_remove,
	.driver = {
		.name = "RASPI_RS485",
		.of_match_table = my_driver_ids,
	},
};

static int dt_probe(struct platform_device *pdev)
{
    return 0;
}

static int dt_remove(struct platform_device *pdev)
{
    return 0;
}


struct raspi_rs485* get_device_info(struct cdev* dev_cdev)
{
    struct raspi_rs485 *device = container_of(dev_cdev, struct raspi_rs485, device);
    if (device == &CHANNEL_A) return &CHANNEL_A;
    else if (device == &CHANNEL_B) return &CHANNEL_B;
    return NULL;
}

/**
 * @brief Read callback when something is read from the file
 *
 * @param File
 * @param user_buffer
 * @param count
 * @param off
 * @return ssize_t
 */
static ssize_t driver_read(struct file *File, char *user_buffer, size_t count, loff_t *off)
{
    int to_copy, not_copied, delta;

    struct raspi_rs485 *device_info = get_device_info(File->f_inode->i_cdev);
    if (device_info == NULL) return -1;
    gpio_set_low(device_info->TX_PIN);


    to_copy = min(count, buffer_pointer);
    not_copied = copy_to_user(user_buffer, buffer, to_copy);
    delta = to_copy - not_copied;

    return delta;
}


/**
 * @brief Write call when something is written to the file
 *
 * @param File
 * @param user_buffer
 * @param count
 * @param offs
 * @return ssize_t
 */
static ssize_t driver_write(struct file *File, const char *user_buffer, size_t count, loff_t *offs) {
	int to_copy, not_copied, delta;
    struct raspi_rs485 *device_info = get_device_info(File->f_inode->i_cdev);
    if (device_info == NULL) return -1;
    gpio_set_high(device_info->TX_PIN);

	/* Get amount of data to copy */
	to_copy = min(count, sizeof(buffer));


    // gpio_set_high(gpio_nr);
	/* Copy data to user */
	not_copied = copy_from_user(buffer, user_buffer, to_copy);
	buffer_pointer = to_copy;

	/* Calculate data */
	delta = to_copy - not_copied;

	return delta;
}


/**
 * @brief Is called when device file is opened
 *
 * @param device_file
 * @param instance
 * @return int
 */
static int dev_open(struct inode *device_file, struct file *instance)
{
    printk("Dev file open was called\n");
    return 0;
}

static int dev_close(struct inode *device_file, struct file *instance)
{
    printk("Dev file close was called\n");
    return 0;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open  = dev_open,
    .release = dev_close,
    .read = driver_read,
    .write = driver_write
};

int setup_gpio(struct raspi_rs485* device_info)
{
    int retval;
    retval = gpio_load(device_info->TX_PIN, OUTPUT);
    retval = gpio_load(device_info->RX_PIN, INPUT);
    retval = gpio_load(device_info->DE_PIN, OUTPUT);
    retval = gpio_load(device_info->RE_N_PIN, OUTPUT);
    return retval;
}

void remove_gpio(struct raspi_rs485* device_info)
{
    gpio_unload(device_info->TX_PIN);
    gpio_unload(device_info->RX_PIN);
    gpio_unload(device_info->DE_PIN);
    gpio_unload(device_info->RE_N_PIN);
}

static int create_device_file(struct raspi_rs485* device_info)
{
    // register dev file
    //Allocate Device Number
    setup_gpio(device_info);
    if (alloc_chrdev_region(&(device_info->device_number), 0, 1, device_info->device_name) < 0)
    {
        logger_critical("DEVICE_REGISTER", "ALLOC CHRDEV WENT WRONG");
        return -1;
    }
    // printk("ALLOC WAS SUCCESSFUL MAJOR: %d, MINOR: %d", device_info->device_number >> 20, my_device_nr & 0xFFFFF);

    // Create device class
    if ((device_info->driver_class = class_create(THIS_MODULE, device_info->class_name )) == NULL)
    {
        logger_critical("DEVICE_REGISTER", "CLASS CREATE WENT WRONG");
        goto ClassError;
    }

    // Create Device File
    if (device_create(device_info->driver_class, NULL, device_info->device_number, NULL, device_info->device_name) == NULL)
    {
        logger_critical("DEVICE_REGISTER", "DEVICE CREATE WENT WRONG");
        goto FileError;

    }

    cdev_init(&(device_info->device), &fops);
    if(cdev_add(&(device_info->device), device_info->device_number, 1) < 0)
    {
        logger_critical("DEVICE_REGISTER", "REGISTER OF DEVICE TO KERNEL FAILED");
        goto AddError;
    }

    return 0;

    AddError:
        device_destroy(device_info->driver_class, device_info->device_number);
    FileError:
        class_destroy(device_info->driver_class);

    ClassError:
        unregister_chrdev(device_info->device_number, device_info->device_name);
        return -1;
}

static void remove_device_file(struct raspi_rs485* device_info)
{
    remove_gpio(device_info);
    cdev_del(&(device_info->device));
    device_destroy(device_info->driver_class, device_info->device_number);
    class_destroy(device_info->driver_class);
    unregister_chrdev(device_info->device_number, device_info->device_name);
}

/**
 * @brief Init function of the driver
 *
 * @return int
 */
static int __init gpio_driver_init(void)
{
    int retval = 0;
    logger_info("Main", "Entering Driver");

    retval = create_device_file(&CHANNEL_A);
    logger_info("Main", "Channel A Created");

    retval = create_device_file(&CHANNEL_B);
    logger_info("Main", "Channel B Created");

    return retval;
}

/**
 * @brief Exit function of the driver
 *
 */
static void __exit gpio_driver_exit(void)
{
    remove_device_file(&CHANNEL_A);
    remove_device_file(&CHANNEL_B);
    logger_info("Main", "Leaving Driver");
}

MODULE_DEVICE_TABLE(of, my_driver_ids);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("FS");
MODULE_DESCRIPTION("Serial RS485 Driver over GPIO");
MODULE_VERSION("1.0");

module_init(gpio_driver_init);
module_exit(gpio_driver_exit);
