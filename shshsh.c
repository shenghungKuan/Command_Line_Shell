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

struct command_line {
    char **tokens;
    bool stdout_pipe;
    char *stdout_file;
};

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

void execute_pipeline(struct command_line *cmds)
{
    while(cmds->stdout_pipe){
        int fd[2];
        pipe(fd);

        pid_t pid = fork();
        if (pid == 0) {
            /* Child */
            close(fd[0]);
            dup2(fd[1], STDOUT_FILENO);
            execvp(cmds->tokens[0], cmds->tokens);
            close(fd[1]);
        } else {
            /* Parent */
            close(fd[1]);
            dup2(fd[0], STDIN_FILENO);
            close(fd[0]);
        }
        cmds += 1;
    }

    if(cmds->stdout_file != NULL){
        int fd = open(cmds->stdout_file, O_CREAT | O_WRONLY, 0666);
        if (fd == -1) {
            perror("open");
            return;
        }
        if (dup2(fd, fileno(stdout)) == -1) {
            perror("dup2");
            return;
        }
    }
    
    execvp(cmds->tokens[0], cmds->tokens);
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
    signal(SIGINT, sigint_handler);
    
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
        }else{
            command = readinput();
        }

        free(prompt);

        if(command == NULL){
            break;
        }

        if(strlen(command) == 0){
            cnum--;
            continue;
        }
        
        LOG("Input command: %s\n", command);

        if (*command == '!') {
            command++;
            if(isdigit(*command)){
                int val = strtol(command, &command, 10);
                if(hist_search_cnum(val) == NULL){
                    continue;
                }
                command--;
                strcpy(command, hist_search_cnum(val));
            }else if(*command == '!'){
                if(hist_search_cnum(hist_last_cnum()) == NULL){
                    continue;
                }
                command--;
                strcpy(command, hist_search_cnum(hist_last_cnum()));
            }else{
                if(hist_search_prefix(command) == NULL){
                    continue;
                }
                strcpy(command, hist_search_prefix(command));
            }
        }

        hist_add(command);

        char *args[20] = {0};
        int tokens = 0;
        int prev_tokens = 0;
        char *next_tok = command;
        char *curr_tok;
        int pipe_num = 0;
        struct command_line cmd[20] = {0};
        // if((char *s = strchr(command, '|')) != NULL)

        while ((curr_tok = next_token(&next_tok, " \t\r\n\b")) != NULL) {
            if (strncmp(curr_tok, "#", 1) == 0) {
                break;
            }
            // if(strcmp(curr_tok, "|") == 0){
            //     args[tokens] = (char *) NULL;
            //     cmd[pipe_num].stdout_pipe = true;
            //     cmd[pipe_num++].tokens = args + prev_tokens;
            //     prev_tokens = tokens;
            //     tokens++;
            //     continue;
            // }
            args[tokens++] = curr_tok;
        }

        if(pipe_num > 0){
            cmd[pipe_num].stdout_pipe = false;
            cmd[pipe_num++].tokens = args + prev_tokens;
            execute_pipeline(cmd);
            continue;
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
            if(args[1] == NULL){
                chdir(getenv("HOME"));
            }
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
        } else if (child == 0) { // child
            execvp(args[0], args); // replaces the child process with a completely different
            perror("exec");
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
    free(command);
    hist_destroy();
    return 0;
}
