/*************************************************************************
  > File Name: manager.c
  > Author: leaker
  > Mail: xiao13149920@foxmail.com 
  > Created Time: Thu 18 Mar 2021 10:24:54 AM CST
 ************************************************************************/

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<errno.h>
#include<sys/types.h>
#include<unistd.h>
#include<signal.h>

void syserror(const char* const str);
void sig_rtmin_handler(int signum, siginfo_t* siginfo, void* context);

int main(int ac, char* av[])
{
	struct sigaction act;
	memset(&act, 0, sizeof(act));
	act.sa_sigaction = sig_rtmin_handler; // register signal function handler
	act.sa_flags = SA_SIGINFO; // for real-time signal

	if (sigaction(SIGRTMIN, &act, NULL) == -1) {
		syserror("sigaction");
	} else {
		printf("regist sigrtmin success!\n");
	}

	// do important work
	while (1) {
		sleep(10);
	}

	return 0;
}

void sig_rtmin_handler(int signum, siginfo_t* siginfo, void* context)
{
#if 1
	switch (siginfo->si_value.sival_int) {
		case '4':
			write(2, "Four runs scored.\n", 18);
			break;

		case '6':
			write(2, "Six runs scored.\n", 17);
			break;

		case 'W':
			write(2, "A wicket has fallen.\n", 21);
			break;

		default:
			write(2, "Unclear communication\n", 22);
			break;
	}
#else
	//fprintf(stdout, "siginfo.sival_ptr is: %s\n", (char*)siginfo->si_value.sival_ptr);
	char* val = siginfo->si_value.sival_ptr;
	fprintf(stdout, "siginfo.sival_ptr is: %s\n", val);
#endif
}

void syserror(const char* const str)
{
	perror(str);
	exit(EXIT_FAILURE);
}
