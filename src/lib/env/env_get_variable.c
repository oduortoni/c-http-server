#include "header.h"

const char *env_get_variable(const char *var_name)
{
    const char *value = getenv(var_name);
    if (value)
    {
        return value;
    }
    else
    {
        return NULL;
    }
}
