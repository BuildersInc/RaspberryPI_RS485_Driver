#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>



#define MAYOR_DEVICE_NUMBER 90

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
    .release = dev_close
};



/**
 * @brief Init function of the driver
 *
 * @return int
 */
static int __init gpio_driver_init(void)
{
    int retval;
    printk("Welcome to the Driver\n");

    // register dev file
    retval = register_chrdev(MAYOR_DEVICE_NUMBER, "my_dev_nr", &fops);
    if (retval == 0)
    {
        printk("Device number registered Major: %d ,Minor: %d", MAYOR_DEVICE_NUMBER, 0);
    }
    else if (retval > 0)
    {
        printk("Device number registered Major: %d ,Minor: %d", retval >> 20, retval & 0xfffff);
    }
    else
    {
        printk("Something went Wrong");
        return -1;
    }
    return 0;
}

/**
 * @brief Exit function of the driver
 *
 */
static void __exit gpio_driver_exit(void)
{
    unregister_chrdev(MAYOR_DEVICE_NUMBER, "my_dev_nr");
    printk("Leaving my driver\n");

}

module_init(gpio_driver_init);
module_exit(gpio_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("FS");
MODULE_DESCRIPTION("TEST DRIVER");
MODULE_VERSION("1.0");

