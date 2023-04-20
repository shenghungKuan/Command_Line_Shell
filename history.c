#include <stddef.h>
#include "elist.h"
#include "history.h"

struct elist *elists;

void hist_init(unsigned int limit)
{
    // TODO: set up history data structures, with 'limit' being the maximum
    // number of entries maintained.
    elists = elist_create(limit, 256 * sizeof(char));
}

void hist_destroy(void)
{
    elist_destroy(elists);
}

void hist_add(const char *cmd)
{

}

void hist_print(void)
{

}

const char *hist_search_prefix(char *prefix)
{
    // TODO: Retrieves the most recent command starting with 'prefix', or NULL
    // if no match found.
    return NULL;
}

const char *hist_search_cnum(int command_number)
{
    // TODO: Retrieves a particular command number. Return NULL if no match
    // found.
    return NULL;
}

unsigned int hist_last_cnum(void)
{
    // TODO: Retrieve the most recent command number.
    return 0;
}
