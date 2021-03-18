/*************************************************************************
  > File Name: scorer.c
  > Author: leaker
  > Mail: xiao13149920@foxmail.com 
  > Created Time: Thu 18 Mar 2021 10:46:09 AM CST
 ************************************************************************/

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/types.h>
#include<errno.h>
#include<unistd.h>
#include<signal.h>

void syserror(const char* const str);
int get_option(void);

int main(int ac, char* av[])
{
	int option;
	if (ac != 2) {
		printf("Usage: score manager-pid\n");

		exit(1);
	}

	pid_t manager_pid = atoi(av[1]);
	while (1) {
		printf("Action: \n");
		printf("\t1\tBatsman hit 4\n");
		printf("\t2\tBatsman hit 6\n");
		printf("\t3\tWicket falls\n\n");
		printf("\t0\tQuit\n\n\n");
		scanf("%d", &option);
		if (option == 0) {
			break;
		}

#if 1
		if (option >=1 && option <= 3) {
			union sigval sigval;
			switch (option) {
				case 1:
					sigval.sival_int = '4';
					break;

				case 2:
					sigval.sival_int = '6';
					break;

				case 3:
					sigval.sival_int = 'W';
					break;
			}

			if (sigqueue(manager_pid, SIGRTMIN, sigval) == -1) {
				syserror("sigqueue");
			}
		} else {
			printf("Illegal option, try again\n\n");
		}
#else
		union sigval* sigval = malloc(sizeof(union sigval));
		sigval->sival_ptr = (void*)malloc(10);
		strncpy(sigval->sival_ptr, option, 10);
		*((char*)sigval->sival_ptr+9) = 0;
		if (sigqueue(manager_pid, SIGRTMIN, *sigval) == -1) {
			syserror("sigqueue");
		}
#endif
	}

	return 0;
}

void syserror(const char* const str)
{
	perror(str);
	exit(EXIT_FAILURE);
}
