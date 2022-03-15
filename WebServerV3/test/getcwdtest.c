#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>


int main()
{
    char currPath[108];
    getcwd(currPath, sizeof(currPath));
    printf("%s\n", currPath);
    strcat(currPath,"getcwdtest.c");
    printf("%s\n", currPath);
    return 0;
}