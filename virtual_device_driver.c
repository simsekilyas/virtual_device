// virtual_device_driver.c
// A Linux kernel module that creates a virtual character device (/dev/virtual_temp).
// This device reads a temperature value from a hardware driver and provides it to user-space applications.

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/device.h>

#define DEVICE_NAME "virtual_temp" // Name of the virtual device

static dev_t dev_number;         // Device number
static struct cdev c_dev;        // Character device structure
static struct class *cl;         // Device class

// Functions provided by the hardware driver to get and set the mock temperature
extern int get_mock_temperature(void);
extern void set_mock_temperature(int value);

// Prototypes for the read and write functions
ssize_t virtual_temp_read(struct file *f, char __user *buf, size_t len, loff_t *off);
ssize_t virtual_temp_write(struct file *f, const char __user *buf, size_t len, loff_t *off);

// Read function for /dev/virtual_temp
ssize_t virtual_temp_read(struct file *f, char __user *buf, size_t len, loff_t *off)
{
    char temp_str[16]; // Buffer to hold the temperature string
    int temp = get_mock_temperature(); // Get the mock temperature
    int temp_len = snprintf(temp_str, sizeof(temp_str), "Temp: %d\n", temp); // Format the temperature as a string

    // If the offset is beyond the string length, indicate end of file
    if (*off >= temp_len) {
        return 0;
    }

    // Copy the temperature string to user-space
    if (copy_to_user(buf, temp_str, temp_len)) {
        return -EFAULT; // Return error if copy fails
    }

    *off += temp_len; // Update the offset
    return temp_len;  // Return the number of bytes read
}

// Write function for /dev/virtual_temp
ssize_t virtual_temp_write(struct file *f, const char __user *buf, size_t len, loff_t *off)
{
    char temp_str[16];
    int temp;

    if (len > sizeof(temp_str) - 1) {
        return -EINVAL; // Return error if input is too long
    }

    // Copy data from user-space
    if (copy_from_user(temp_str, buf, len)) {
        return -EFAULT;
    }

    temp_str[len] = '\0'; // Null-terminate the string

    // Convert the string to an integer
    if (kstrtoint(temp_str, 10, &temp) < 0) {
        return -EINVAL;
    }

    // Update the mock temperature
    set_mock_temperature(temp);

    return len; // Return the number of bytes written
}

// File operations structure for the virtual device
static struct file_operations fops = {
    .owner = THIS_MODULE,        // Module that owns this file
    .read = virtual_temp_read,   // Read function
    .write = virtual_temp_write, // Write function
};

// Function called when the module is loaded
static int __init virtual_temp_init(void)
{
    int ret;

    // Allocate a major and minor number for the device
    if ((ret = alloc_chrdev_region(&dev_number, 0, 1, DEVICE_NAME)) < 0) {
        pr_err("Failed to allocate device number\n");
        return ret;
    }

    // Create a device class
    if ((cl = class_create(DEVICE_NAME)) == NULL) {
        unregister_chrdev_region(dev_number, 1);
        pr_err("Failed to create class\n");
        return -1;
    }

    // Create the device file in /dev
    if (device_create(cl, NULL, dev_number, NULL, DEVICE_NAME) == NULL) {
        class_destroy(cl);
        unregister_chrdev_region(dev_number, 1);
        pr_err("Failed to create device\n");
        return -1;
    }

    // Initialize and add the character device to the system
    cdev_init(&c_dev, &fops);
    if ((ret = cdev_add(&c_dev, dev_number, 1)) < 0) {
        device_destroy(cl, dev_number);
        class_destroy(cl);
        unregister_chrdev_region(dev_number, 1);
        pr_err("Failed to add cdev\n");
        return ret;
    }

    pr_info("Virtual device driver initialized\n");
    return 0;
}

// Function called when the module is removed
static void __exit virtual_temp_exit(void)
{
    cdev_del(&c_dev);                           // Remove the character device
    device_destroy(cl, dev_number);             // Destroy the device
    class_destroy(cl);                          // Destroy the device class
    unregister_chrdev_region(dev_number, 1);    // Unregister the device number
    pr_info("Virtual device driver exited\n");
}

module_init(virtual_temp_init); // Register the initialization function
module_exit(virtual_temp_exit); // Register the cleanup function

MODULE_LICENSE("GPL");           // Specify the license type
MODULE_AUTHOR("SIMSEK");         // Specify the module's author
MODULE_DESCRIPTION("Virtual device driver that reads and writes temperature to a hardware driver"); // Provide a short description of the module
