#include <sys/time.h>
#include <signal.h>
#include <time.h>
#include <stdint.h>
#include <string.h>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

using namespace std;

#define ONE_THOUSAND	1000
#define ONE_MILLION		1000000
/* offset and period are in microsends. */
#define OFFSET 1000000
#define PERIOD 5000000

sigset_t sigst;
ifstream myFile;


string fuel_consumption[94382]; //A
string engine_speed[94382]; //M
string engine_coolant_temperature[94382]; //R
string current_gear[94382]; //AH
string vehicle_speed[94382]; //AR

int x = 0;


static void wait_next_activation(void) {
	int dummy;
	/* suspend calling process until a signal is pending */
	sigwait(&sigst, &dummy);
}

int start_periodic_timer(uint64_t offset, int period) {
	struct itimerspec timer_spec;
	struct sigevent sigev;
	timer_t timer;
	const int signal = SIGALRM;
	int res;

	/* set timer parameters */
	timer_spec.it_value.tv_sec = offset / ONE_MILLION;
	timer_spec.it_value.tv_nsec = (offset % ONE_MILLION) * ONE_THOUSAND;
	timer_spec.it_interval.tv_sec = period / ONE_MILLION;
	timer_spec.it_interval.tv_nsec = (period % ONE_MILLION) * ONE_THOUSAND;

	sigemptyset(&sigst); // initialize a signal set
	sigaddset(&sigst, signal); // add SIGALRM to the signal set
	sigprocmask(SIG_BLOCK, &sigst, NULL); //block the signal

	/* set the signal event a timer expiration */
	memset(&sigev, 0, sizeof(struct sigevent));
	sigev.sigev_notify = SIGEV_SIGNAL;
	sigev.sigev_signo = signal;

	/* create timer */
	res = timer_create(CLOCK_MONOTONIC, &sigev, &timer);

	if (res < 0) {
		perror("Timer Create");
		exit(-1);
	}

	/* activate the timer */
	return timer_settime(timer, 0, &timer_spec, NULL);
}



static void task_body(void) {
	static int cycles = 0;
	static uint64_t start;
	uint64_t current;
	struct timespec tv;

	if (start == 0) {
		clock_gettime(CLOCK_MONOTONIC, &tv);
		start = tv.tv_sec * ONE_THOUSAND + tv.tv_nsec / ONE_MILLION;
	}

	clock_gettime(CLOCK_MONOTONIC, &tv);
	current = tv.tv_sec * ONE_THOUSAND + tv.tv_nsec / ONE_MILLION;

	if (cycles > 0) {
		fprintf(stderr, "Ave interval between instances: %f milliseconds\n",(double)(current-start)/cycles);
		cout << fuel_consumption[x] + "\n";
		cout << engine_speed[x] + "\n";
		//cout << engine_coolant_temperature[x] + "\n";
		//cout << current_gear[x] + "\n";
		//cout << vehicle_speed[x] + "\n";
		//x+= 5;
	}

	cycles++;
}

static string ReturnColumnValue(string line, int index_1, int index_2, size_t pos)
{
	string next_val;
	string rest_of_line = line.substr(pos+1);
	size_t nextpos = pos;
	for(int i = index_1; i <= index_2; i++)
	{
		 nextpos = rest_of_line.find(","); //find the next comma
		 next_val = rest_of_line.substr(0, nextpos);
		 rest_of_line = rest_of_line.substr(nextpos+1); //track the rest of the line

	}

	return next_val;
}

static void ReadDataSet() //method to read in all values from csv file
{
	  string line; //variable to read each line of the csv file
	  ifstream myfile ("dataset.csv"); //declaration of dataset file
	  int j = 0; //counter to read down a row of values separated by columns
	  if (myfile.is_open()) //while this file is open
	  {
	    while (getline(myfile,line)) //at each iteration, the next row is looked at
	    {

	    	//line contains full set of values

	    	//finding fuel consumption in row
	    	size_t pos = line.find(","); //find first comma
	    	string next_val = line.substr(0, pos); //separate value of first column
	    	fuel_consumption[j] = next_val; //story value in proper array



	    	int engine_speed_pos = 11;

	    	engine_speed[j] = ReturnColumnValue(line, 0, engine_speed_pos, pos);

	    	int engine_coolant_temp_pos = 27;

	    	engine_coolant_temperature[j] = ReturnColumnValue(line, engine_speed_pos, engine_coolant_temp_pos, pos);

	    	int current_gear_pos = 59;

	    	current_gear[j] = ReturnColumnValue(line, engine_coolant_temp_pos, current_gear_pos, pos);

	    	int vehicle_speed_pos = 101;

	    	vehicle_speed[j] = ReturnColumnValue(line, current_gear_pos, vehicle_speed_pos, pos);

	    	/*string next_val;
	    	string rest_of_line = line.substr(pos+1);
	    	size_t nextpos = pos;
	    	for(int i = 0; i <= engine_speed_pos; i++)
	    	{
	    		//cout<<rest_of_line;
	    	    nextpos = rest_of_line.find(","); //find the next comma
	    	    cout << nextpos;
	    	    next_val = rest_of_line.substr(0, nextpos);
	    	    rest_of_line = rest_of_line.substr(nextpos+1); //track the rest of the line

	    	}

	    	//engine_speed[j] = next_val;

	    	int engine_coolant_temp_pos = 15;
	    	for(int i = engine_speed_pos; i <= engine_coolant_temp_pos; i++)
	    		    	{
	    		    		//cout<<rest_of_line;
	    		    	    nextpos = rest_of_line.find(","); //find the next comma
	    		    	    //cout << nextpos;
	    		    	    next_val = rest_of_line.substr(0, nextpos);
	    		    	    rest_of_line = rest_of_line.substr(nextpos+1); //track the rest of the line

	    		    	}
	    	engine_coolant_temperature[j] = next_val;

	    	int current_gear_pos = 30;
	    	for(int i = engine_coolant_temp_pos; i <= current_gear_pos; i++)
	    	{
	    		nextpos = rest_of_line.find(","); //find the next comma
	    			    	    next_val = rest_of_line.substr(0, nextpos);
	    			    	    rest_of_line = rest_of_line.substr(nextpos+1); //track the rest of the line
	    	}
	    	current_gear[j] = next_val;


	    	int vehicle_speed_pos = 39;
	    	for(int i = current_gear_pos; i <= vehicle_speed_pos; i++)
	    	{
	    		nextpos = rest_of_line.find(","); //find the next comma
	    	    next_val = rest_of_line.substr(0, nextpos);
	    	    rest_of_line = rest_of_line.substr(nextpos+1); //track the rest of the line

	    	}
	    	vehicle_speed[j] = next_val;


	    	//string rest_of_line = line.substr(pos+1); //track the rest of the line
	    	//size_t nextpos = rest_of_line.find(","); //find the next comma
	    	//string next_val = rest_of_line.substr(0, nextpos);

	    	//accelerator_pedal_value[i] = next_val;
	    	j++;
	        //cout << str << '\n';

	         */
	    	j++;
	    }
	    myfile.close();
	  }

	  else cout << "Unable to open file";

}


int main (int argc, char *argv[]) {


	ReadDataSet(); //reads all needed values into an array with the same name

	cout << "All data has been read and stored into arrays \n";
	cout << fuel_consumption[x] + "\n";
			cout << engine_speed[x] + "\n";
			cout << engine_coolant_temperature[x] + "\n";
			cout << current_gear[x] + "\n";
			cout << vehicle_speed[x] + "\n";
	/*int res;

	//set and activate a timer
	res = start_periodic_timer(OFFSET, PERIOD);
	if (res < 0 ){
		perror("Start periodic timer");
		return -1;
	}

	while (1) {
		wait_next_activation(); //wait for timer expiration
		task_body(); //executes the task
	}*/

	//running a producer to display values every 5 seconds


	return 0;
}
