#include "main.h"

using namespace std;

void reaper(int sig) {
	int status;
	waitpid(-1, &status, WNOHANG);
}

void prompt() {
	cout << "＼(・ω・＼)SAN値！(／・ω・)／ピンチ！ ";
}

int main(int argc, char **argv) {
	(void) signal(SIGCHLD, reaper);
	prompt();
	while(1) {
		string input;
		getline(cin, input);
		Commands command(input);
		command.execute();
		prompt();
	}
}
