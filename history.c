// Sheng-Hung Kuan
#include <stddef.h>
#include "elist.h"
#include "history.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct elist *elists;
int cnum = 0;
int bound;

void hist_init(unsigned int limit)
{
    // TODO: set up history data structures, with 'limit' being the maximum
    // number of entries maintained.
    elists = elist_create(limit, 256 * sizeof(char));
    bound = limit;
}

void hist_destroy(void)
{
    elist_destroy(elists);
}

void hist_add(const char *cmd)
{
    cnum += 1;
    if(cnum > bound){// cnum = 11, bound = 10, command = 1~10 ; cum = 20, 20 - 10 - 1 = 9
        if(elist_remove(elists, 0) == -1){
            perror("remove");
            return;
        }
    }
    char *command = malloc(256 * sizeof(char));
    strcpy(command, cmd);
    elist_add(elists, command);
    free(command);
}

void hist_print(void)
{
    if(cnum > bound){// cnum = 11, bound = 10
        for(int i = 0; i < bound; i++){
            char *command;
            command = elist_get(elists, i);
            if(command == NULL){
                return;
            }
            printf("%d %s\n", cnum + i - bound + 1, command);
        }
    }else{
        for(int i = 0; i < cnum; i++){
            char *command;
            command = elist_get(elists, i);
            if(command == NULL){
                return;
            }
            printf("%d %s\n", i + 1, command);
        }
    }
    fflush(stdout);
}

const char *hist_search_prefix(char *prefix)
{
    // TODO: Retrieves the most recent command starting with 'prefix', or NULL
    // if no match found.
    if(cnum > bound){
        for(int i = bound - 1; i >= 0; i--){
            char *element;
            element = elist_get(elists, i);
            if(element == NULL){
                continue;
            }
            if(strncmp(element, prefix, strlen(prefix)) == 0){
                return element;
            }
        }
    }else{
        for(int i = cnum - 1; i >= 0; i--){
            char *element;
            element = elist_get(elists, i);
            if(element == NULL){
                continue;
            }
            if(strncmp(element, prefix, strlen(prefix)) == 0){
                return element;
            }
        }
    }
    return NULL;
}

const char *hist_search_cnum(int command_number)
{
    // TODO: Retrieves a particular command number. Return NULL if no match
    // found.
    if(command_number > cnum){
        return NULL;
    }
    if(command_number > bound){// cnum = 422, bound = 100, command_number = 400, index = 78
        return elist_get(elists, bound - cnum + command_number - 1);
    }
    if(command_number < cnum - bound){
        return NULL;
    }
    return elist_get(elists, command_number - 1);
}

unsigned int hist_last_cnum(void)
{
    // TODO: Retrieve the most recent command number.
    return cnum;
}
