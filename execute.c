#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <assert.h>
#include <fcntl.h>

#include "execute.h"
#include "mish.h"

/* execute.c
 * A couple of funtions that prepares the filedescriptors of the program for
 * execution as specified by the user.
 *
 * Authors: Max Malmer(c17mmr@cs.umu.se)
 * Assignment 3, 5DV088HT19
 * Version information:
 *   2019-10-10: v1.0.
 */

/* Duplicate a pipe to a standard I/O file descriptor
 * Arguments:	pip	the pipe
 *		end	tells which end of the pipe shold be dup'ed; it can be
 *			one of READ_END or WRITE_END
 *		destfd	the standard I/O file descriptor to be replaced
 * Returns:	-1 on error, else destfd
 */
int dupPipe(int pip[2], int end, int destfd) {

    if (dup2(pip[end], destfd) < 0) {
        fprintf(stderr, "Process with pid %d couldn't be duped\n", pip[end]);
        return ERROR;
    } else {

        if (close(pip[end]) < 0) {
            fprintf(stderr, "Pip could not be closed after dup\n");
            return ERROR;
        }

        if (end == WRITE_END) {

            if (close(pip[READ_END]) < 0) {
                fprintf(stderr, "Pip could not be closed after dup\n");
                return ERROR;
            }
        }

        if (end == READ_END) {

            if (close(pip[WRITE_END]) < 0) {
                fprintf(stderr, "Pip could not be closed after dup\n");
                return ERROR;
            }
        }
        return destfd;
    }
}

/* Redirect a standard I/O file descriptor to a file
 * Arguments:	filename	the file to/from which the standard I/O file
 * 				descriptor should be redirected
 * 		flags	indicates whether the file should be opened for reading
 * 			or writing
 * 		destfd	the standard I/O file descriptor which shall be
 *			redirected
 * Returns:	-1 on error, else destfd
 */
int redirect(char *filename, int flags, int destfd) {

    if (flags == READ_END) {
        int filedes = open(filename, O_RDONLY);

        if (filedes < 0) {
            fprintf(stderr, "File %s can't be opened\n", filename);
            return ERROR;
        } else if (dup2(filedes, destfd) < 0) {
            fprintf(stderr, "File %s can't be duped\n", filename);
            return ERROR;
        }
    } else {

        if (access(filename, W_OK) == 0) {
            perror("File exists");
            errno = EEXIST;
            return ERROR;
        } else {
            int filedes2 = open(filename, O_WRONLY | O_CREAT | O_EXCL, S_IRWXU);

            if (filedes2 < 0) {
                fprintf(stderr, "File %s can't be opened\n", filename);
                return ERROR;
            }

            if (dup2(filedes2, destfd) < 0) {
                fprintf(stderr, "File %s can't be duped\n", filename);
                return ERROR;
            }
        }
    }
    return destfd;
}
