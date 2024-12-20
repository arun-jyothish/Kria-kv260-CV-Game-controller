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
 *  Find exact word match and seek file descriptor to that point
 */
int word_finder(FILE *fd, const char *match_word);

/*
 *  Print current Seek point to next occurance of character provided
 */
void print_section(FILE *fd, char stop_chara);

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

#define PIPE_NAME "cv_pipe"
