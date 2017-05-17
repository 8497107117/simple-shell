#include <vector>
#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <string>

#include "command.h"

using namespace std;

class Job {
	private:
		pid_t pgid;
		vector<pid_t> pids;
		string commands;
	public:
		Job(pid_t pgid, vector<pid_t> pids, vector<Command> commands);
		pid_t getPgid();
		vector<pid_t> getPids();
		string getCommands();
};

class Jobs {
	private:
		vector<Job> jobs;
	public:
		Jobs();
		Job getJob(int jobId);
		int getJobsSize();
		int addJob(pid_t pgid, vector<pid_t> pids, vector<Command> commands);
		void removeJob(int jobId);
		void showJobs();
};
