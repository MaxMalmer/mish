
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>

#include "mish.h"
#include "parser.h"
#include "execute.h"
#include "sighant.h"

static const unsigned int MAX_COMMANDS = 2;

int main(void) {
    //sighant();

    while (true) {
        command comLine[MAX_COMMANDS + 1];
        int num_commands = 0;
        char* current_command = malloc(sizeof(char*) * 1032);
        memory_control(current_command);

        if (isatty(STDOUT_FILENO) && isatty(STDIN_FILENO)) {
            fprintf(stdout, "mish%% ");
            fflush(stdout);
        }
        current_command = stream_reader(STDIN_FILENO, current_command);
        num_commands = parse(current_command, comLine);
        pid_t pipes[num_commands + 1];

        if (num_commands == ERROR) {
            fprintf(stderr, "Command related error.\n");
            exit(1);
        }
        command first_command = comLine[0];

        if (strcmp(first_command.argv[0], "exit") == 0) {
            return 0;
        }

        if (strcmp(first_command.argv[0], "echo") == 0) {
            strsep(&current_command, " ");
            echo(current_command);
            continue;
        }

        if (strcmp(first_command.argv[0], "cd") == 0) {

            if (strcmp(current_command, "cd") == 0) {
                cd(current_command);
            } else {
                strsep(&current_command, " ");
                cd(current_command);
            }
            continue;
        }
        pipes[0] = getpid();

        if (pipes[0] == ERROR) {
            perror("Fork related error");
            exit(errno);
        }

        for (int i = 0; i < num_commands; i++) {
            int filedes[2];
            int filedesold[2];
            command this_command = comLine[i];
            fprintf(stdout, "%d\n", this_command.argc);
            fprintf(stdout, "%s\n", this_command.argv[0]);
            fprintf(stdout, "%s\n", this_command.argv[1]);
            fprintf(stdout, "%s\n", this_command.argv[2]);
            fprintf(stdout, "%s\n", this_command.infile);
            fprintf(stdout, "%s\n", this_command.outfile);

            if (pipe(filedes) == ERROR) {
                perror("pipe");
                exit(1);
            }
            pipes[i + 1] = fork();

            if (pipes[i + 1] == ERROR) {
                perror("Fork related error");
                exit(errno);
            } else if (pipes[i + 1] == 0) {

                if (i + 1 != 0) {
                    memcpy(filedesold, filedes, sizeof(filedesold));
                }

                if (i + 1 != 0) {
                    printf("Fett\n");
                    filedes[0] = dupPipe(filedesold, 0, filedes[0]);
                    close(filedesold[0]);
                } else if (i + 1 <= num_commands - 1) {
                    printf("READ END\n");
                    filedes[1] = dupPipe(filedes, 1, filedes[1]);
                }
                char * const ptr[] = {this_command.argv[0], this_command.argv[1] , NULL};
                execvp(this_command.argv[0], ptr);
                perror("Couldn't exec given command");
                _exit(1);
            }
            char buffer[4096];
            memset(buffer, 0, sizeof(buffer));

            if (strlen(buffer) != 0) {
                fprintf(stderr, "The buffer isn't empty\n");
                exit(1);
            }

            while (i >= 0) {
                ssize_t count;
                printf("Kom till andra\n");
                count = read(filedes[0], buffer, sizeof(buffer));

                if (count == -1) {

                    if (errno == EINTR) {
                        continue;
                    } else {
                        perror("read");
                        exit(1);
                    }

                } else if (count == 0) {
                    break;
                } else {
                    write(filedes[1], buffer, strlen(buffer));
                    break;
                }
            }
            wait(0);
            close(filedes[0]);
        }
        //free(current_command);
        //close(filedes[0]);
    }
    return 0;
}

int one_external() {

}

int several_external() {

    for (int i = 0; i < num_commands; i++) {
        int filedes[2];
        int filedesold[2];
        command this_command = comLine[i];
        fprintf(stdout, "%d\n", this_command.argc);
        fprintf(stdout, "%s\n", this_command.argv[0]);
        fprintf(stdout, "%s\n", this_command.argv[1]);
        fprintf(stdout, "%s\n", this_command.argv[2]);
        fprintf(stdout, "%s\n", this_command.infile);
        fprintf(stdout, "%s\n", this_command.outfile);

        if (pipe(filedes) == ERROR) {
            perror("pipe");
            exit(1);
        }
        pipes[i + 1] = fork();

        if (pipes[i + 1] == ERROR) {
            perror("Fork related error");
            exit(errno);
        } else if (pipes[i + 1] == 0) {

            if (i + 1 != 0) {
                memcpy(filedesold, filedes, sizeof(filedesold));
            }

            if (i + 1 != 0) {
                printf("Fett\n");
                filedes[0] = dupPipe(filedesold, 0, filedes[0]);
                close(filedesold[0]);
            } else if (i + 1 <= num_commands - 1) {
                printf("READ END\n");
                filedes[1] = dupPipe(filedes, 1, filedes[1]);
            }
            char * const ptr[] = {this_command.argv[0], this_command.argv[1] , NULL};
            execvp(this_command.argv[0], ptr);
            perror("Couldn't exec given command");
            _exit(1);
        }
        char buffer[4096];
        memset(buffer, 0, sizeof(buffer));

        if (strlen(buffer) != 0) {
            fprintf(stderr, "The buffer isn't empty\n");
            exit(1);
        }
    }
}

int cd(char* buffer) {

    if (buffer == NULL) {
        chdir(getenv("HOME"));
    } else {

        if (chdir(buffer) == ERROR) {
            perror("Can't change dir:");
            return ERROR;
        }
    }
    return 0;
}

int echo(char* buffer) {
    fprintf(stdout, "%s\n", buffer);
    return 0;
}

char *stream_reader(int filedes, char* buffer) {

    while (true) {
        ssize_t count = read(filedes, buffer, sizeof(buffer));

        if (count == -1) {

            if (errno == EINTR) {
                continue;
            } else {
                perror("read");
                exit(1);
            }

        } else if (count == 0) {
            break;
        } else {
            break;
        }
    }
    return buffer;
}

// ===========HELP FUNCTIONS============

/**
 * memory_control() - Controls that memory is allocated.
 * @memory: Memory to be checked.
 *
 * Returns: Nothing if everything is okey. Otherwise exit is used and perror
 *          is used to specify the error.
 */
void memory_control(void *memory) {

    if (memory == NULL) {
        perror("Memory related error");
        exit(errno);
    }
}

/**
 * file_control() - Controls that a given input file can be opened.
 * @argv: The filename.
 * @argc: The type of input file.
 * Returns: 0 if the file is opened successfully. ERROR if something
 *          is out of the specification of passwd file.
 */
int file_control(const char *filename, int argc, int flags) {
    int file_type;
    FILE *f;

    if (argc == 1) {
        file_type = STDIN;

        if (flags == WRITE_END) {
            f = fopen(filename, "a");

            if (f == NULL) {
                fprintf(stderr, "Specified file cannot be read.\n");
                return ERROR;
            }
            fclose(f);
        }
    } else if (argc == 2) {
        file_type = EFILE;

        if (flags == WRITE_END) {
            f = fopen(filename, "a");

            if (f == NULL) {
                fprintf(stderr, "Specified file cannot be read.\n");
                return ERROR;
            }
            fclose(f);
        }
    } else {
        fprintf(stderr, "Input arguments are incorrect.\n");
        return ERROR;
    }

    if (file_type == EFILE) {
        if (flags == WRITE_END) {
            f = fopen(filename, "a");

            if (f == NULL) {
                fprintf(stderr, "Specified file cannot be read.\n");
                return ERROR;
            }
            fclose(f);
        } else if (flags == READ_END) {
            f = fopen(filename, "r");

            if (f == NULL) {
                fprintf(stderr, "Specified file cannot be read.\n");
                return ERROR;
            }
            fclose(f);
        }
    }
    return file_type;
}
