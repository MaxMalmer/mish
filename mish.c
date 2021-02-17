#include "mish.h"

/* mish.c
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

// ===========PUBLIC DATA TYPES============

static const unsigned int MAX_COMMANDS = 2;
void sig_handler(int);
pid_t pipes[MAXCOMMANDS + 1];
int num_commands;

/**
 * main() - Main start function of the mish shell.
 *
 * Returns: 0 on a EXIT_SUCCESS and ERROR if there's an error with errno set
 * or a proper print to stderr.
 */
int main(void) {
    sighant(SIG_IGN, SIGINT);

    while (true) {
        command comLine[MAX_COMMANDS + 1];
        char current_command[MAXLINELEN];
        num_commands = 0;

        if (isatty(STDOUT_FILENO) && isatty(STDIN_FILENO)) {
            fprintf(stdout, "mish%% ");
            fflush(stdout);
        }

        if (fgets(current_command, MAXLINELEN, stdin) == NULL) {
            fprintf(stderr, "\n");
            exit(EXIT_SUCCESS);
        }

        if (strlen(current_command) == 0) {
            break;
        }

        if ((num_commands = parse(current_command, comLine)) == 0){
            continue;
        }

        if (num_commands < 0) {
            fprintf(stderr, "Command related error\n");
            exit(1);
        }

        if (internal_command(comLine)) {
            continue;
        }

        if ((pipes[0] = getpid()) < 0) {
            perror("Pids haven't been correctly set");
            exit(errno);
        }
        external_command(comLine);
    }
    return 0;
}

/**
 * internal_command() - Checks if the command is eternal and calls it. Exits the
 * shell if the line ''exit'' is given.
 * @comLine: An array of commands as a structure.
 *
 * Returns: true if one of the arguments are internal false otherwise.
 */
bool internal_command(command comLine[]) {
    command first_command = comLine[0];

    if (strcmp(first_command.argv[0], "exit") == 0) {
        exit(EXIT_SUCCESS);
    }

    if (strcmp(first_command.argv[0], "echo") == 0) {
        echo(comLine);
        return true;
    }

    if (strcmp(first_command.argv[0], "cd") == 0) {
        cd(comLine);
        return true;
    } else {
        return false;
    }
}

/**
 * external_command() - Executes external commands given by the user. If there's
 * more then one command specified a pipe is created between the two. If a in or
 * outfile is specified in command the function either reads from it or writes
 * to it.
 * @comLine: An array of commands as a structure.
 *
 * Returns: 0 if EXIT_SUCCESS and exits with exit(1) upon error.
 */
int external_command(command comLine[]) {
    int filedes_array[MAXCOMMANDS][2];
    int status;

    if (sighant(sig_handler, SIGINT) == ERROR) {
        fprintf(stderr, "Couldn't register signal handler\n");
        perror("signal fault");
        exit(1);
    }

    for (int i = 0; i < num_commands; i++) {
        command this_command = comLine[i];

        if (num_commands - 1 != i) {

            if (pipe(filedes_array[i]) < 0) {
                perror("pipe");
                exit(1);
            }
        }

        if((pipes[i + 1] = fork()) < 0) {
            perror("Fork related error");
            exit(errno);
        }

        if (pipes[i + 1] == 0) {

            if (num_commands > 1) {

                if (i > 0) {
                    dupPipe(filedes_array[(i + 1) % 2], READ_END, STDIN_FILENO);
                }

                if (i < num_commands - 1) {
                    dupPipe(filedes_array[i % 2], WRITE_END, STDOUT_FILENO);
                }
            }
            file_redirect(this_command, i);

            if (execvp(this_command.argv[0], this_command.argv) < 0) {
                perror("Couldn't exec given command");
                _exit(1);
            }
        } else if (i > 0) {

            if (close(filedes_array[(i + 1) % 2][WRITE_END]) < 0 ||
                close(filedes_array[(i + 1) % 2][READ_END]) < 0) {
                fprintf(stderr, "Pipe couldn't be closed\n");
            }
        }
    }

    for (int k = 0; k < num_commands; k++) {
        waitpid(pipes[k + 1], &status, WUNTRACED);
    }
    return 0;
}

/**
 * file_redirect() - Checks if the current command has an in our an out file.
 * It the executes the function that writes to it.
 * @this_command: The current command in the comLine array structure.
 * @i: The current iteration of the external command loop.
 *
 * Returns: 0 if EXIT_SUCCESS and ERROR if file can't opened.
 */
int file_redirect(command this_command, int i) {

    if ((i == 0) && this_command.infile != NULL) {

        if(redirect(this_command.infile, READ_END, STDIN_FILENO) == ERROR) {
            fprintf(stderr, "Error opening infile for one command\n");
            return ERROR;
        }
    }

    if ((num_commands - 1 == i) && this_command.outfile != NULL){

        if(redirect(this_command.outfile, WRITE_END, STDOUT_FILENO) == ERROR) {
            fprintf(stderr, "Error opening outfile for one command\n");
            return ERROR;
        }
    }
    return 0;
}

/**
 * cd() - A simple interal implementation of the cd function of a shell.
 * @comLine: An array of commands as a structure.
 *
 * Returns: 0 if EXIT_SUCCESS and ERROR if the dir can't be changed.
 */
int cd(command comLine[]) {

    if (comLine[0].argv[1] == NULL) {

        if (chdir(getenv("HOME")) < 0) {
            perror("Can't change dir:");
            return ERROR;
        }
    } else if (comLine[0].argv[1] != NULL) {

        if (chdir(comLine[0].argv[1]) < 0) {
            perror("Can't change dir:");
            return ERROR;
        }
    }
    return 0;
}

/**
 * echo() - A simple interal implementation of the echo function of a shell.
 * @comLine: An array of commands as a structure.
 *
 * Returns: 0 if EXIT_SUCCESS.
 */
int echo(command comLine[]) {

    for (int i = 0; i < comLine[0].argc - 1; i++) {

        if (i + 1 != comLine[0].argc - 1) {
            fprintf(stdout, "%s ", comLine[0].argv[i + 1]);
        } else {
            fprintf(stdout, "%s", comLine[0].argv[i + 1]);
        }
    }
    printf("\n");
    return 0;
}

/**
 * kill_all() - A function that when called on with SIGINT kills all active
 * sub-processes.
 * @signo: The signal type from the system.
 *
 */
void kill_all(int signo) {

    if (signo == SIGINT) {

        for (int i = 0; i < num_commands; i++) {

            if (pipes[i + 1] != 0) {
                kill(pipes[i + 1], SIGINT);
            }
        }
    }
}
