# Targets for kernel modules
obj-m += sysfs_temperature_driver.o
obj-m += virtual_device_driver.o

# Target for the user-space application
USER_APP := test_app

# Build rules
all: kernel user

# Build kernel modules
kernel:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

# Build the user-space application
user:
	gcc -o $(USER_APP) test_app.c

# Clean rules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm -f $(USER_APP)
