/*
 * Producer.cpp
 *
 *  Created on: Nov. 5, 2021
 *      Author: athan
 */

#include "Producer.h"

//declaration of shared memory objects on producer end
const char* Producer::variables[5] = {"Current_Gear.txt",
		 "Engine_coolant_temperature.txt",
		 "Engine_speed.txt",
		 "Fuel_consumption.txt",
		 "Vehicle_speed.txt"};

Producer::Producer( int priority) {
	//producer constructor


	for(int i = 0; i < 5; i++) { //goes through each shared memory object and opens them then maps them
		shm[i] = shm_open(variables[i], O_RDWR, 0666); //shm_open with the shared variable
		//shm_open(const char* name, int flag, mode_t mode) => parameters to be passed
		//O_RDW to open variable with read write access
		//0666 for directory permissions
		//same references used to adapt this code as seen in the Consumer in relation to shm
			if (shm[i] == -1) { //check if shared memory object returns an error
				perror("in shm_open()");
				exit(1); //error if can't be opened
			}
			//attempt memory mapping shared memory code
			sharedMemory[i] = (memory*) mmap(0,sizeof(struct memory), PROT_READ | PROT_WRITE, MAP_SHARED, shm[i], 0);
			if (sharedMemory[i] == MAP_FAILED) { //error check for if memory map couldn't be established
				perror("in mmap()");
				exit(1);
			}
		//also makes use of the references noted within the consumer class (for opening shared memory locations and memory mapping)
	}

	//set instance variables based on parameters
	this->priority=priority;
	this->period = period;

	//thread declaration (as seen in tutorial)
	pthread_attr_t* thread_attr = new pthread_attr_t;
	pthread_attr_init(thread_attr);

	//essentially start_periodic_timer code from tutorials
	//also made use of QNX manual: http://www.qnx.com/developers/docs/6.5.0/index.jsp?topic=%2Fcom.qnx.doc.neutrino_getting_started%2Fs1_timer.html&cp=13_3_5
	struct sigevent sigev;
	SIGEV_THREAD_INIT(&sigev, trampolineFunc, this, thread_attr);
	int res = timer_create(CLOCK_REALTIME, &sigev, &timer);
	if(res == -1) {
			perror("Could not create timer");
			exit(1);
	}
}

void Producer::trampolineFunc(sigval sig) { ((Producer*) sig.sival_ptr)->writeValues(); }
//essentially to avoid certain errors such as deadlocks and force write a value


void Producer::writeValues() { //method to produce the value for consumer
	time += period; //set the current time

	float values[5]; //float data that saves each sensors value at given point in time
	for(int i = 0; i < 5; i++) { //collect a value for each variable utilizing the consumer mutex
		sharedMemory[i]->tex.lock(); //lock the mutex to prevent dual access to shared memory
		values[i] = sharedMemory[i]->value; //set all the current sensor values
		sharedMemory[i]->tex.unlock(); //unlock mutex to allow other process to lock it
	}

	mutexLock.lock(); //lock mutex to produce the needed values
	//output the values to be displayed and updated at given period
	std::cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
	std::cout << "Time: " << time << std::endl;
	std::cout << "\t0: Current Gear: " 			<< values[0]
		 << "\n\t1: Engine Coolant Temperature: " 	<< values[1]
		 << "\n\t2: Engine Speed: " 				<< values[2]
		 << "\n\t3: Fuel Consumption: " 			<< values[3]
		 << "\n\t4: Vehicle Speed: "				<< values[4]
		 << "\nEnter sensor number, followed by the period to set" << std::endl;
	mutexLock.unlock(); //unlocks mutex
}

int Producer::setPeriod(int period) { //method to set period on producer end

	this->period = period; //sets the period based on passed parameter

	itimerspec timerPeriod; //define timer as seen in tutorial code in start_periodic_timer
	timerPeriod.it_value.tv_sec = period;
	timerPeriod.it_value.tv_nsec = 0;
	timerPeriod.it_interval.tv_sec = period;
	timerPeriod.it_interval.tv_nsec = 0;
	return timer_settime(timer, 0, &timerPeriod, NULL);
}
//trgbtbrbgrf

//methods to set or release mutex as producer
void Producer::outputLock() {mutexLock.lock();}
void Producer::outputUnlock() {mutexLock.unlock();}

Producer::~Producer() { //producer destructor (unlock all shared memory values)

	timer_delete(timer);
	for(int i = 0; i < 5; i++) {
		sharedMemory[i]->tex.unlock();
		close(shm[i]);

	}
}



