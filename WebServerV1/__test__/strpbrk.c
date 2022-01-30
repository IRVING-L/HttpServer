

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>


int main()
{
    char *s = "GET /index.html HTTP/1.1";
    printf("s len is %d\n",strlen(s));
    char tmp[24];
    int i;
    int count = 0;
    for(i = 0; i < strlen(s); ++i)
    {
        if(s[i] != ' ')
        {
            tmp[count++] = s[i]; 
        }
        else
        {
            tmp[count] = '\0';
            count = 0;
            printf("tmp:%s\n",tmp);
        }
    }
    if( i == strlen(s))
    {
        tmp[count] = '\0';
        printf("tmp:%s\n",tmp);
    }
    i = 0;
    while(1)
    {
        printf("ss\n");
        switch (i)
        {
        case 0:
            printf("0\n");
            i = 1;
            break;
        case 1:
            printf("1\n");
            i = 2;
            break;
        
        default:
            return 0;
            break;
        }
    }

    return 0;
}