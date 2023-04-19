#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

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
    printf("Welcome to BESTSH SHELL\n");
    printf("It's the BEST SHELL\n");
    printf("Also, have a great day!\n");
    
    // NOTE: "scripting" mode really just means reading from stdin
    //       and NOT printing a whole bunch of junk (including the prompt)

    while (true) {
        printf("$ ");
        char buf[128];
        if (fgets(buf, 128, stdin) == NULL) {
            printf("it is time for us to quit.\n");
            break;
        }
        printf("We are going to run: %s", buf);

        char *args[20];
        int tokens = 0;
        char *next_tok = buf;
        char *curr_tok;
        while ((curr_tok = next_token(&next_tok, " \t\r\n")) != NULL) {
            args[tokens] = curr_tok;
            printf("Token %02d: '%s'\n", tokens++, curr_tok);
        }
        args[tokens] = (char *) NULL;
        
        if (args[0] == (char *) NULL) {
            continue;
        }

        if (strcmp(args[0], "exit") == 0) {
            printf("Have a great day, bye!\n");
            break;
        }

	// chdir system call
	if (strcmp(args[0], "cd") == 0) {
		chdir(args[1]);
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
            return EXIT_FAILURE;
        } else {
            // We should wait for the child to finish executing
            int status;
            wait(&status);
        }
    }
}
