
#include <stddef.h>
#include <sys/types.h>
#include <unistd.h>


int _write (int fd, const void *buf, size_t count)
{
    return 0;
}

int _read (int fd, void * ptr, size_t len)
{
    return 0;
}

void * _sbrk (int nbytes)
{
    return 0;
}

void _exit(int stat)
{
}

int _getpid()
{
    return 0;
}

int _kill (int pid, int sig)
{
    return 0;
}

int _isatty(int fildes)
{
    return 0;
}

int _lseek(int fd, off_t offset, int whence)
{
    return 0;
}

int _close(int fd)
{
    return 0;
}

int _fstat (int file, struct stat *st)
{
    return 0;
}

//  FIN
