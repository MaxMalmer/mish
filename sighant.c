#include "sighant.h"
#include "mish.h"

/* sighant.c
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

#define _POSIX_C_SOURCE 200809L

/**
 * sig_handler - Waits for a signal from the system and executes it.
 * @signo: The signal, for example SIGINT.
 *
 */
void sig_handler(int signo) {

    if (signo == SIGINT) {
        kill_all(signo);
    }
}

/**
 * sighant - A function that waits for signals either from the user or the
 * OS itself.
 * @signo: The signal, for example SIGINT.
 *
 * Returns: 0 if EXIT_SUCCESS or ERROR if a sigaction can't be taken.
 */
int sighant(signal_function *function, int signo) {
    struct sigaction action, action2;
    action.sa_handler = function;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;

    if (signo == SIGALRM) {
        #ifdef	SA_INTERRUPT
        action.sa_flags |= SA_INTERRUPT;
        #endif
    } else {
        #ifdef	SA_RESTART
        action.sa_flags |= SA_RESTART;
        #endif
    }
    if (sigaction(signo, &action, &action2) < 0) {
        return ERROR;
    }
    return 0;

}
