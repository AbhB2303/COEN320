/*
 * Producer.h
 *
 *  Created on: Nov. 5, 2021
 *      Author: athan
 */

#ifndef PRODUCER_H_
#define PRODUCER_H_

#include <iostream>
#include <fstream>
#include <time.h>
#include <sys/time.h>
#include "memory.h"
#include <sys/mman.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/neutrino.h>
#include <mutex>

#define SIZE sizeof(struct memory)

class Producer {

	//TODO: change time reference to use global time, not this local time
	int time = 0;	//the current time
	int period;	//period of the timer
	int priority = 123;	//priority of the thread

	timer_t timer; //QNX defined timer


	static const char *variables[5];
	int shm[5]; //implements shared memory objects
	struct memory* sharedMemory[5]; //define a mutex for each variable



	std::mutex mutexLock;

private:

	bool createThread();
	void writeValues();
	static void trampolineFunc(sigval);

public:
	Producer(int priority = 127);

	void outputLock();
	void outputUnlock();
	int setPeriod(int);

	virtual ~Producer();
};

#endif /* PRODUCER_H_ */
