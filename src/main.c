#include <stdlib.h>
#include <stdio.h>

int main() {
    char buffer[128];
    FILE *pipe;

    // Open a pipe to execute the command and read its output
    pipe = popen("mosquitto_sub -t topic1", "r");
    if (!pipe) {
        perror("popen failed");
        return 1;
    }

    // Read the output line by line
    while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
        // Append the output to the result
        printf("Topic1: %s\n", buffer);
    }

    // Close the pipe
    if (pclose(pipe) == -1) {
        perror("pclose failed");
        return 1;
    }

    // Print the captured output
    /* printf("Command output: %s", buff); */
    printf("closed !!");

    return 0;
}
