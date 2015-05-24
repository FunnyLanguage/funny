/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2014 by fanguangping (fanguangping@163.com)
 *
 */

#ifndef _COMMAND_H_
#define _COMMAND_H_

#define MAX_VAR_NUM 10
#define PIPE_MAX_NUM 10
#define FILE_MAX_SIZE 40

struct CommandType {
  char *command;
  char *varList[MAX_VAR_NUM];
  int varNum;
};

/* parsing information structure */
typedef struct _ParseInfo{
  int   boolInfile;		       /* boolean value - infile specified */
  int   boolOutfile;		       /* boolean value - outfile specified */
  int   boolBackground;		       /* run the process in the background? */
  struct CommandType commArray[PIPE_MAX_NUM];
  int   pipeNum;
  char  inFile[FILE_MAX_SIZE];	       /* file to be piped from */
  char  outFile[FILE_MAX_SIZE];	       /* file to be piped into */
} ParseInfo;

/* the function prototypes */
ParseInfo *parse_info(char *);
void free_info(ParseInfo *);
void execute_command(char *cmd);

#endif // _COMMAND_H_
