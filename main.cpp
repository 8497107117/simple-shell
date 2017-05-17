#include "main.h"

using namespace std;

void reaper(int sig) {
	int status;
	waitpid(-1, &status, WNOHANG);
}

void prompt() {
	cout << "＼(・ω・＼)SAN値！(／・ω・)／ピンチ！ ";
}

void init() {
	signal(SIGCHLD, reaper);
	signal(SIGINT, SIG_IGN);
	signal(SIGTSTP, SIG_IGN);
	signal(SIGQUIT, SIG_DFL);
	signal(SIGTTIN, SIG_IGN);
	signal(SIGTTOU, SIG_IGN);
}

void executeSingleCommand(char **argv, int in, int out, int index, vector<UnNamedPipe> pipeCtrl, vector<int> &pids,
		char *inputFile, char *outputFile) {
	pid_t pid;
	pids.push_back(pid = fork());

	if(pid < 0) {
		cout << "fork error" << endl;
		exit(1);
	}
	/* child */
	else if(pid == 0) {
		/* input & output */
		if(in != STDIN_FILENO) { dup2(in, STDIN_FILENO); }
		else if(inputFile) {
			int input = open(inputFile, O_RDONLY);
			if(input == -1) {
				cout << "read file error" << endl;
				exit(1);
			}
			dup2(input, STDIN_FILENO);
			close(input);
		}
		if(outputFile) {
			int output = open(outputFile, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
			if(output == -1) {
				cout << "write file error" << endl;
				exit(1);
			}
			dup2(output, STDOUT_FILENO);
			close(output);
		}
		else if(out != STDOUT_FILENO) { dup2(out, STDOUT_FILENO); }
		/* close pipe */
		int pipeSize = pipeCtrl.size();
		for(int i = 0;i < pipeSize;i++) {
			pipeCtrl[i].closeReadPipe();
			pipeCtrl[i].closeWritePipe();
		}
		/* exec */
		if(!strcmp(argv[0], "export") || !strcmp(argv[0], "unset")) { execlp("printenv", "printenv", NULL); }
		else { execvp(argv[0], argv); }
		printf("Unknown command: [%s].\n", argv[0]);
		exit(1);
	}
}

bool execute(vector<Command> commands) {
	int commandsSize = commands.size(), pipeSize = 0, pgid = 0;
	for(int i = 0;i < commandsSize;i++) {
		if(commands[i].type() == 1) { pipeSize++; }
	};
	vector<int> pids;
	vector<UnNamedPipe> pipeCtrl(pipeSize, UnNamedPipe());
	for(int i = 0;i < pipeSize;i++) { pipeCtrl[i].createPipe(); };

	/* Each command */
	int index = 0;
	for(int i = 0;i < commandsSize;i++) {
		int in, out;
		int nextType = i < commandsSize - 1 ? commands[i+1].type() : -1,
			afterNextType = i < commandsSize - 2 ? commands[i+2].type() : -1;
		char **argv = commands[i].genArgs(),
			 **nextArgv = i < commandsSize - 1 ? commands[i+1].genArgs() : NULL,
			 **afterNextArgv = i < commandsSize - 2 ? commands[i+2].genArgs() : NULL;
		string cmd(argv[0]);
		/* input & output */
		in = index == 0 ? STDIN_FILENO : pipeCtrl[index - 1].getReadPipe();
		out = index == pipeSize ? STDOUT_FILENO : pipeCtrl[index].getWritePipe();
		if((nextType == 2 && afterNextType == 3) || (nextType == 3 && afterNextType == 2)) { i += 2; }
		else if(nextType == 2 || nextType == 3) { i++; }
		/* execute command */
		if(cmd == "exit") { return false; }
		else if(cmd == "export") {
			int j = 1;
			while(argv[j]) {
				string env(argv[j]);
				int indexOfEqual = env.find_first_of("=");
				if(setenv(env.substr(0, indexOfEqual).c_str(), env.substr(indexOfEqual + 1).c_str(), 1) != 0) {
					cout << "export error" << endl;
				}
				j++;
			}
		}
		else if(cmd == "unset") {
			int j = 1;
			while(argv[j]) {
				if(unsetenv(argv[j]) != 0) {
					cout << "unset error" << endl;
				}
				j++;
			}
		}
		char *inputFile = nextType == 2 ? nextArgv[0] : afterNextType == 2 ? afterNextArgv[0] : NULL;
		char *outputFile = nextType == 3 ? nextArgv[0] : afterNextType == 3 ? afterNextArgv[0] : NULL;
		executeSingleCommand(argv, in, out, index, pipeCtrl, pids, inputFile, outputFile);
		index++;
	}
	/* close pipe */
	for(int i = 0;i < pipeSize;i++) {
		pipeCtrl[i].closeReadPipe();
		pipeCtrl[i].closeWritePipe();
	}
	/* setpgid */
	for(unsigned int i = 0;i < pids.size();i++) {
		setpgid(pids[i], pgid);
		if(i == 0) { pgid = pids[i]; }
		if(i == 0) { tcsetpgrp(0, pgid); }
	}
	/* waitpid */
	for(unsigned int i = 0;i < pids.size();i++) {
		int status;
		waitpid(pids[i], &status, WUNTRACED);
	}
	tcsetpgrp(0, getpid());
	return true;
}

int main(int argc, char **argv) {
	init();
	while(1) {
		/* prompt */
		prompt();

		string input;
		getline(cin, input);
		Commands command(input);
		if(!execute(command.getCommands())) {
			break;
		}
	}
}
