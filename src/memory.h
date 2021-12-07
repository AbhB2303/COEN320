/*
 * memory.h
 *
 *  Created on: Dec. 5, 2021
 *      Author: athan
 */

#ifndef MEMORY_H_
#define MEMORY_H_

#include <mutex>
//used to lock and unlock data
//REFERENCE: https://en.cppreference.com/w/cpp/thread/mutex

struct memory { //definition for mutex call for given value
	std::mutex tex;
	float value;
};



#endif /* MEMORY_H_ */
