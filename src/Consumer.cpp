/*
 * Consumer.cpp
 *
 *  Created on: Nov. 5, 2021
 *      Author: athan
 */

#include "Consumer.h"

Consumer::Consumer(std::string fileName, int width, int priority) {
	//setting all instance variables
    this->variableName = fileName;
    this->width = width;
    this->period = period;
    this->priority=priority;

    //REFERENCE: shm_open understood and adapted from https://man7.org/linux/man-pages/man3/shm_open.3.html
    //REFERENCE: QNX explanation on use of shm_open()": https://www.qnx.com/developers/docs/6.5.0SP1.update/com.qnx.doc.neutrino_lib_ref/s/shm_open.html
    shm = shm_open(this->variableName.c_str(), O_CREAT | O_RDWR, 0666); //method to open needed file
    //REFERENCE: https://www.geeksforgeeks.org/posix-shared-memory-api/
    //adapted code from GeeksToGeeks to be able to open shared memory locations with memory-mapped files
    if (shm== -1){
        perror("Shared memory could not be created, or already exists"); //error check for creating shared memory
        exit(1);
    }//successful call opens

    ftruncate(shm, SIZE); //configure size

    //___________________________________________________________________________________
    //ask athanas about this section
    sharedMemory = (memory*) mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm, 0);
    //memory map of the shared memory value
    //MAP_SHARED is a flag arguement to share the mapping to other processes
    //PROT describes desired memory protection
    //PROT_READ bitwise OR with PROT_WRITE to ensure pages can be read and written to
    if (sharedMemory == MAP_FAILED) { //error check for if memory mapping failed
        perror("Map failed\n");
        exit(1);
    }
    //if error is not called, the sharedMemory value should return a point to the mapped object.

    file.open(this->variableName);  //attempt to open file of given name
    if (!file.is_open()) { //error check for if file could not be opened
        perror("Could not open file");
        exit(1);
    }
    //___________________________________________________________________________________

    //creating thread (taken from given code)
    pthread_attr_t* thread_attr = new pthread_attr_t;
    pthread_attr_init(thread_attr);

    //start timer events as given in tutorials
    //makes use of
    struct sigevent sigev;
    SIGEV_THREAD_INIT(&sigev, trampolineFunc, this, thread_attr);
    int res = timer_create(CLOCK_REALTIME, &sigev, &timer);
    if(res == -1) { //error check for creating a timer
            perror("Could not create timer");
            exit(1);
    }
    float val; //define file
    file >> val;
    sharedMemory->tex.lock(); //lock the shared memory to give value of file
    sharedMemory->value = val; //set value
    sharedMemory->tex.unlock(); //unlock
}

void Consumer::trampolineFunc(sigval sig) {((Consumer*) sig.sival_ptr)->Read();} //if program is stuck forces write value

void Consumer::Read() {

    time += period; //set the new current time

    float val;
    file.seekg(time * width); //seek out the correct value for given period
    file >> val; //set that value

    sharedMemory->tex.lock(); //take the lock to be able to set value in shared memory
    sharedMemory->value = val; //consume the value
    sharedMemory->tex.unlock(); //unlock the mutex
}

int Consumer::setPeriod(int period) { //method to set the period

    this->period = period; //sets the current period

    itimerspec timerPeriod; //defines the period
    //values set in same manner as tutorial for timers
    timerPeriod.it_value.tv_sec = period;
    timerPeriod.it_value.tv_nsec = 0;
    timerPeriod.it_interval.tv_sec = period;
    timerPeriod.it_interval.tv_nsec = 0;
    return timer_settime(timer, 0, &timerPeriod, NULL);
}

Consumer::~Consumer() { //destructor for consumer objects

    timer_delete(timer);
    sharedMemory->tex.unlock();
    file.close();
    close(shm);

}
