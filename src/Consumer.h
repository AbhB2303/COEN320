/*
 * Consumer.h
 *
 *  Created on: Nov. 5, 2021
 *      Author: athan
 */

#ifndef CONSUMER_H_
#define CONSUMER_H_

#include <iostream>
#include <fstream>
#include <time.h>
#include <sys/time.h>
#include "memory.h"
#include <sys/mman.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/neutrino.h>

#define SIZE sizeof(struct memory)

class Consumer {

	//instance variables
    int time = 0;
    int period;
    int priority = 123;

    timer_t timer; //declaration of timer as seen in start_periodic_timer
    int channel = 0;

    int shm; //used for file opening
    struct memory* sharedMemory; //define memory mutex

    std::string variableName; //name of consumer (sensor)
    int width; //largest possible length of each value
    std::fstream file; //stream to read file




private:

    static void trampolineFunc(sigval); //forces call for write value in case of error or deadlock

    void Read(); //reads values being seeked out for given period
    bool create_thread(); //used to define the thread the consumer is running on

public:
    Consumer(std::string file, int width, int priority = 123);
    int setPeriod(int); //simply sets period
    virtual ~Consumer();
};

#endif /* CONSUMER_H_ */
