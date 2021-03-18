# 1 Signals
Signals are notifications delivered asynchronously to a process by the kernel. Signals are grouped in two categories:

+ __standard signal__[1: SIGHUP, 31: SIGSYS]
+ __real time signal__[34: SIGRTMIN ... 49: SIGRTMIN+15, 50: SIGRTMAX-14 ... 64: SIGRTMAX]

Execute __`kill -l`__ to show all signals table. 
In this post, we will look at POSIX real-time signals in linux

# 2. Characteristics of POSIX real-time signals
The linux kernel support real-time signals range defined by macros __SIGRTMIN__ and __SIGRTMAX__.
The default action for a real-time signal is to __terminate__ the receiving __process__
Real-time signals are __quequed__ to the receiving process. This applies even when the same __real-time signal is received multiple times__. In Contrast, if a __standard signal__ is blocked and multiple instance of it are to be delivered to a process, __only one__ becomes __pending__ and the __rest are discarded__
If multiple real-time signals are queued to a process, they are delivered in the __ascending order__ of their signal numbers. That is the __lower real-time signal first__. 

If a real-time signal is sent using the __`sigqueue`__ function, a __value or a pointer__ can be sent along with the signal use the __`union  sigval`__. This value or the pointer can be retrieved by the receiving process from the second parameter of the real-time signal handler, the pointer to __`siginfo_t`__. The value or the pointer is stored in __`si_value`__ or __`si_ptr`__ members respecitively.

# 3. Setting the signal action
The signal hander is installed with the __`sigaction`__ system call

```
#include<signal.h>

int sigaction(int signum, const struct sigaction* act, struct sigaction* oldact)
```

The struct of __`signaction`__ is:

```
struct sigaction {
	void (*sa_handler)(int); // handler function for standard signal
	void (*sa_sigaction)(int, siginfo_t* , void* ); // hander function for real-time signal
	sigset_t sa_mask;
	int sa_flags;
	void (*sa_restorer)(void)
};
```

If __`SA_SIGINFO`__ is specified in __`sa_flags`__, the pointer to function, __`sa_sigaction`__ is the signal handler to be set in the __`sigaction`__ call. When the siganl comes, the signal handler is called with pointer to the __`siginfo_t`__ structure as the second argument. The signal handler can get information about the signal from the __`siginfo_t`__ structure, The __`siginfo_t`__  structure is:
```
siginfo_t {
	int si_signo;	// Signal number
	int si_error;	// An errno value
	int si_code;	// singal code
	int si_trapno;	// trap number that caused hardware-generated signal(unused on most architectures)
	pid_t si_pid;	// sending process ID
	uid_t si_uid;	// real user id of sending process
	int si_status;	// exit value or signal
	clock_t  si_utime;     /* User time consumed */
	clock_t  si_stime;     /* System time consumed */
	sigval_t si_value;     /* Signal value */
	int      si_int;       /* POSIX.1b signal */
	void    *si_ptr;       /* POSIX.1b signal */
	int      si_overrun;   /* Timer overrun count;
	POSIX.1b timers */
	int      si_timerid;   /* Timer ID; POSIX.1b timers */
	void    *si_addr;      /* Memory location which caused fault */
	long     si_band;      /* Band event (was int in
	glibc 2.3.2 and earlier) */
	int      si_fd;        /* File descriptor */
	short    si_addr_lsb;  /* Least significant bit of address 
	(since Linux 2.6.32) */ 
	void    *si_call_addr; /* Address of system call instruction
	(since Linux 3.5) */
	int      si_syscall;   /* Number of attempted system call
	(since Linux 3.5) */
	unsigned int si_arch;  /* Architecture of attempted system call
	(since Linux 3.5) */
}          
```

The member __`si_code`__ specifies the reason for the signal. Some of the values of __`si_code`__ are:

`si_code`|reason for the singal
---------|-----------------
`SI_USER`|sent by __`kill`__ system call
`SI_KERNEL`|sent by the __kernel__
`SI_QUEUE`| sent by __`sigqueue`__
`SI_TIMER`|expiration of a __POSIX timer__
`SI_ASYNCIO`|asynchronous I/O completed
`SI_MESGQ`|Message arrival on an empty message queue, signal because of __`mq_notify`__ registration signal reasons

An important characteristic of real-time signals is to communicate information and __not just the signal id__. It is the capability to pass an integer or a pointer, which is elaborated below int the description of the __`sigqueue`__ function. The signal handler can get more information about the signal from this __integer or pointer__.

# 4 system calls
## 4.1 sigqueue

```
#include<signal.h>

union sigval {
	int sival_int;
	void* sival_ptr;
};

int sigqueue(pid_t pid, int sig, const union sigval value);
```

The __`sigqueue`__ function sends the signal __sig__ to the process identified by __pid__ along with data __value__. The __value__ is a union of an integer and a pointer, If the __receiving process__  has __installed__  a singal handler using the __`SA_SIGINFO`__ flag, it can get the value using the __`si_value`__ field of the __`siginfo_t`__ structure.

## 4.2 sigwaitinfo
```
#include<signal.h>

int sigwaitinfo(const sigset_t* set, siginfo_t* info);
```

__`sigwaitinfo`__ is an improvement on the __`sigwait`__ system call. Like __`sigwait`__, it blocks till a signal specified in the set becomes pending. However, __`sigwaitinfo`__ has a second parameter, a pointer to typedef __`siginfo_t`__ i, in which data about the signal is returned. Like, __`sigwait`__, the signals specified int the __set should be blocked__ before making the call. On success, __sigwaitinfo`__ returns the accepted signal, a value greater than zero, and the signal is removed from the set of pending signals for the thread. In case of failure, -1 is returned and errno is set accordingly.

## 4.3 sigtimedwait
```
#include<signal.h>

struct timespec {
	long tv_sec; // seconds
	long tv_nsec; // nanoseconds
}

int sigtimedwait(const sigset_t* set, siginfo_t* info, const struct timespec* timeout);
```

__`sigtimedwait`__ is similar to __`sigwaitinfo`__, except that the maximum time of wait is as specified in the structrue pointed by the third parameter: __timeout__. if timeout occurs and no signal is pending, __`sigtimedwait`__ return __-1__ with errno set to __EAGIN__

# 5 An example Using real-time signal


