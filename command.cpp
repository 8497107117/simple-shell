#include "command.h"

void Command::splitCommand(string input) {
	int indexOfSpace = 0, indexOfQuotes = 0, indexOfEndQuotes;
	while(1) {
		indexOfSpace = input.find_first_of(' ', 0);
		indexOfQuotes = input.find_first_of('\"', 0);
		indexOfEndQuotes = input.find_first_of('\"', 1);
		if(indexOfSpace == std::string::npos) {
			if(indexOfQuotes == 0 && indexOfEndQuotes != string::npos) {
				this->args.push_back(input.substr(1, indexOfEndQuotes - 1));
			}
			else if(input.find("*") != string::npos || input.find("?") != string::npos) {
				glob_t result;
				glob(input.c_str(), GLOB_TILDE, NULL, &result);
				for(int i = 0;i < (int)result.gl_pathc;i++) {
					this->args.push_back(result.gl_pathv[i]);
				}
				if(!result.gl_pathc) {
					this->args.push_back(input);
				}
			}
			else {
				this->args.push_back(input);
			}
			return;
		}
		else if(indexOfSpace == 0) {
			input.assign(input, 1, input.length() - 1);
		}
		else {
			string tmp;
			tmp.assign(input, 0, indexOfSpace);
			if(indexOfQuotes == 0 && indexOfEndQuotes != string::npos) {
				this->args.push_back(input.substr(1, indexOfEndQuotes - 1));
				input.assign(input, indexOfEndQuotes + 1, input.length() - indexOfEndQuotes - 1);
			}
			else if(tmp.find("*") != string::npos || tmp.find("?") != string::npos) {
				glob_t result;
				glob(tmp.c_str(), GLOB_TILDE, NULL, &result);
				for(int i = 0;i < (int)result.gl_pathc;i++) {
					this->args.push_back(result.gl_pathv[i]);
				}
				if(!result.gl_pathc) {
					this->args.push_back(tmp);
				}
				input.assign(input, indexOfSpace + 1, input.length() - indexOfSpace - 1);
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
	for(unsigned int i = 0;i < this->args.size();i++) {
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
