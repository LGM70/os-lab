#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

#define BUFFER_LEN 50
#define MAX_COUNT 5

int pipefd[2]; // pipe
int pid[2]; // process id
int RcvMsgCount[2]; // message received count
int SendMegCount; // message sent count

// child process 1

void process1_sig_handler(int sig_no) {
	if (sig_no == SIGUSR1) {
		close(pipefd[0]); // close the pipe

		printf("Child Process 1 Receives %d Messages.\n", RcvMsgCount[0]);
		printf("Child Process 1 is Killed by Parent!\n");
		exit(0);
	}
	// ignore other signals
}

void process1() {
	close(pipefd[1]); // close the write end

	signal(SIGINT, process1_sig_handler); // set SIGINT
	signal(SIGUSR1, process1_sig_handler); // set SIGUSR1/2
	signal(SIGUSR2, process1_sig_handler);

	char buffer[BUFFER_LEN];
	while (1) {
		memset(buffer, 0, sizeof(buffer)); // empty the buffer
		read(pipefd[0], buffer, sizeof(buffer) / sizeof(char)); // read from pipe
		printf("%s", buffer);
		RcvMsgCount[0]++;
	}
}

// child process 2

void process2_sig_handler(int sig_no) {
	if (sig_no == SIGUSR2) {
		close(pipefd[0]); // close the pipe

		printf("Child Process 2 Receives %d Messages.\n", RcvMsgCount[1]);
		printf("Child Process 2 is Killed by Parent!\n");
		exit(0);
	}
	// ignore other signals
}
void process2() {
	close(pipefd[1]); // close the write end

	signal(SIGINT, process2_sig_handler); // set SIGINT
	signal(SIGUSR1, process2_sig_handler); // set SIGUSR1/2
	signal(SIGUSR2, process2_sig_handler);
	
	char buffer[BUFFER_LEN];
	while (1) {
		memset(buffer, 0, sizeof(buffer)); // empty the buffer
		read(pipefd[0], buffer, sizeof(buffer) / sizeof(char)); // read from pipe
		printf("%s", buffer);
		RcvMsgCount[1]++;
	}
}

// parent process

void main_sig_handler(int sig_no) {
	if (sig_no == SIGINT) {
		kill(pid[0], SIGUSR1);
		kill(pid[1], SIGUSR2);

		int status;
		wait(&status);
		wait(&status); // wait for child processes to exit
		
		close(pipefd[1]); // close the pipe
		
		printf("Parent Process Sends %d Messages.\n", SendMegCount);
		printf("Parent Process is Killed!\n");
		exit(0);
	}
}

int main() {
	pipe(pipefd); // creat the pipe

	signal(SIGINT, main_sig_handler); // set SIGINT

	if ((pid[0] = fork()) == 0) { // the first child process
		process1();
		exit(0);
	}
	if ((pid[1] = fork()) == 0) { // the second child process
		process2();
		exit(0);
	}

	close(pipefd[0]); // close the read end

	int count = 1;
	char str[BUFFER_LEN];
	while (count <= MAX_COUNT) {
		memset(str, 0, sizeof(str) / sizeof(char)); // empty the buffer
		sprintf(str, "I send you %d times.\n", count++);
		write(pipefd[1], str, strlen(str)); // write into pipe
		SendMegCount++;
		sleep(1);
	}

	kill(getpid(), SIGINT); // normal exit

	return 0;
}