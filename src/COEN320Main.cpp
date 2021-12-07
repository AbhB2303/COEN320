#include <iostream>


#include "Consumer.h"
#include "Producer.h"


using namespace std;

Consumer* variables[5]; //declaration of consumer objects for each sensor


int main() {

	//define files to be read from
	string files[5] = {string("Current_Gear.txt"),
						string("Engine_coolant_temperature.txt"),
						string("Engine_speed.txt"),
						string("Fuel_consumption.txt"),
						string("Vehicle_speed.txt")
					   };

	//length of each entry in file, including newlines (\n)
	int size[5] = {3, 3, 4, 7, 3};

	for(int i = 0; i < 5; i++) { //create instances of each variable using the consumer class
		variables[i] = new Consumer(files[i], size[i], 4);
	}


	//auto& to share each variables reference rather than create a local copy and set the period
	//REFERENCE: http://www.cplusplus.com/forum/beginner/252949/
	//auto assumes data type being set
	for(auto& variable: variables) {
			variable->setPeriod(5); //set the default period
		}
	//essentially ensures we setPeriod of each variable by reference rather than copying into a new object

	Producer producer;// initialize producer
	producer.setPeriod(1); //set initial period to gain proper time tick (setPeriod(2) would make time tick by 2 at each increment)

	while(true){ //Sporadic task
		int input, newPeriod; //declaration of values
		cin >> input >> newPeriod; //taking input for both values

		producer.outputLock(); //take lock to change period definition
		if(input >= 0 && input <= 4 && newPeriod > 0)//check for valid change in input
		{
		variables[input] -> setPeriod(newPeriod); //set the period for the requested sensor
		cout << "set period for " << files[input] << " to " << newPeriod << endl; //set the period and output confirmation
		}
		else
		{
			cout << "Please select a valid input"; //request proper input from user
		}
		producer.outputUnlock(); //unlock mutex to allow other tasks to run
	}

	return EXIT_FAILURE; //return program failure in case of unsuccessful execution
}
