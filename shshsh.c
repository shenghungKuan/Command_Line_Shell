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


int main(void)
{
    rl_startup_hook = readline_init;
    hist_init(100);
    
    // NOTE: "scripting" mode really just means reading from stdin
    //       and NOT printing a whole bunch of junk (including the prompt)

    char *command = malloc(256 * sizeof(char));
    bool emoji = true;
    int cnum = 0;
    while (true) {
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
            hist_add(command);
        }else{
            command = readinput();
        }

        free(prompt);

        if(command == NULL){
            break;
        }
        
        LOG("Input command: %s\n", command);

        char *args[20] = {0};
        int tokens = 0;
        char *next_tok = command;
        char *curr_tok;
        while ((curr_tok = next_token(&next_tok, " \t\r\n")) != NULL) {
            if (strcmp(curr_tok, "#") == 0) {
                break;
            }
            args[tokens++] = curr_tok;
        }
        args[tokens] = (char *) NULL;
        
        if (args[0] == (char *) NULL) {
            continue;
        }

        if (strcmp(args[0], "exit") == 0) {
            fprintf(stderr, "Have a great day, bye!\n");
            break;
        }

        // chdir system call
        if (strcmp(args[0], "cd") == 0) {
            chdir(args[1]);
            emoji = true;
            continue;
        }

        if (strcmp(args[0], "history") == 0) {
            hist_print();
            emoji = true;
            continue;
        }
        
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
        } else if (child == 0) {
            execvp(args[0], args); // replaces the child process with a completely different
            perror("exec");
            close(fileno(stdin));
            return EXIT_FAILURE;
        } else {
            // We should wait for the child to finish executing
            int status;
            wait(&status);
            if(status){
                emoji = false;
            }else{
                emoji = true;
            }
        }
        free(command);
    }
    hist_destroy();
    return 0;
}
