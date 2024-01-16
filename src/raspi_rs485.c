#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>



#define MAYOR_DEVICE_NUMBER 90
#define DRIVER_NAME "RASPI_RS485"
#define DRIVER_CLASS "DRIVER_CLASS"

static dev_t my_device_nr;
static struct class *my_class;
static struct cdev mydevice;

static char buffer[255];
static size_t buffer_pointer;

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

	/* Get amount of data to copy */
	to_copy = min(count, sizeof(buffer));

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



/**
 * @brief Init function of the driver
 *
 * @return int
 */
static int __init gpio_driver_init(void)
{
    // int retval;
    printk("Welcome to the Driver\n");

    // register dev file
    //Allocate Device Number
    if (alloc_chrdev_region(&my_device_nr, 0, 1, DRIVER_NAME) < 0)
    {
        printk("ALLOC CHRDEV WENT WRONG\n");
        return -1;
    }
    printk("ALLOC WAS SUCCESSFUL MAJOR: %d, MINOR: %d", my_device_nr >> 20, my_device_nr & 0xFFFFF);

    // Create device class
    if ((my_class = class_create(THIS_MODULE, DRIVER_CLASS )) == NULL)
    {
        printk("CLASS CREATE WENT WRONG\n");
        goto ClassError;
    }

    // Create Device File
    if (device_create(my_class, NULL, my_device_nr, NULL, DRIVER_NAME) == NULL)
    {
        printk("DEVICE CREATE WENT WRONG\n");
        goto FileError;

    }
    cdev_init(&mydevice, &fops);

    if(cdev_add(&mydevice, my_device_nr, 1) == -1)
    {
        printk("REGISTER OF DEVICE TO KERNEL FAILED");
        goto AddError;
    }


    return 0;

    AddError:
        device_destroy(my_class, my_device_nr);
    FileError:
        class_destroy(my_class);

    ClassError:
        unregister_chrdev(my_device_nr, DRIVER_NAME);
        return -1;
}

/**
 * @brief Exit function of the driver
 *
 */
static void __exit gpio_driver_exit(void)
{
    cdev_del(&mydevice);
    device_destroy(my_class, my_device_nr);
    class_destroy(my_class);
    unregister_chrdev(my_device_nr, DRIVER_NAME);
    printk("Leaving my driver\n");

}

module_init(gpio_driver_init);
module_exit(gpio_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("FS");
MODULE_DESCRIPTION("TEST DRIVER");
MODULE_VERSION("1.0");

