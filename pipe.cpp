#include "pipe.h"

UnNamedPipe::UnNamedPipe() {
	this->enable[0] = false;
	this->enable[1] = false;
}

void UnNamedPipe::createPipe() {
	if(pipe(this->fd) < 0) {
		printf("create pipe error: %s\n", strerror(errno));
		return;
	}
	this->enable[0] = true;
	this->enable[1] = true;
}

void UnNamedPipe::setPipe(UnNamedPipe p) {
	this->enable[0] = p.readable();
	this->enable[1] = p.writable();
	if(p.readable()) {
		this->fd[0] = p.getReadPipe();
	}
	if(p.writable()) {
		this->fd[1] = p.getWritePipe();
	}
}

void UnNamedPipe::closeReadPipe() {
	if(this->enable[0]) {
		this->enable[0] = false;
		if(close(this->fd[0]) < 0) {
			printf("close readPipe error: %s\n", strerror(errno));
		}
	}
}

void UnNamedPipe::closeWritePipe() {
	if(this->enable[1]) {
		this->enable[1] = false;
		if(close(this->fd[1]) < 0) {
			printf("close writePipe error: %s\n", strerror(errno));
		}
	}
}

int UnNamedPipe::getReadPipe() {
	if(this->enable[0]) {
		return this->fd[0];
	}
	printf("get readPipe failed\n");
	return -1;
}

int UnNamedPipe::getWritePipe() {
	if(this->enable[1]) {
		return this->fd[1];
	}
	printf("get writePipe failed\n");
	return -1;
}

bool UnNamedPipe::readable() {
	return this->enable[0];
}

bool UnNamedPipe::writable() {
	return this->enable[1];
}
