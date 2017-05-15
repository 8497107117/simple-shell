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
	(void) signal(SIGCHLD, reaper);
	signal(SIGINT, SIG_IGN);
	signal(SIGTSTP, SIG_IGN);
	signal(SIGQUIT, SIG_DFL);
}

bool execute(vector<Command> commands) {
	UnNamedPipe cur, pre;
	int status, commandsSize = commands.size();

	for(int i = 0;i < commandsSize;i++) {
		int type = commands[i].type(),
			nextType = i < commandsSize - 1 ? commands[i+1].type() : -1,
			afterNextType = i < commandsSize - 2 ? commands[i+2].type() : -1;
		char **argv = commands[i].genArgs(),
			 **nextArgv = i < commandsSize - 1 ? commands[i+1].genArgs() : NULL,
			 **afterNextArgv = i < commandsSize - 2 ? commands[i+2].genArgs() : NULL;
		string cmd(argv[0]);
		if((nextType == 2 && afterNextType == 3) || (nextType == 3 && afterNextType == 2)) {
			i += 2;
		}
		else if(nextType == 2 || nextType == 3) {
			i++;
		}

		if(cmd == "exit") {
			return false;
		}
		else if(cmd == "export") {
			if(setenv(argv[1], argv[2], 1) != 0) {
				cout << "export error" << endl;
			}
		}
		else if(cmd == "unset") {
			if(unsetenv(argv[1]) != 0) {
				cout << "unset error" << endl;
			}
		}

		if(i != commandsSize - 1) {
			cur.createPipe();
		}

		pid_t pid;
		pid = fork();

		if(pid < 0) {
			cout << "fork error" << endl;
			exit(1);
		}
		/* child */
		else if(pid == 0) {
			signal(SIGINT, SIG_IGN);
			if(i != commandsSize - 1) {
				cur.closeReadPipe();
			}
			/*  stdin  */
			/* | */
			if(type == 1) {
				dup2(pre.getReadPipe(), STDIN_FILENO);
				pre.closeReadPipe();
			}
			/* < */
			if(nextType == 2) {
				int input = open(nextArgv[0], O_RDONLY);
				if(input == -1) {
					cout << "read file error" << endl;
					exit(1);
				}
				dup2(input, STDIN_FILENO);
				close(input);
			}
			else if(afterNextType == 2 && nextType == 3) {
				int input = open(afterNextArgv[0], O_RDONLY);
				if(input == -1) {
					cout << "read file error" << endl;
					exit(1);
				}
				dup2(input, STDIN_FILENO);
				close(input);
			}
			/*  stdout  */
			/* > */
			if(nextType == 3) {
				int output = open(nextArgv[0], O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
				if(output == -1) {
					cout << "write file error" << endl;
					exit(1);
				}
				dup2(output, STDOUT_FILENO);
				close(output);
			}
			else if(afterNextType == 3 && nextType == 2) {
				int output = open(afterNextArgv[0], O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
				if(output == -1) {
					cout << "write file error" << endl;
					exit(1);
				}
				dup2(output, STDOUT_FILENO);
				close(output);
			}
			else if(i != commandsSize - 1) {
				dup2(cur.getWritePipe(), STDOUT_FILENO);
				cur.closeWritePipe();
			}
			/* exec */
			if(cmd == "export" || cmd == "unset") {
				execlp("printenv", "printenv", NULL);
			}
			else {
				execvp(argv[0], argv);
			}
			printf("Unknown command: [%s].\n", argv[0]);
			exit(1);
		}
		/* parent */
		else {
			waitpid(pid, &status, WUNTRACED | WCONTINUED);
			if(WIFEXITED(status)) {
				kill(pid, SIGTERM);
			}
			else if(WIFSTOPPED(status)) {
				//printf("stopped\n");
			}
			else {
				//printf("???\n");
			}
			if(i != commandsSize - 1) {
				if(nextType == 3) {
					int input = open(nextArgv[0], O_RDONLY);
					if(input == -1) {
						cout << "read file error" << endl;
						exit(1);
					}
					cur.setReadPipe(input);
				}
				else if(afterNextType == 3 && nextType == 2) {
					int input = open(afterNextArgv[0], O_RDONLY);
					if(input == -1) {
						cout << "read file error" << endl;
						exit(1);
					}
					cur.setReadPipe(input);
				}
				cur.closeWritePipe();
				pre.closeReadPipe();
				pre.setPipe(cur);
			}
			else {
				pre.closeReadPipe();
			}
		}
	}
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
