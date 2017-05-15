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

bool execute(vector<Command> commands);
