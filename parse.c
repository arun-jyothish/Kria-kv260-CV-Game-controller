#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <fcntl.h>
#include <string.h>

/*
 *  Find exact word match and seek file descriptor to that point
 */
int word_finder(FILE *fd, const char *match_word);

/*
 *  Print current Seek point to next occurance of character provided
 */
void print_section(FILE *fd, char stop_chara);

/*
 *  Structure that saves parsed values from pipes
 */
typedef struct {
  int x_value;
  int y_value;
  int width;
  int height;
  char label[20];
} CV_Parsed_Parameters;

/*
 * Print parsed values, bounding box co-ordinates
 * Object class, its width and height
 */
void print_parsed_values(const CV_Parsed_Parameters *st);

/*
 * Parse Values from file descriptor
 * x, y ,width, height, class ..etc
 */
void parse_values(FILE *fd, CV_Parsed_Parameters *st);


/*
 * Main 
 */
void main() {
  /* FILE *fd = fopen("inference.txt", "r"); */
  FILE *fd = fopen("pipe1", "r");
  perror("Opening File");
  if (fd == NULL)
    return;

  /* print_section(fd, '}'); */
  /* return; */
  CV_Parsed_Parameters person;
loop1:
  parse_values(fd, &person);
  print_parsed_values(&person);
  /* goto loop1; */
}

void print_parsed_values(const CV_Parsed_Parameters *st) {
  puts("------------------------------------");
  printf("x_value: %d\t\t", st->x_value);
  printf("y_value: %d\t\t", st->y_value);
  printf("width: %d\t\t", st->width);
  printf("height: %d\t\t", st->height);
  printf("label: %s\n", st->label);
  puts("------------------------------------");
}

int word_finder(FILE *fd, const char *match_word) {

  int word_match = 0;
  char c;
  int word_len = strlen(match_word);
  /* while (read(fd, &c, 1) > 0) { */
  while ((c = getc(fd)) != EOF) {
    for (int i = 0; i < word_len; ++i) {
      if (match_word[i] != c)
        break;
      if (i == word_len - 1) {
        printf("\033[032m Match Found: %s\n\033[0m", match_word);
        /* provided word match found return true  */
        return 1;
      }
      c = getc(fd);
    }
  }
  printf("\033[31m !! No Match Found: %s\n\033[0m", match_word);
  return 0;
}

void print_section(FILE *fd, char stop_chara) {
  char c;
  while (c = getc(fd), c) {
    /* while (read(*fd, &c, 1) > 0) { */
    if (c == stop_chara)
      return;
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
