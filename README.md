# Virtual Temperature Driver Project

## Project Overview
The Virtual Temperature Driver project consists of two kernel modules:

1. **sysfs_temperature_driver**: This driver exposes a mock temperature value via the sysfs interface, creating a virtual file at `/sys/kernel/virtual_temp/temperature`.
2. **virtual_device_driver**: This driver creates a character device at `/dev/virtual_temp` to read the mock temperature value provided by the sysfs driver.

The project demonstrates kernel module development and inter-driver communication in the Linux kernel.

---

## Development Environment and Dependencies

### Development Environment
- **Operating System**: Linux
- **Kernel Version**: Ensure the kernel source for the currently running kernel is installed (check `/lib/modules/$(uname -r)/build`).
- **Compiler**: GCC
- **Build System**: `make`

### Dependencies
- Linux kernel headers must be installed.
- Root privileges are required for loading/unloading kernel modules.

---

## Build Instructions

1. Ensure the kernel headers for your running kernel are installed.
   ```bash
   sudo apt install linux-headers-$(uname -r)
   ```

2. Place the provided source files (`sysfs_temperature_driver.c`, `virtual_device_driver.c` and `test_app.c`) and the `Makefile` in the same directory.

3. Run the following command to build the modules:
   ```bash
   make
   ```

4. After building, two `.ko` files and one executable application will be created:
   - `sysfs_temperature_driver.ko`
   - `virtual_device_driver.ko`
   - `test_app`

---

## Load Modules

1. Load the `sysfs_temperature_driver` module first:
   ```bash
   sudo insmod sysfs_temperature_driver.ko
   ```
   Check for the creation of the sysfs entry:
   ```bash
   cat /sys/kernel/virtual_temp/temperature
   ```

2. Load the `virtual_device_driver` module:
   ```bash
   sudo insmod virtual_device_driver.ko
   ```
   Verify the creation of the device file:
   ```bash
   ls /dev/virtual_temp
   ```

---

## Usage

1. **Read Temperature via sysfs**
   ```bash
   cat /sys/kernel/virtual_temp/temperature
   ```
   Example output:
   ```
   42
   ```

2. **Read Temperature via Character Device**
   ```bash
   cat /dev/virtual_temp
   ```
   Example output:
   ```
   Temp: 42
   ```

---

## Unload Modules

1. Unload the `virtual_device_driver` module:
   ```bash
   sudo rmmod virtual_device_driver
   ```

2. Unload the `sysfs_temperature_driver` module:
   ```bash
   sudo rmmod sysfs_temperature_driver
   ```

---

## Test Application
### Run the application:
   ```bash
   sudo ./test_app <new_temp_value>
   ```

### Expected Output

If the driver is functioning correctly, you should see:
```
New temperature written: 75
Temperature read: Temp: 75
Verification successful: Written and read values match.
```

## Notes
- Ensure that the `sysfs_temperature_driver` module is loaded before the `virtual_device_driver` module since the latter depends on the exported function `get_mock_temperature`.
- Use `dmesg` to debug and check kernel logs:
   ```bash
   dmesg | tail
   ```

- If you encounter a `Permission denied error` when accessing `/sys/kernel/virtual_temp/temperature` or `/dev/virtual_temp`, it is likely due to insufficient file access permissions. You can resolve this by updating the file permissions:

   1.For sysfs file `/sys/kernel/virtual_temp/temperature`:
   ```bash
   sudo chmod 644 /sys/kernel/virtual_temp/temperature
   ```
   2.For the character device `/dev/virtual_temp`:
   ```bash
   sudo chmod 666 /dev/virtual_temp
   ```

