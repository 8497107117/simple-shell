#include "main.h"

using namespace std;

void prompt() {
	cout << "＼(・ω・＼)SAN値！(／・ω・)／ピンチ！ ";
}

int main(int argc, char **argv) {
	prompt();
	while(1) {
		string input;
		getline(cin, input);
		Commands command(input);
		cout << input << endl;
		prompt();
	}
}
