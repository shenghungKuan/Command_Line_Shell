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
    if(cnum > bound){
        elist_remove(elists, cnum - bound);
    }
    char *command;
    *command = *cmd;
    elist_add(elists, command);
}

void hist_print(void)
{
    for(int i = cnum - bound; i < cnum; i++){
        char *command;
        command = elist_get(elists, i);
        if(command == NULL){
            return;
        }
        printf("%d %s", i + bound, command);
    }
}

const char *hist_search_prefix(char *prefix)
{
    // TODO: Retrieves the most recent command starting with 'prefix', or NULL
    // if no match found.
    for(int i = 0; i < bound; i++){
        if(strcmp(elist_get(elists, i), prefix) == 0){
            return elist_get(elists, i);
        }
    }
    return NULL;
}

const char *hist_search_cnum(int command_number)
{
    // TODO: Retrieves a particular command number. Return NULL if no match
    // found.
    if(command_number > bound){
        return elist_get(elists, command_number - bound);
    }
    return elist_get(elists, command_number);
}

unsigned int hist_last_cnum(void)
{
    // TODO: Retrieve the most recent command number.
    return cnum;
}
