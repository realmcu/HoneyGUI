#include "map_platform.h"
#include <stddef.h>

static const map_platform_ops_t *g_platform_ops = NULL;

int map_platform_register(const map_platform_ops_t *ops)
{
    if (ops == NULL)
    {
        return -1;
    }

    g_platform_ops = ops;
    return 0;
}

const map_platform_ops_t *map_platform_get(void)
{
    return g_platform_ops;
}

bool map_platform_is_registered(void)
{
    return g_platform_ops != NULL;
}
