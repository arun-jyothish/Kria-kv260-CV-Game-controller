#include <stdio.h>
#include <string.h>
#include <errno.h>

void match_finder(FILE * fd, const char* match_word, short word_len);

void main()
{
	FILE* fd = fopen("inference.txt", "r");
	perror("Opening File");
	if (fd == NULL)	
		return;
	const char pred[] = "predictions";
	match_finder(fd, pred, strlen(pred));

	fclose(fd);
}

void match_finder(FILE * fd, const char* match_word, short word_len)
{

  int predition_match = 0;
  char c;

  while ((c = getc(fd)) != EOF) {
		//putc(c, stdout);
		for (int i = 0; i < word_len; ++i) {
			if (match_word[i] != c)
				break;
			if (i = word_len - 1){
				predition_match = 1;
				break;
			}
			c = getc(fd);
		}
		if (predition_match){
			printf("#------------------------------# \n\n");
			while ((c = getc(fd)) != ']' && c != EOF) {
				putc(c, stdout);
      }
			printf("*------------------------------* \n\n");

		}
	} 

}


/*
 *  control chars  "{ , : [  ] }"
 *  chk if any non-control char is occured after <SPC> char. if it
 *  occured that <SPC> char is valid. else ignore 
 */
