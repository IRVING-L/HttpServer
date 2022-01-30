#include <signal.h>
#include <stdio.h>
#include <unistd.h>

void ouch(int sig)
{
	printf("i got signal %d\n", sig);
}


int main()
{
	(void) signal(SIGINT, ouch);

	while(1)
	{
		printf("hello world\n");
		sleep(1);
	}
	return 0;
}
