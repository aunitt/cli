
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#define UNUSED(x) ((x) = (x))

ssize_t __attribute__((weak)) _write(int fd, const void *buf, size_t count)
{
    UNUSED(fd);
    UNUSED(buf);
    UNUSED(count);
    return 0;
}

ssize_t  __attribute__((weak)) _read(int fd, void * ptr, size_t len)
{
    UNUSED(fd);
    UNUSED(ptr);
    UNUSED(len);
    return 0;
}

void *__attribute__((weak)) _sbrk(int nbytes)
{
    UNUSED(nbytes);
    return 0;
}

void __attribute__((weak)) _exit(int stat)
{
    UNUSED(stat);
    abort();
}

int __attribute__((weak)) _getpid()
{
    return 0;
}

int __attribute__((weak)) _kill(int pid, int sig)
{
    UNUSED(pid);
    UNUSED(sig);
    return 0;
}

int __attribute__((weak)) _isatty(int fildes)
{
    UNUSED(fildes);
    return 0;
}

off_t __attribute__((weak)) _lseek(int fd, off_t offset, int whence)
{
    UNUSED(fd);
    UNUSED(offset);
    UNUSED(whence);
    return 0;
}

int __attribute__((weak)) _close(int fd)
{
    UNUSED(fd);
    return 0;
}

int __attribute__((weak)) _fstat(int file, struct stat *st)
{
    UNUSED(file);
    UNUSED(st);
    return 0;
}

//  FIN
