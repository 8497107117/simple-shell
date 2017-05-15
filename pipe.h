#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>
#include <errno.h>

int errno;

using namespace std;

class UnNamedPipe {
	private:
		int fd[2];
		bool enable[2];
	public:
		UnNamedPipe();
		/* pipe(fd) */
		void createPipe();
		/* for record */
		void setPipe(UnNamedPipe p);
		void setReadPipe(int fd);
		void closeReadPipe();
		void closeWritePipe();
		int getReadPipe();
		int getWritePipe();
		bool readable();
		bool writable();
};
