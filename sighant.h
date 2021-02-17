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
/* sighant.h
 * A couple of funtions that handles SIGINT, SIGALRM from the OS and other
 * terminals. POSIX C is defined so that the standard library of c is expanded
 * to use safer functions. SA_INTERRUPT and SA_RESTART is used for fast start
 * procedures even though they've been deprechiated for years. This was the
 * only interupt I got working well for most situations.
 *
 * Authors: Max Malmer(c17mmr@cs.umu.se)
 * Assignment 3, 5DV088HT19
 *
 * Version information:
 *   2019-10-10: v1.0.
 */

// ===========PUBLIC VOID FUNCTION============

typedef	void signal_function(int);
/**
 * sig_handler - Waits for a signal from the system and executes it.
 * @signo: The signal, for example SIGINT.
 *
 */
void sig_handler(int signo);

/**
 * sighant - A function that waits for signals either from the user or the
 * OS itself.
 * @signo: The signal, for example SIGINT.
 *
 * Returns: 0 if EXIT_SUCCESS or ERROR if a sigaction can't be taken.
 */
int sighant(signal_function *function, int signo);
