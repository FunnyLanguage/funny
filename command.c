/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2014 by fanguangping (fanguangping@163.com)
 *
 */

#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include "command.h"

#define MAXLINE 81

void init_info(ParseInfo *p) {
	int i;
	p->boolInfile = 0;
	p->boolOutfile = 0;
	p->boolBackground = 0;
	p->pipeNum = 0;

	for (i = 0; i < PIPE_MAX_NUM; i++) {
		p->commArray[i].command = NULL;
		p->commArray[i].varList[0] = NULL;
		p->commArray[i].varNum = 0;
	}
}

void parse_command(char * command, struct CommandType *comm) {
	int i = 0;
	int pos = 0;
	char word[MAXLINE];

	comm->varNum = 0;
	comm->command = NULL;
	comm->varList[0] = NULL;
	while (isspace(command[i]))
		i++;
	if (command[i] == '\0')
		return;
	while (command[i] != '\0') {
		while (command[i] != '\0' && !isspace(command[i])) {
			word[pos++] = command[i++];
		}
		word[pos] = '\0';
		comm->varList[comm->varNum] = malloc((strlen(word) + 1) * sizeof(char));
		strcpy(comm->varList[comm->varNum], word);
		comm->varNum++;
		word[0] = '\0';
		pos = 0;
		while (isspace(command[i]))
			i++;
	}
	comm->command = malloc((strlen(comm->varList[0]) + 1) * sizeof(char));
	strcpy(comm->command, comm->varList[0]);
	comm->varList[comm->varNum] = NULL;
}

ParseInfo *parse_info(char *cmdline) {
	ParseInfo *result;
	int i = 0;
	int pos;
	int end = 0;
	char command[MAXLINE];
	int com_pos;

	if (cmdline[i] == '\n' && cmdline[i] == '\0')
		return NULL;

	result = malloc(sizeof(ParseInfo));
	if (result == NULL) {
		return NULL;
	}
	init_info(result);
	com_pos = 0;
	while (cmdline[i] != '\n' && cmdline[i] != '\0') {
		if (cmdline[i] == '&') {
			result->boolBackground = 1;
			if (cmdline[i + 1] != '\n' && cmdline[i + 1] != '\0') {
				fprintf(stderr, "Ignore anything beyond &.\n");
			}
			break;
		} else if (cmdline[i] == '<') {
			result->boolInfile = 1;
			while (isspace(cmdline[++i]))
				;
			pos = 0;
			while (cmdline[i] != '\0' && !isspace(cmdline[i])) {
				if (pos == FILE_MAX_SIZE) {
					fprintf(stderr,
							"Error.The input redirection file name exceeds the size limit 40\n");
					free_info(result);
					return NULL;
				}
				result->inFile[pos++] = cmdline[i++];
			}
			result->inFile[pos] = '\0';
			end = 1;
			while (isspace(cmdline[i])) {
				if (cmdline[i] == '\n')
					break;
				i++;
			}
		} else if (cmdline[i] == '>') {
			result->boolOutfile = 1;
			while (isspace(cmdline[++i]))
				;
			pos = 0;
			while (cmdline[i] != '\0' && !isspace(cmdline[i])) {
				if (pos == FILE_MAX_SIZE) {
					fprintf(stderr,
							"Error.The output redirection file name exceeds the size limit 40\n");
					free_info(result);
					return NULL;
				}
				result->outFile[pos++] = cmdline[i++];
			}
			result->outFile[pos] = '\0';
			end = 1;
			while (isspace(cmdline[i])) {
				if (cmdline[i] == '\n')
					break;
				i++;
			}
		} else if (cmdline[i] == '|') {
			command[com_pos] = '\0';
			parse_command(command, &result->commArray[result->pipeNum]);
			com_pos = 0;
			end = 0;
			result->pipeNum++;
			i++;
		} else {
			if (end == 1) {
				fprintf(stderr, "Error.Wrong format of input\n");
				free_info(result);
				return NULL;
			}
			if (com_pos == MAXLINE - 1) {
				fprintf(stderr,
						"Error. The command length exceeds the limit 80\n");
				free_info(result);
				return NULL;
			}
			command[com_pos++] = cmdline[i++];
		}
	}
	command[com_pos] = '\0';
	parse_command(command, &result->commArray[result->pipeNum]);
	/*Result->pipeNum++;*/
	return result;
}

void free_info(ParseInfo *info) {
	int i, j;
	struct CommandType *comm;

	if (NULL == info)
		return;
	for (i = 0; i < PIPE_MAX_NUM; i++) {
		comm = &(info->commArray[i]);
		for (j = 0; j < comm->varNum; j++) {
			free(comm->varList[j]);
		}
		if (NULL != comm->command) {
			free(comm->command);
		}
	}
	free(info);
}

enum BUILTIN_COMMANDS {
	NO_SUCH_BUILTIN = 0, JOBS, CD, KILL, HELP
};

//char global_history_count;
char global_jobs_count;
//char history[100][20];
char jobs[100][20];
pid_t background_pid;

void display_jobs() {
	int i = 0;

	if (jobs[i][0] != '\0') {
		printf("%d %s\n", i + 1, *(jobs + i));
		++i;
	} else {
		printf("\n No background jobs \n");
	}
}

int isBackgroundJob(ParseInfo * info) {
	return info->boolBackground;
}

void makebackgroundjobs() {
	char *pArgs[10];
	int background;
	int status;

	if (strlen(pArgs[0]) > 1) {
		background_pid = fork();
		if (background_pid == -1) {
			perror("fork");
			exit(1);
		} else if (background_pid == 0) {
			execvp(pArgs[0], pArgs);
			exit(1);
		} else if (!background) {
			background_pid = waitpid(background_pid, &status, 0);
			if (background_pid > 0)
				printf("waitpid child pid %d\n", background_pid);
		}
	}
}

/*prints current working directory*/
void printPrompt() {
	char cwd[256];
	char *username = getenv("USER");

	char hostname[1024];
	hostname[1023] = '\0';
	gethostname(hostname, 1023);

	if (getcwd(cwd, sizeof(cwd)) == NULL) {
		perror("getcwd() doesn't working");
	} else {
		printf("%s@%s %s ", username, hostname, cwd);
	}
}

void execute_cmd(ParseInfo *info) {
	pid_t pid;
	int status = 1;

	pid = fork();
	if (pid == 0) {
		execvp(info->commArray[0].command, info->commArray[0].varList);
		exit(0);
	} else {
		waitpid(-1, &status, 0);
		/*	wait(pid);
		 printf("at parent function");*/
	}
}

char * buildPrompt() {
	printPrompt();
	return "$";
}

int isBuiltInCommand(char * cmd) {
	if (strncmp(cmd, "jobs", strlen("jobs")) == 0) {
		return JOBS;
	}
	if (strncmp(cmd, "cd", strlen("cd")) == 0) {
		return CD;
	}
	if (strncmp(cmd, "kill", strlen("kill")) == 0) {
		return KILL;
	}
	if (strncmp(cmd, "help", strlen("help")) == 0) {
		return HELP;
	}
	return NO_SUCH_BUILTIN;
}

void execute_command(char *cmd) {
	FILE *infile = NULL;
	FILE *outfile = NULL;
	int stdIN, stdOUT;
	int jobs_index = 0;
	//char * cmdLine;
	ParseInfo *info; /*info stores all the information returned by parser.*/
	struct CommandType *com; /*com stores command name and Arg list for one command.*/

	stdIN = dup(fileno(stdin));
	stdOUT = dup(fileno(stdout));

	if (cmd == NULL) {
		fprintf(stderr, "Unable to read command\n");
		return;
	}

	/*calls the parser*/
	info = parse_info(cmd);
	if (info == NULL) {
		free(cmd);
		return;
	}

	com = &info->commArray[0];
	if ((com == NULL) || (com->command == NULL)) {
		free_info(info);
		free(cmd);
		return;
	}

	if (isBuiltInCommand(com->command) == JOBS) {
		display_jobs();
	}

	if (isBuiltInCommand(com->command) == CD) {
		const char * const path = com->varList[1];
		if (chdir(path) == -1) {
			printf("change directory failed\n");
		}
	}

	if (isBuiltInCommand(com->command) == KILL) {
		const char * const path = com->varList[1];
		int kill_pid = 0;

		kill_pid = atoi(com->varList[1]);

		if (kill_pid <= 0) {
			printf("\nInvalid pid:%d\n", kill_pid);
		} else {
			//kill(kill_pid, SIGKILL);
		}
	}

	if (isBuiltInCommand(com->command) == HELP) {
		printf("help: prints this help screen.\n\n");
		printf(" jobs - prints out the list of running background jobs.\n\n");
		printf(
				" kill [process_id] - kills background process_id to kill a running background job.\n\n");
		printf(" cd [location] - change directory\n\n");
		printf(
				" sometext < infile.txt > outfile.txt - create a new process to run sometext and assign STDIN for the new process to infile and STDOUT for the new process to outfile\n\n");
		printf(
				"Append & to the end of any command to run it in the background.\n\n");
	}

	if (isBackgroundJob(info)) {
		/*record in an array of background jobs*/
		char * previous_command;
		previous_command = (char*) com->command;
		printf("%s\n", previous_command);

		while (jobs[jobs_index][0] != '\0') {
			jobs_index++;
		}

		strcpy(jobs[jobs_index], cmd);
		++global_jobs_count;
		printf("\n jobs array: %s\n", jobs[jobs_index]);
		makebackgroundjobs();
	}

	if (strcmp(info->inFile, "") != 0) {
		infile = fopen(info->inFile, "r");
		dup2(fileno(infile), 0);
	}

	if (strcmp(info->outFile, "") != 0) {
		outfile = fopen(info->outFile, "w");
		dup2(fileno(outfile), 1);
	}

	execute_cmd(info);
	fflush(stdout);
	fflush(stdin);

	if (infile != NULL) {
		fclose(infile);
	}

	if (outfile != NULL) {
		fclose(outfile);
	}

	dup2(stdIN, 0);
	dup2(stdOUT, 1);

	free_info(info);
	free(cmd);

	/*
	 int childPid;

	 cmd = parseCommand(cmd);
	 if ( isBuiltInCommand(cmd))
	 {
	 executeBuiltInCommand(cmd);
	 } else {
	 childPid = fork();
	 if (childPid == 0)
	 {
	 //executeCommand(cmd); //calls execvp
	 }
	 else
	 {
	 if (isBackgroundJob(cmd))
	 {
	 //record in list of background jobs
	 }
	 else
	 {
	 waitpid (childPid);
	 }
	 }
	 }
	 */
}

