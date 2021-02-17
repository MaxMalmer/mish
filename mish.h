#ifndef _SIGHANT_
#define _SIGHANT_

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <signal.h>

#include "parser.h"
#include "execute.h"
#include "sighant.h"

/* mish.h
 * The main implementation of a minimal unix shell. When started the shell
 * promts you with a mish% promt which you can specify command after. The shell
 * have the funtions cd and echo implemented in a simple form internaly, all
 * other commands are executed externaly.
 *
 * Authors: Max Malmer(c17mmr@cs.umu.se)
 * Assignment 3, 5DV088HT19
 *
 * Version information:
 *   2019-10-10: v1.0.
 */

#define ERROR -1
#define STDIN 1
#define EFILE 2

/**
 * internal_command() - Checks if the command is eternal and calls it. Exits the
 * shell if the line ''exit'' is given.
 * @comLine: An array of commands as a structure.
 *
 * Returns: true if one of the arguments are internal false otherwise.
 */
bool internal_command(command comLine[]);

/**
 * external_command() - Executes external commands given by the user. If there's
 * more then one command specified a pipe is created between the two. If a in or
 * outfile is specified in command the function either reads from it or writes
 * to it.
 * @comLine: An array of commands as a structure.
 *
 * Returns: 0 if EXIT_SUCCESS and exits with exit(1) upon error.
 */
int external_command(command comLine[]);

/**
 * file_redirect() - Checks if the current command has an in our an out file.
 * It the executes the function that writes to it.
 * @this_command: The current command in the comLine array structure.
 * @i: The current iteration of the external command loop.
 *
 * Returns: 0 if EXIT_SUCCESS and ERROR if file can't opened.
 */
int file_redirect(command this_command, int i);

/**
 * cd() - A simple interal implementation of the cd function of a shell.
 * @comLine: An array of commands as a structure.
 *
 * Returns: 0 if EXIT_SUCCESS and ERROR if the dir can't be changed.
 */
int cd(command comLine[]);

/**
 * echo() - A simple interal implementation of the echo function of a shell.
 * @comLine: An array of commands as a structure.
 *
 * Returns: 0 if EXIT_SUCCESS.
 */
int echo(command comLine[]);

/**
 * kill_all() - A function that when called on with SIGINT kills all active
 * sub-processes.
 * @signo: The signal type from the system.
 *
 */
void kill_all(int signo);

#endif
