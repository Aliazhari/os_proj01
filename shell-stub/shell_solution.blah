/*
 * shell.c
 * Author: Zach Kissel
 * Date: 2/9/15
 * Course: CSC3320
 * Description: This file implements a simple Unix shell.
 */
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

/* Our libraries. */
#include <history.h>
#include <joblist.h>

/* Define the maximum command length */
#define CMDLEN	1024

/* Define the maximum history size. */
#define MAX_HISTORY_SIZE	5

/* Prototypes */
void get_command(char *);
void sanitize_string(char *);
int process_cmd(char *, history_hdl_t*, joblist_hdl_t*);
void run_external_cmd(char*, joblist_hdl_t*);
int build_argument_array(char***, int*, char*);

int main(int argc, char **argv)
{
	int done = 0;	/* Determines if the user exites the shell. */
	char cmd[CMDLEN];	/* Allocate space for a command. */
	history_hdl_t *cmd_history;	/* Holds the command history. */
	joblist_hdl_t *jobs;	/* The joblist. */

	/* Setup the history and joblist */
	cmd_history = history_create(MAX_HISTORY_SIZE);
	jobs = joblist_create(30);

	/* Loop until the user no longer wants the shell to run. */
	while (!done)
	{
		/* Get the command. */
		get_command(cmd);

		/*
 		 * Only try to process a command if it is *not*
 		 * the empty string.
 		 */
		if (cmd[0] != '\0')
			done = process_cmd(cmd, cmd_history, jobs);
	}

	/* Free the history. */
	history_destroy(cmd_history);

	/* Free joblist. */
	joblist_destroy(jobs);

	return 0;
}

/*
 * get_command
 * Description: This function fills the array cmd with the
 * 		command run from the comamnd line. The
 * 		command line itself has no trailing spaces.
 */
void get_command(char *cmd)
{
	/* Zero the cmd buffer. */
	memset(cmd, 0, CMDLEN);

	/* Prompt for the command. */
	printf("msh> ");
	fgets(cmd, CMDLEN, stdin);

	/* Strip a trailing newline. */
	cmd[strlen(cmd) - 1] = '\0';

	sanitize_string(cmd);
}

/*
 * sanitize_string
 * Description: This function removes all the leading and trailing
 * 		spaces from a string.
 * Pre: cmd is not NULL
 * Post: The cmd pointed to is stripped of leading and trailing white space.
 */
void sanitize_string(char* cmd)
{
	int idx;		/* Use to track position in the string. */

	/* Strip all trailing white space. */
	for(idx = strlen(cmd) - 1; isspace(cmd[idx]) != 0 &&
	    idx >= 0; idx--)
		cmd[idx] = '\0';

	/* Strip leading white space. */
	while (isspace(cmd[0]) != 0)
		for (idx = 0; idx < strlen(cmd) - 1; idx++)
			cmd[idx] = cmd[idx + 1];
}

/*
 * process_cmd
 * Description: This function takes a command and attempts to
 * 		perform the requested action.
 * Pre: cmd != NULL
 * Post: 1 is returned if the quit command was executed and 0 otherwise.
 */
int process_cmd(char *cmd, history_hdl_t *history, joblist_hdl_t* jobs)
{
	char *exec_cmd = NULL;	/* The new command to execute. */
	char error_msg[160];	/* The error messages for perror. */
	pid_t	completed;	/* The PID of the completed process. */

	assert(cmd != NULL);	/* Make sure there is a command. */

	/*
 	 * Add the command to the history if it is
 	 * NOT a !! or ! command
 	 */
	if (cmd[0] != '!')
		history_add(history, cmd);

	/*
 	 * First try to handle all of the builtins.
 	 * If all those tests fail, treat the command as
 	 * an external command.
 	 */
	if (strcmp(cmd, "exit") == 0 || strcmp(cmd, "quit") == 0)
	{
		/*
		 * We don't want to create zombies/orphans. This means we
		 * must wait for all the background processes to
		 * end.
		 */
		while(!joblist_is_empty(jobs))
		{
			completed = -2;		/* Impossible value. */

			/* Wait for a process to complete. */
			completed = wait(NULL);

			if (completed == -1 && errno == ECHILD)
				joblist_empty(jobs);
			else if (completed != -1)
				joblist_remove_job(jobs, completed);
		}
		return 1;
	}
	else if(strcmp(cmd, "history") == 0)	/* Handle history */
	{
		history_print(history);
		return 0;
	}
	else if(strcmp(cmd, "!!") == 0)	/* Handle repeat previous. */
	{
		if (!history_is_empty(history))
			/*
			 * We can safely use recursion here, as the
 			 * the only history command we add to the command
 			 * history is "history." Thus, we don't have to
 			 * worry about infinite recursion.
 			 */
			return process_cmd(history_get_last_cmd(history),
			    history, jobs);
	}
	else if (cmd[0] == '!')	/* Execute command n */
	{
		if ((exec_cmd = history_get_cmd(history,
		    atoi(&cmd[1]))) != NULL)
		{
			/*
			 * We can safely use recursion here, as the
 			 * the only history command we add to the command
 			 * history is "history." Thus, we don't have to
 			 * worry about infinite recursion.
 			 */
			return process_cmd(exec_cmd, history, jobs);
		}
	}
	else if(strncmp(cmd, "cd", 2) == 0)  /* Change current directory. */
	{
		/*
		 * Make the system call to change the
 		 * working directory.
 		 */
		if (chdir(&cmd[3]) < 0)
		{
			memset(error_msg, 0, 160);
			sprintf(error_msg, "cd: %s", &cmd[3]);
			perror(error_msg);
		}
		return 0;
	}

	/*
 	 * We don't understand the command. Therefore, it must
 	 * must be an external command.
 	 */
	run_external_cmd(cmd, jobs);

	return 0;	/* We are not done. */
}

/*
 * run_external_cmd
 * Description: This function attempts to run an external command.
 * Pre: cmd != NULL
 * Post: None.
 */
void run_external_cmd(char* cmd, joblist_hdl_t *jobs)
{
	pid_t pid; 		/* The process id */
	pid_t completed;	/* The PID of the complted process. */
	char **argv = NULL;	/* The array of arguments. */
	int argc;		/* The number of arguments. */
	int idx;		/* The current location in argv. */
	char err_msg[160];	/* Holds the error message to display. */
	int isBackground = 0;	/* Holds 1 if we should background the cmd. */

	/* Check for a background command. */
	if (cmd[strlen(cmd) - 1] == '&')
	{
		cmd[strlen(cmd) - 1] = '\0';
		isBackground = 1;
		sanitize_string(cmd);
	}

	/* Build the array of arguments. */
	if (build_argument_array(&argv, &argc, cmd) != 0)
	{
		perror("Internal error");
		return;
	}


	pid = fork();	/* Fork a child process. */

	if (pid != 0)	/* The parent. */
	{
		/* Wait for the child to complete execution */
		if (!isBackground)
		{
			completed = -2;

			/* Reap all jobs that have completed. */
			while (pid != completed)
			{
				completed = wait(NULL);

				if (completed == -1 && errno == ECHILD)
				{
					joblist_empty(jobs);
                                        completed = pid;
				}

				if (completed != pid)
				{
					joblist_remove_job(jobs, completed);
				}
			}
		}
		else
			joblist_add_job(jobs, pid); /* add job to list. */

		/* Free the memory we allocated for argv. */
		for (idx = 0; idx < argc; idx++)
		{
			if (argv[idx] != NULL)
				free(argv[idx]);
			argv[idx] = NULL;
		}
		if (argv != NULL)
			free(argv);
		argv = NULL;
	}
	else		/* The child */
	{
		/*
 		 * Replace the process image with
 		 * a new program image.
 		 */
		if (execvp(argv[0], argv) < 0)
		{
			/* Build and dispaly and error message. */
			memset(err_msg, 0, 160);
			sprintf(err_msg, "%s", argv[0]);
			perror(err_msg);

			/* Free the memory we allocated for argv. */
			for (idx = 0; idx < argc; idx++)
			{
				if (argv[idx] != NULL)
					free(argv[idx]);
				argv[idx] = NULL;
			}
			if (argv != NULL)
				free(argv);
			argv = NULL;

			exit(1);	/* exit the child process. */
		}
	}
}

/*
 * build_argument_array
 * Description: This function takes a command an breaks it up into
 * 		an array of arguments. The last command is NULL.
 * Pre: cmd != NULL
 * Post: argc holds the number of arguments and argv holds the
 * 	 command line arguements themselves. The function returns
 * 	 ENOMEM on bad allocations and 0 on success.
 */
int build_argument_array(char***argv, int* argc, char* cmd)
{
	int idx;	/* The current location in cmd. */
	char *tok;	/* The current token. */

	assert(cmd != NULL);	/* Make sure the command is NOT NULL. */

	/*
	 * Determine the number of arguemtns by counting the number
 	 * of spaces.
 	 */
	*argc = 1;
	for (idx = 0; idx < strlen(cmd); idx++)
		if (cmd[idx] == ' ')
			(*argc)++;

	/*
 	 * Allocate the space for you argument array.
 	 */
	*argv = (char **)calloc(*argc + 1, sizeof (char *));

	/* Check to see if the allocation succeeded. */
	if (*argv == NULL)
		return ENOMEM;

	/* Set all strings to NULL. */
	for (idx = 0; idx < *argc + 1; idx++)
	    (*argv)[idx] = NULL;


	/*
	 * Walk through the command creating each necessary
 	 * command.
 	 */
	idx = 0;
	tok = strtok(cmd, " ");
	while(tok != NULL)
	{
		/* Copy the string into the argument array. */
		(*argv)[idx] = (char *)calloc(strlen(tok) + 1,  sizeof (char));
		strncpy((*argv)[idx], tok, strlen(tok));
 		idx++;

 		/* Continue tokenizing the same strings. */
		tok = strtok(NULL, " ");
	}

	return 0;
}
