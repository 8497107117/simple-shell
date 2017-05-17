#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/signal.h>
#include <signal.h>

#include "job.h"

using namespace std;

Jobs jobs;

void reaper(int sig);
void prompt();
void init();
void exportEnv(char **argv);
void unsetEnv(char **argv);
void foreground(int jobId);
void executeSingleCommand(char **argv, int in, int out, int index, vector<UnNamedPipe> pipeCtrl, vector<int> &pids,
		char *inputFile, char *outputFile);
void execute(Commands commands);
