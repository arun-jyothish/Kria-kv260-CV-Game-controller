#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h> // For close()
#include <stdlib.h>

#define PORT 8080
#define BUF_SIZE 1024

#define SHIFT_THRESHOLD 50
#define X_TOLERANCE	20
#define MAX_HEIGHT	480
#define MAX_WIDTH	640

typedef enum {
  UP,
  DOWN,
  LEFT,
  RIGHT
} control_t;

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

control_t control_logic(const CV_Parsed_Parameters *st);

void send_msg(char *msg);
/* void send_msg(); */

void init_comm(void);
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
char* print_parsed_values(const CV_Parsed_Parameters *st);

/*
 * Parse Values from file descriptor
 * x, y ,width, height, class ..etc
 */
void parse_values(FILE *fd, CV_Parsed_Parameters *st);


#define PIPE_NAME "cv_pipe" // Define the name of the pipe

int sockfd;
char buffer[BUF_SIZE];
struct sockaddr_in server_addr;

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

    init_comm();
    CV_Parsed_Parameters person;

    // Continuously read from the pipe
    while (1) {
      parse_values(fd, &person);
      char *gen_msg = print_parsed_values(&person);
      control_logic(&person);
      /* send_msg(gen_msg); */
      /* send_msg(); */
      fflush(stdout); // Ensure output is printed immediately
    }

    fclose(fd); // Close the FIFO when done (this will not happen in this loop)
  }

  return 0;
}

char* print_parsed_values(const CV_Parsed_Parameters *st) {
  static char buffer[128];
  puts("------------------------------------");
  sprintf(buffer, "x_value: %d\t\t", st->x_value);
  sprintf(buffer + strlen(buffer), "y_value: %d\t\t", st->y_value);
  sprintf(buffer + strlen(buffer), "width: %d\t\t", st->width);
  sprintf(buffer + strlen(buffer), "height: %d\t\t", st->height);
  sprintf(buffer + strlen(buffer), "label: %s\n", st->label);
  printf("%s", buffer);
  puts("------------------------------------");
  return buffer;
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

void init_comm()
{
  
  // Create socket
  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("Socket creation failed");
    exit(EXIT_FAILURE);
  }

  memset(&server_addr, 0, sizeof(server_addr));

  // Configure server address
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  server_addr.sin_addr.s_addr = inet_addr("10.42.0.10"); // Server IP address
  /* server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Server IP address */

}

void send_msg(char *msg)

/* void send_msg() */
{

  // Send message to server
  /* const char *msg = "Hello, Server!"; */
  sendto(sockfd, msg, strlen(msg), 0, (const struct sockaddr *)&server_addr,
         sizeof(server_addr));
  printf("Message sent to server\n");

  // Receive acknowledgment from server
  /*

    socklen_t len = sizeof(server_addr);
    int n = recvfrom(sockfd, buffer, BUF_SIZE, 0, (struct sockaddr *)&server_addr,
    &len); buffer[n] = '\0'; // Null-terminate the received string printf("Server:
    %s\n", buffer);
  
  */

  /* close(sockfd); */
}

typedef enum{
  NO_SHIFT,
  LEFT_SHIFT,
  RIGHT_SHIFT
} shift_t;

control_t control_logic(const CV_Parsed_Parameters *st) {
  static CV_Parsed_Parameters prev_st;
  int delta_x = st->x_value - prev_st.x_value;
  int delta_y = st->y_value - prev_st.y_value;

  int delta_width = st->width - prev_st.width;
  int delta_height = st->height - prev_st.height;

  int frame_shift = 0;

  // find mod tolerance
  /* delta_x = delta_x < 0 ? -1 * delta_x, frame_shift = LEFT_SHIFT : delta_x; */
  delta_x = delta_x < 0 ? -1 * delta_x : delta_x;
  /* frame_shift = delta_x > 0 ? RIGHT_SHIFT : NO_SHIFT; */

  /*

    switch (frame_shift) {
    NO_SHIFT:
    break;
    LEFT_SHIFT:
    break;
    RIGHT_SHIFT:
    break;
    default:
    printf("frameshift error \n");
    }
  */

  /* if (delta_x < X_TOLERANCE && SHIFT_THRESHOLD > delta_width) */

#define RIGHT		"RIGHT"
#define LEFT		"LEFT"
#define UP		"UP"
#define DOWN		"DOWN"
#define STABLE		"STABLE"
#define BOTTOM_MARGIN	50
#define TOP_MARGIN	50
  /*

    if (strcmp(st->label, "person")) {
    printf("!!! not a person ..\n");
    return 0;
    }
  */

  if (st->x_value < MAX_WIDTH/3){
    sendto(sockfd, RIGHT,  strlen(RIGHT), 0, (const struct sockaddr *)&server_addr,
           sizeof(server_addr));
    printf("\033[033m ........ RIGHT ........>\n\033[0m");
  }
  if (st->x_value > 2 * MAX_WIDTH / 3) {
    sendto(sockfd, LEFT, strlen(LEFT), 0, (const struct sockaddr *)&server_addr,
           sizeof(server_addr));
    printf("\033[031m ........ LEFT .........>\n\033[0m");
  }
  if (st->y_value < TOP_MARGIN) {
    sendto(sockfd, UP, strlen(UP), 0,
	   (const struct sockaddr *)&server_addr, sizeof(server_addr));
    printf("\033[035m ........ UP ...........>\n\033[0m");
  }
  if (st->y_value + st->height > MAX_HEIGHT - BOTTOM_MARGIN) {
    sendto(sockfd, DOWN, strlen(DOWN), 0, (const struct sockaddr *)&server_addr,
	   sizeof(server_addr));
    printf("\033[034m ........ DOWN ...........>\n\033[0m");
  }
  /* if (st->x */
  /*     ) { */
  /*   sendto(sockfd, STABLE, strlen(STABLE), 0, */
  /* 	   (const struct sockaddr *)&server_addr, sizeof(server_addr)); */
  /*   printf("\033[032m ........STABLE.........>\n\033[0m"); */
  /* } */

  prev_st.width = st->width;
  prev_st.x_value = st->x_value;
}
