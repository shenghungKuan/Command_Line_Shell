// Sheng-Hung Kuan
#include <stddef.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <readline/readline.h>
#include "logger.h"
#include "history.h"
#include <fcntl.h>
#include <locale.h>
#include <pwd.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>



void sigint_handler(int signo) {    
}

int readline_init(void){
    rl_variable_bind("show-all-if-ambiguous", "on");
    rl_variable_bind("colored-completion-prefix", "on");
    return 0;
}

char *readinput(void){
    char *line = NULL;
    size_t line_sz = 0;
    ssize_t result = getline(&line, &line_sz, stdin);
    if(result == -1){
        free(line);
        perror("getline");
        return NULL;
    }
    line[result - 1] = '\0';
    return line;
}



/**
 * Retrieves the next token from a string.
 *
 * Parameters:
 * - str_ptr: maintains context in the string, i.e., where the next token in the
 *   string will be. If the function returns token N, then str_ptr will be
 *   updated to point to token N+1. To initialize, declare a char * that points
 *   to the string being tokenized. The pointer will be updated after each
 *   successive call to next_token.
 *
 * - delim: the set of characters to use as delimiters
 *
 * Returns: char pointer to the next token in the string.
 */
char *next_token(char **str_ptr, const char *delim)
{
    if (*str_ptr == NULL) {
        return NULL;
    }

    size_t tok_start = strspn(*str_ptr, delim);
    size_t tok_end = strcspn(*str_ptr + tok_start, delim);

    /* Zero length token. We must be finished. */
    if (tok_end  == 0) {
        *str_ptr = NULL;
        return NULL;
    }

    /* Take note of the start of the current token. We'll return it later. */
    char *current_ptr = *str_ptr + tok_start;

    /* Shift pointer forward (to the end of the current token) */
    *str_ptr += tok_start + tok_end;

    if (**str_ptr == '\0') {
        /* If the end of the current token is also the end of the string, we
         * must be at the last token. */
        *str_ptr = NULL;
    } else {
        /* Replace the matching delimiter with a NUL character to terminate the
         * token string. */
        **str_ptr = '\0';

        /* Shift forward one character over the newly-placed NUL so that
         * next_pointer now points at the first character of the next token. */
        (*str_ptr)++;
    }

    return current_ptr;
}

void execute_pipeline(char *command)
{
    char *args[100] = {0};
    int tokens = 0;
    char *next_tok = command;
    char *curr_tok;
    
    int redir = 0;
    char *input_file[1] = {0};
    char *output_file[1] = {0};
    char *append_file[1] = {0};

    
    while ((curr_tok = next_token(&next_tok, " \t\r\n\b")) != NULL) {
            if (strncmp(curr_tok, "#", 1) == 0) {
                break;
            }

            if(redir == 1){
                // strcpy(input_file, curr_tok);
                input_file[0] = curr_tok;
                // input_file[1] = (char*) NULL;
                int file = open(input_file[0], O_RDONLY, 0666);
                if(file == -1){
                    perror("file open");
                    return;
                }
                if (dup2(file, fileno(stdin)) == -1) {
                    perror("dup2");
                    return;
                }
                redir = 4;
            }else if(redir == 2){
                // strcpy(input_file, curr_tok);
                append_file[0] = curr_tok;
                // append_file[1] = (char*) NULL;
                int file = open(append_file[0], O_CREAT | O_WRONLY | O_APPEND, 0666);
                if(file == -1){
                    perror("file open");
                    return;
                }
                if (dup2(file, fileno(stdout)) == -1) {
                    perror("dup2");
                    return;
                }
                redir = 4;
            }else if(redir == 3){
                // strcpy(input_file, curr_tok);
                output_file[0] = curr_tok;
                // output_file[1] = (char*) NULL;
                int file = open(output_file[0], O_CREAT | O_WRONLY, 0666);
                if(file == -1){
                    perror("file open");
                    return;
                }
                if (dup2(file, fileno(stdout)) == -1) {
                    perror("dup2");
                    return;
                }
                redir = 4;
            }
/*
            if(redir == 1){
                // args[tokens++] = (char*) NULL;
                LOG("file: %s\n", *args);

                int file = open(input_file[0], O_RDONLY, 0666);
                if(file == -1){
                    perror("file open");
                    return;
                }
                if (dup2(file, fileno(stdin)) == -1) {
                    perror("dup2");
                    return;
                }
                redir = 0;
            }else if(redir == 2){
                // args[tokens++] = (char*) NULL;
                LOG("file: %s\n", *args);

                int file = open(append_file[0], O_CREAT | O_WRONLY | O_APPEND, 0666);
                if(file == -1){
                    perror("file open");
                    return;
                }
                if (dup2(file, fileno(stdout)) == -1) {
                    perror("dup2");
                    return;
                }
                redir = 0;
            }else if(redir == 3){
                // args[tokens++] = (char*) NULL;
                LOG("file: %s\n", *args);

                int file = open(output_file[0], O_CREAT | O_WRONLY, 0666);
                if(file == -1){
                    perror("file open");
                    return;
                }
                if (dup2(file, fileno(stdout)) == -1) {
                    perror("dup2");
                    return;
                }
                redir = 0;
            }
*/

            if(strcmp(curr_tok, "<") == 0){
                redir = 1;
                LOG("redirection: %s\n", *args);
            }
            else if(strcmp(curr_tok, ">>") == 0){
                redir = 2;            
                LOG("redirection: %s\n", *args);
            }
            else if(strcmp(curr_tok, ">") == 0){
                redir = 3;    
                LOG("redirection: %s\n", *args);
            }



            if(strcmp(curr_tok, "|") == 0){
                args[tokens++] = (char*) NULL;

                // LOG("pipe: %s\n", *args);
                int fd[2];
                pipe(fd);

                pid_t pid = fork();
                if (pid == 0) {
                    /* Child */
                    close(fd[0]);
                    dup2(fd[1], STDOUT_FILENO);
                    execvp(args[0], args);
                    perror("exec in pipe");
                    close(fd[1]);
                    close(fileno(stdin));
                    exit(EXIT_FAILURE);
                } else {
                    /* Parent */
                    // int status;
                    // wait(&status);
                    close(fd[1]);
                    dup2(fd[0], STDIN_FILENO);
                    close(fd[0]);
                    
                }
                tokens = 0;
            }else if(redir == 0){
                args[tokens++] = curr_tok;
            }

    }
    args[tokens++] = (char*) NULL;
    
    LOG("pipe: %s\n", *args);

    execvp(args[0], args);
    close(fileno(stdin));
    exit(EXIT_FAILURE);

}

int main(void)
{
    rl_startup_hook = readline_init;
    hist_init(100);
    signal(SIGINT, sigint_handler);
    
    // NOTE: "scripting" mode really just means reading from stdin
    //       and NOT printing a whole bunch of junk (including the prompt)

    
    bool emoji = true;
    int cnum = 0;
    while (true) {
        char *command;// = malloc(256 * sizeof(char));
        cnum += 1;
        char *prompt = NULL;
        char cwd[40];
        char *username = getenv("USER");
        char hostname[40];
        prompt = malloc(1048 * sizeof(char));
        getcwd(cwd, sizeof(cwd));
        gethostname(hostname, sizeof(hostname));

        // tokenize cwd
        char *cwds[20] = {0};
        int tnum = 0;
        char *next_t = cwd;
        char *curr_t;
        while ((curr_t = next_token(&next_t, "/")) != NULL) {
            cwds[tnum++] = curr_t;
        }

        if(emoji){
            sprintf(prompt, "😍< %d > [%s@%s:~/%s]$ ", cnum, username, hostname, cwds[tnum - 1]);
        }else{
            sprintf(prompt, "💩< %d > [%s@%s:~/%s]$ ", cnum, username, hostname, cwds[tnum - 1]);
        }
        

        if(isatty(STDIN_FILENO)){
            command = readline(prompt);
        }else{
            command = readinput();
        }

        free(prompt);

        if(command == NULL){
            free(command);
            break;
        }

        if(strlen(command) == 0){
            cnum--;
            free(command);
            continue;
        }
        
        LOG("Input command: %s\n", command);

        if (*command == '!') {
            char *command_instead = command;
            command_instead++;
            if(isdigit(*command_instead)){
                int val = strtol(command_instead, &command_instead, 10);
                if(hist_search_cnum(val) == NULL){
                    free(command);
                    continue;
                }
                // command--;
                strcpy(command, hist_search_cnum(val));
            }else if(*command_instead == '!'){
                if(hist_search_cnum(hist_last_cnum()) == NULL){
                    free(command);
                    continue;
                }
                // command--;
                strcpy(command, hist_search_cnum(hist_last_cnum()));
            }else{
                if(hist_search_prefix(command_instead) == NULL){
                    free(command);
                    continue;
                }
                // command--;
                strcpy(command, hist_search_prefix(command_instead));
            }
        }

        hist_add(command);

        char *args[20] = {0};
        int tokens = 0;
        char *next_tok = command;
        char *curr_tok;
        char *s = strchr(command, '|');
        char *r = strchr(command, '>');
        char *l = strchr(command, '<');
        if(s != NULL || r != NULL || l != NULL){
            
            pid_t pid = fork();
            if (pid == 0) {
                execute_pipeline(command);
            } else {
                int status;
                wait(&status);
            }
            // free(command);
            continue;
        }

        while ((curr_tok = next_token(&next_tok, " \t\r\n\b")) != NULL) {
            if (strncmp(curr_tok, "#", 1) == 0) {
                break;
            }
            args[tokens++] = curr_tok;
        }

        args[tokens] = (char *) NULL;
        
        if (args[0] == (char *) NULL) {
            free(command);
            continue;
        }

        if (strcmp(args[0], "exit") == 0) {
            fprintf(stderr, "Have a great day, bye!\n");
            free(command);
            break;
        }

        // chdir system call
        if (strcmp(args[0], "cd") == 0) {
            if(args[1] == NULL){
                chdir(getenv("HOME"));
            }else{
                chdir(args[1]);
            }
            emoji = true;
            free(command);
            continue;
        }

        if (strcmp(args[0], "history") == 0) {
            hist_print();
            emoji = true;
            free(command);
            continue;
        }
        // free(command);



        
        // TODO:
        // * history
        // * ctrl+c not dying mode
        // * logged in user
        // * command line editing (autocomplete) => use readline library for this
        // * more builtin functions (like cd)
        // * redirect, piping
        // * protecting ourselves from storms
        
        pid_t child = fork();
        if (child == -1) {
            perror("fork");
        } else if (child == 0) { // child
            execvp(args[0], args); // replaces the child process with a completely different
            perror("exec outside");
            close(fileno(stdin));
            return EXIT_FAILURE;
        } else { // parent
            // We should wait for the child to finish executing
            int status;
            wait(&status);
            if(status){
                emoji = false;
            }else{
                emoji = true;
            }
        }
        
    }
    hist_destroy();
    return 0;
}
