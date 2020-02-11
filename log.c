#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>


void write_log(int nof_parameters, int file_descriptor, ...) {

  int hours, minutes, seconds;
  char input[100];

  time_t now;
	time(&now);

  struct tm *local = localtime(&now);
  hours = local->tm_hour;
  minutes = local->tm_min;
  seconds = local->tm_sec;

  sprintf(input, " PID %d at %02d:%02d:%02d ", getpid(), hours, minutes, seconds);

  va_list input_list;
  int i;

  va_start(input_list, file_descriptor); // va_start used to start before accessing arguments

  for(i = 0; i < nof_parameters; i++) {
      sprintf(input + strlen(input), "%s", va_arg(input_list, char*));
   }

   sprintf(input + strlen(input), "\n");
   va_end(input_list); // va_end used after completing access of arguments

  int j;
	while ((j = write(file_descriptor, input, strlen(input))) == -1) {
      // Try again if someone else is reading the file
   		if (j == EWOULDBLOCK || EAGAIN) {
   			continue;
   		} else {
   			return;
   		}
   	}
}
