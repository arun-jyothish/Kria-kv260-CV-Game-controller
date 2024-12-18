#include <errno.h>
#include <stdio.h>
#include <string.h>

int word_finder(FILE *fd, const char *match_word);
void print_section(FILE *fd, char stop_chara);
void parse_values(FILE *fd, const char *word);
void print_parsed_values(const CV_Parsed_Parameters *st);

typedef struct {
  int x_value;
  int y_value;
  int width;
  int height;
} CV_Parsed_Parameters;

void main() {
  FILE *fd = fopen("inference.txt", "r");
  perror("Opening File");
  if (fd == NULL)
    return;

  CV_Parsed_Parameters person;
  if (word_finder(fd, "DEBUG: Prediction tree:"))
    if (word_finder(fd, "predictions"))
      if (word_finder(fd, "bbox")) {

        word_finder(fd, "x : ");
        fscanf(fd, "%d", &person.x_value);
        word_finder(fd, "y : ");
        fscanf(fd, "%d", &person.y_value);

        /* print_section(fd, ']'); */
      }
  print_parsed_values(&person);
}

void print_parsed_values(const CV_Parsed_Parameters *st) {
  printf("x_value: %d\n", st->x_value);
  printf("y_value: %d\n", st->y_value);
}

int word_finder(FILE *fd, const char *match_word) {

  int word_match = 0;
  char c;
  int word_len = strlen(match_word);

  while ((c = getc(fd)) != EOF) {
    for (int i = 0; i < word_len; ++i) {
      if (match_word[i] != c)
        break;
      if (i == word_len - 1) {
        printf("Match Found: %s\n", match_word);
        /* provided word match found return true  */
        return 1;
      }
      c = getc(fd);
    }
  }
  printf("!! No Match Found: %s\n", match_word);
  return 0;
}

void print_section(FILE *fd, char stop_chara) {
  char c;
  while (c = getc(fd), c) {
    if (c == stop_chara)
      return;
    putc(c, stdout);
  }
}
/*
 *  control chars  "{ , : [  ] }"
 *  chk if any non-control char is occured after <SPC> char. if it
 *  occured that <SPC> char is valid. else ignore
 */

void parse_values(FILE *fd, const char *word) {
  int val;
  char c;
  word_finder(fd, word);

  while ((c = getc(fd)) != EOF) {
    putc(c, stdout);
  }
  /*
fscanf(fd, "x : %d", &val);
printf("val: %d\n", val);

print_section(fd, ']');
printf("\n");
fclose(fd);
     */
}
