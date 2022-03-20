#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <iostream>

int main()
{
	pid_t pid = fork();
	if (pid == 0)
	{
		char *argv[3];
		argv[0] = "/usr/bin/python3";
		argv[1] = "sleeper.py";
		argv[2] = NULL;

		execve(argv[0], argv, NULL);
	}

	int ret = 0;
	while (ret == 0)
	{
		ret = waitpid(pid, NULL, WNOHANG);
		std::cout << "waipid returned 0" << std::endl;
		usleep(50000);
	}
	std::cout << "waipid returned " << ret << std::endl;
}