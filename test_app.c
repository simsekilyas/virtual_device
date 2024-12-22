#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define DEVICE_PATH "/dev/virtual_temp"

int main(int argc, char *argv[]) {
    int fd;
    char buffer[16];
    ssize_t bytes;

    // Check if the correct number of arguments is provided
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <new_temperature_value>\n", argv[0]);
        return 1;
    }

    const char *new_temp = argv[1];

    // Validate if the provided temperature is a valid number
    for (size_t i = 0; i < strlen(new_temp); ++i) {
        if (!isdigit(new_temp[i])) {
            fprintf(stderr, "Error: Please provide a valid numeric temperature value.\n");
            return 1;
        }
    }

    // 1. Open the device file
    fd = open(DEVICE_PATH, O_RDWR);
    if (fd < 0) {
        perror("Failed to open the device file");
        return 1;
    }

    // 2. Write the new temperature value
    bytes = write(fd, new_temp, strlen(new_temp));
    if (bytes < 0) {
        perror("Failed to write the temperature");
        close(fd);
        return 1;
    }
    printf("New temperature value written: %s\n", new_temp);

    // 3. Verify the written temperature value by reading it back
    lseek(fd, 0, SEEK_SET); // Reset the file offset to the beginning
    bytes = read(fd, buffer, sizeof(buffer) - 1);
    if (bytes < 0) {
        perror("Failed to read the temperature");
        close(fd);
        return 1;
    }

    buffer[bytes] = '\0'; // Null-terminate the string
    printf("Temperature read: %s\n", buffer);

    // 4. Check if the written and read values match
    if (strstr(buffer, new_temp)) {
        printf("Verification successful: Written and read values match.\n");
    } else {
        printf("Verification failed: Written and read values do not match.\n");
    }

    // 5. Close the device file
    close(fd);
    return 0;
}
