#include <stdio.h>
#include <stdlib.h>

int main() {
    char buffer[128];
    char result[1024] = ""; // To store the full output
    FILE *pipe;

    // Open a pipe to execute the command and read its output
    pipe = popen("echo bash", "r");
    if (!pipe) {
        perror("popen failed");
        return 1;
    }

    // Read the output line by line
    while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
        // Append the output to the result
        strcat(result, buffer);
    }

    // Close the pipe
    if (pclose(pipe) == -1) {
        perror("pclose failed");
        return 1;
    }

    // Print the captured output
    printf("Command output: %s", result);

    return 0;
}
