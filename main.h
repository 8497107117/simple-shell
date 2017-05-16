#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/signal.h>

#include "command.h"

void reaper(int sig);

void prompt();

void init();

void executeSingleCommand(char **argv, int in, int out, int index, vector<UnNamedPipe> pipeCtrl, vector<int> &pids,
		char *inputFile, char *outputFile);

bool execute(vector<Command> commands);
