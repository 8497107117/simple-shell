#include "command.h"

void Command::splitCommand(string input) {
	int indexOfSpace = 0;
	while(1) {
		indexOfSpace = input.find_first_of(' ', 0);
		if(indexOfSpace == std::string::npos) {
			this->args.push_back(input);
			return;
		}
		else if(indexOfSpace == 0) {
			input.assign(input, 1, input.length() - 1);
		}
		else {
			string tmp;
			tmp.assign(input, 0, indexOfSpace);
			if(tmp == "ls") {
				this->args.push_back("sh");
				this->args.push_back("-c");
				this->args.push_back(input);
				break;
			}
			else {
				this->args.push_back(tmp);
				input.assign(input, indexOfSpace + 1, input.length() - indexOfSpace - 1);
			}
		}
	}
}

Command::Command(char symbol, string input) {
	this->symbol = symbol;
	this->splitCommand(input);
}

int Command::type() {
	return this->symbol == ' ' ? 0 : this->symbol == '|' ? 1 : this->symbol == '<' ? 2 : 3;
}

char** Command::genArgs() {
	char ** argv;
	argv = new char *[this->args.size() + 1];
	for(int i = 0;i < this->args.size();i++) {
		argv[i] = strdup(this->args[i].c_str());
	}
	argv[this->args.size()] = NULL;
	return argv;
}

void Commands::splitSymbol(string input) {
	/* \r\n */
	int indexOfRN = 0;
	while(1) {
		indexOfRN = input.find_first_of("\r\n", 0);
		if(indexOfRN == std::string::npos) {
			break;
		}
		else {
			input.erase(indexOfRN, 1);
		}
	}
		int indexOfSymbol = 0;
		char symbol = ' ';
	while(1) {
		indexOfSymbol = input.find_first_of(this->symbol, 0);
		if(indexOfSymbol == string::npos) {
			string tmp = this->trimCommand(input);
			if(tmp != "") {
				Command cmd(symbol, tmp);
				this->commands.push_back(cmd);
			}
			return;
		}
		else {
			string tmp;
			tmp.assign(input, 0, indexOfSymbol);
			tmp = this->trimCommand(tmp);
			if(tmp != "") {
				Command cmd(symbol, tmp);
				this->commands.push_back(cmd);
			}
			symbol = input.c_str()[indexOfSymbol];
			input.assign(input, indexOfSymbol + 1, input.length() - indexOfSymbol - 1);
		}
	}
}

string Commands::trimCommand(string input) {
	int index = 0;
	while(1) {
		index = input.find_first_not_of(' ', 0);
		if(index == string::npos) {
			return "";
		}
		else if(index == 0) {
			break;
		}
		else {
			input.assign(input, index, input.length() - 1);
		}
	}
	index = input.find_last_not_of(' ', input.length());
	input.assign(input, 0, index + 1);
	return input;
}

Commands::Commands(string input) {
	this->symbol = "|<>";
	this->splitSymbol(input);
}

vector<Command> Commands::getCommands() {
	return this->commands;
}

bool Commands::execute() {
	vector<Command> commands = this->commands;
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
				printf("exited\n");
			}
			else if(WIFSTOPPED(status)) {
				printf("stopped\n");
			}
			else {
				printf("???\n");
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
