/* setilog.cpp - seti@home log analyzer

SYNOPSIS:
  setiathome | setilog

DESCRIPTION:
  setilog analyzes the output of the setiathome program; it saves the
  first few lines, then scans the output for "% done" reports. all reports
  are saved in two log files. edit the #define's below to modify the file
  names.

BUGS/ISSUES:
  setiathome doesn't flush his stdout often, and so the log files are not
  written immediately after startup.

(c) 1999,2021 Michael Fink
https://github.com/vividos/OldStuff/
*/

// includes
#include <stdio.h>
#include <string.h>

// uncomment for debugging reasons
//#define DEBUG

// change this for other filenames
#define LOGFILE "seti_startlog.txt"
#define LOGLINES 24
#define DONEFILE "seti_done.txt"

// -------------------------------------------------------------

void main()
{
  FILE* input = stdin;

  char line[1024];
  line[1023] = 0;

  // check if another client is already running

  fgets(line, 1023, input);
  if (strstr(line, "Another instance of SETI") != NULL)
    return;

  // log the first LOGLINES lines

#ifdef DEBUG
  fprintf(stderr, "log the first %d lines ...\n", LOGLINES);
#endif

  FILE* log = fopen(LOGFILE, "wt");
  fputs(line, log);

  for (int i = 1; i < LOGLINES && !feof(input); i++)
  {
    fgets(line, 1023, input);
    fputs(line, log);
  };

  fclose(log);

  // log the xx.xx% done-msgs

#ifdef DEBUG
  fprintf(stderr, "log the 'xx.x% done'-messages ...\n");
#endif

  while (!feof(input))
  {
    fgets(line, 1023, input);
    if (strstr(line, "% done") != NULL) {
      FILE* done = fopen(DONEFILE, "wt");
      fputs(line, done);
      fclose(done);
    };
  };

};
