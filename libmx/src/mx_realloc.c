#include "../inc/libmx.h"

void *mx_realloc(void *ptr, size_t size)
{
    if (ptr == NULL)
    {
        return malloc(size);
    }
    if (size == 0)
    {
        free(ptr);
        return NULL;
    }
    if (size <= malloc_size(ptr))
    {
        return ptr;
    }

    void *new_ptr = malloc(size);

    mx_memcpy(new_ptr, ptr, malloc_size(ptr));
    free(ptr);
    
    return new_ptr;
}
