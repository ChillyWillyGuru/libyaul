#include <sys/stat.h>
#include <sys/reent.h>

int
_fstat_r(struct _reent *r __attribute__ ((unused)),
    int file __attribute__ ((unused)),
    struct stat *st __attribute__ ((unused)))
{
        return 0;
}
