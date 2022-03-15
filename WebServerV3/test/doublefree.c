#include <arpa/inet.h> //sockaddr_in
#include <sys/uio.h>   //readv/writev
#include <sys/types.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{
    int fd = open("getcwdtest.c", O_RDONLY);

    close(fd);
    close(fd);
    return 0;
}