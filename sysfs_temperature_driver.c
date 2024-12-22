// sysfs_temperature_driver.c
// A Linux kernel module to simulate a mock temperature driver.
// The driver exposes a virtual temperature value via sysfs at /sys/kernel/virtual_temp/temperature.

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/device.h>
#include <linux/sysfs.h>
#include <linux/kobject.h>

#define DEVICE_NAME "virtual_temp" // Name of the sysfs directory

static struct kobject *kobj_ref;  // Reference to the kobject
static int mock_temperature = 42; // Initial mock temperature value

int get_mock_temperature(void);  // Prototype for the exported function
void set_mock_temperature(int value); // Prototype for setting the temperature

// Function to retrieve the mock temperature value
int get_mock_temperature(void)
{
    return mock_temperature;
}
EXPORT_SYMBOL(get_mock_temperature); // Export the function to make it available to other kernel modules

// Function to update the mock temperature value
void set_mock_temperature(int value)
{
    mock_temperature = value;
}
EXPORT_SYMBOL(set_mock_temperature); // Export the function to make it available to other kernel modules

// Read function for /sys/kernel/virtual_temp/temperature
static ssize_t temperature_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%d\n", mock_temperature);
}

// Write function for /sys/kernel/virtual_temp/temperature
static ssize_t temperature_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
    int temp;
    if (kstrtoint(buf, 10, &temp) < 0) {
        return -EINVAL; // Return error if input is invalid
    }
    set_mock_temperature(temp);
    return count;
}

// Definition of the sysfs attribute for temperature
static struct kobj_attribute temperature_attribute = __ATTR(temperature, 0644, temperature_show, temperature_store);

// Module initialization function
static int __init virtual_temp_driver_init(void)
{
    int retval;

    pr_info("Initializing virtual_temp driver\n");

    // Create the kobject under /sys
    kobj_ref = kobject_create_and_add(DEVICE_NAME, kernel_kobj);
    if (!kobj_ref) {
        pr_err("Failed to create sysfs kobject\n");
        return -ENOMEM;
    }

    pr_info("Created kobject: /sys/%s\n", DEVICE_NAME);

    // Add the temperature attribute to the kobject
    retval = sysfs_create_file(kobj_ref, &temperature_attribute.attr);
    if (retval) {
        pr_err("Failed to create sysfs file\n");
        kobject_put(kobj_ref); // Cleanup if the file creation fails
        return retval;
    }

    pr_info("Driver initialized: /sys/%s/temperature\n", DEVICE_NAME);
    return 0;
}

// Module cleanup function
static void __exit virtual_temp_driver_exit(void)
{
    pr_info("Removing driver and cleaning up\n");

    // Remove the sysfs file and kobject
    sysfs_remove_file(kobj_ref, &temperature_attribute.attr);
    kobject_put(kobj_ref);

    pr_info("Driver exited\n");
}

module_init(virtual_temp_driver_init); // Register the initialization function
module_exit(virtual_temp_driver_exit); // Register the cleanup function

MODULE_LICENSE("GPL");           // Specify the license type
MODULE_AUTHOR("SIMSEK");         // Specify the module's author
MODULE_DESCRIPTION("Mock temperature driver exposing data in /sys/virtual_temp/temperature"); // Provide a short description of the module
