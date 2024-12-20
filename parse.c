#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h> // For close()
#include "parse.h"

#define PIPE_NAME "cv_pipe" // Define the name of the pipe

void print_parsed_values(const CV_Parsed_Parameters *st);
int word_finder(FILE *fd, const char *match_word);
void print_section(FILE *fd, char stop_chara);
void parse_values(FILE *fd, CV_Parsed_Parameters *st);

int main() {
    // Create the named pipe (FIFO)
    if (mkfifo(PIPE_NAME, 0666) == -1 && errno != EEXIST) {
        perror("mkfifo");
        return 1;
    }

    // Open the named pipe for reading
    FILE *fd;
    while (1) {
        fd = fopen(PIPE_NAME, "r");
        if (fd == NULL) {
            perror("Opening FIFO");
            return -1;
        }

        CV_Parsed_Parameters person;

        // Continuously read from the pipe
        while (1) {
            parse_values(fd, &person);
            print_parsed_values(&person);
            fflush(stdout); // Ensure output is printed immediately
        }

        fclose(fd); // Close the FIFO when done (this will not happen in this loop)
    }

    return 0;
}

void print_parsed_values(const CV_Parsed_Parameters *st) {
    char buffer[128];
    puts("------------------------------------");
    sprintf(buffer, "x_value: %d\t\t", st->x_value);
    sprintf(buffer + strlen(buffer), "y_value: %d\t\t", st->y_value);
    sprintf(buffer + strlen(buffer), "width: %d\t\t", st->width);
    sprintf(buffer + strlen(buffer), "height: %d\t\t", st->height);
    sprintf(buffer + strlen(buffer), "label: %s\n", st->label);
    printf("%s", buffer);
    puts("------------------------------------");
}

int word_finder(FILE *fd, const char *match_word) {
    int word_match = 0;
    char c;
    int word_len = strlen(match_word);
    while ((c = getc(fd)) != EOF) {
        for (int i = 0; i < word_len; ++i) {
            if (match_word[i] != c) break;
            if (i == word_len - 1) {
                printf("\033[032m Match Found: %s\n\033[0m", match_word);
                return 1; // Match found
            }
            c = getc(fd);
        }
    }
    printf("\033[31m !! No Match Found: %s\n\033[0m", match_word);
    return 0;
}

void print_section(FILE *fd, char stop_chara) {
    char c;
    while ((c = getc(fd)) != EOF) {
        if (c == stop_chara) return;
        putc(c, stdout);
    }
}

void parse_values(FILE *fd, CV_Parsed_Parameters *st) {
    if (word_finder(fd, "DEBUG: Prediction tree:"))
        if (word_finder(fd, "predictions : "))
            if (word_finder(fd, "bbox : ")) {
                word_finder(fd, "x : ");
                fscanf(fd, "%d", &st->x_value);
                word_finder(fd, "y : ");
                fscanf(fd, "%d", &st->y_value);
                word_finder(fd, "width : ");
                fscanf(fd, "%d", &st->width);
                word_finder(fd, "height : ");
                fscanf(fd, "%d", &st->height);
                if (word_finder(fd, "classes : "))
                    if (word_finder(fd, "Label : ")) {
                        fscanf(fd, "%s", st->label);
                    }
            }
}
