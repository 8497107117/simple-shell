#include "job.h"

Job::Job(pid_t pgid, vector<pid_t> pids, vector<Command> commands) {
	this->pgid = pgid;
	this->pids = pids;
	string cmd("");
	for(unsigned int i = 0;i < commands.size();i++) {
		char **argv = commands[i].genArgs();
		int type = commands[i].type();
		cmd += type == 0 ? ' ' : type == 1 ? '|' : type == 2 ? '<' : '>';
		for(int j = 0;argv[j];j++) {
			cmd = cmd + ' ' + argv[j];
		}
		cmd += ' ';
	}
	this->commands = cmd;
}

pid_t Job::getPgid() {
	return this->pgid;
}

vector<pid_t> Job::getPids() {
	return this->pids;
}

string Job::getCommands() {
	return this->commands;
}

Jobs::Jobs() {
}

Job Jobs::getJob(int jobId) {
	if(jobId == 0) { return this->jobs[this->jobs.size() - 1]; }
	else { return this->jobs[jobId - 1]; }
}

int Jobs::getJobsSize() {
	return this->jobs.size();
}

int Jobs::addJob(pid_t pgid, vector<pid_t> pids, vector<Command> commands) {
	this->jobs.push_back(Job(pgid, pids, commands));
	return this->jobs.size();
}

void Jobs::removeJob(int jobId) {
	this->jobs.erase(this->jobs.begin() + jobId - 1);
}

void Jobs::showJobs() {
	for(unsigned int i = 0;i < this->jobs.size();i++) {
		printf("[%d]%s  stopped     %s\n", i + 1, " ", this->jobs[i].getCommands().c_str());
	}
}
