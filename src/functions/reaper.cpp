#include "includes.hpp"

// waits for children processes without blocking
// SIGKILLs if child process has not exited yet
// returns 1 if list is empty, 0 otherwise
int reap_child_processes(std::list<pid_t> &list)
{
	std::list<pid_t>::iterator it = list.begin();
	int ret;

	while (it != list.end())
	{
		ret = waitpid(*it, NULL, WNOHANG);
		if (ret)
			it = list.erase(it);
		else
		{
			kill(*it, SIGKILL);
			++it;
		}
	}
	if (list.empty())
		return (1);
	return (0);
}

void reaper(FdManager &table)
{
	(void)table;
	// time-out requests

	// reap child processes: move reaper's list to ACgiRH
	reap_child_processes(ACgiRH::child_processes);
}
