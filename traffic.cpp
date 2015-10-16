/*	Author: 	Brandon Lundberg
	Filename:	traffic.cpp
	Purpose:	Implement a traffic light using multithreading
	Date: 		2 December 2015
*/

#define NUM_CARS 60
#define DIRECTIONS 4
#define CARS_NORTH 15
#define CARS_EAST 15
#define CARS_SOUTH 15
#define CARS_WEST 15
#define MAX_CARS_PASSING 5

// 0: North
// 1: East
// 2: South
// 3: West
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <mutex>
#include <queue>
#include <unistd.h>

using namespace std;
// Some global variables
time_t timer;
mutex northlock;
mutex eastlock;
mutex southlock;
mutex westlock;


class Intersection{
	public:
		int arrivalCount;
		int passCount;
		int directionFlag;
		// 0: North/South
		// 1: East/West

		Intersection()
		{
			OpenStreet();
		};
		~Intersection() {};
		void OpenStreet() 	// Loop to open street
		{	
			printf("Street is open!");
		} 			
		void EnterIntersection();	// Called by cars

		void WaitForCar();
		void PrintIntersectionStats();

		bool ReachedPassingLimit();
};

class Car{
	public:
		int direction;	// North: 0, East: 1, South:2, West: 3
		int entrytime;
		int exittime;
	
		Car(){}
		void donePassing();
};

queue<Car> northqueue;
queue<Car> eastqueue;
queue<Car> southqueue;
queue<Car> westqueue;

void ProcessCar(Car c){
	c.exittime = difftime(timer,time(NULL));
	return;
}

void GoCarsGo(queue<Car> a, queue<Car> b){
	int carCount = 0;
	while(a.size() != 0 && b.size() != 0 && carCount < MAX_CARS_PASSING){
		Car c1 = a.front();
		Car c2 = b.front();
		ProcessCar(c1);
		ProcessCar(c2);
		a.pop();
		b.pop();a
		carCount++;
	}
}

void *ManageIntersection(void*){
	Intersection intersection;
	while(intersection.passCount < NUM_CARS){
		// Allow cars in that direction to pass
		switch(intersection.directionFlag){
			case 0: 
				lock(northlock, southlock);
				GoCarsGo(northqueue, southqueue);
				northlock.unlock();
				southlock.unlock();
				break;
			case 1:
				lock(eastlock, westlock);
				GoCarsGo(eastqueue, westqueue);
				eastlock.unlock();
				westlock.unlaock();
				break;
			default:
				cout << "Bad direction" << endl;
				break;
		}
		// Stop cars in that direction from passings
		intersection.directionFlag = !intersection.directionFlag;
		usleep(100);
	}
	pthread_exit(NULL);
}
//lock(mutex1, mutex2)
void NorthQueueAddCar(Car c){
	northlock.lock();
	northqueue.push(c);
	northlock.unlock();
}

void *ManageNorth(void*){
	for(int i = 0; i < CARS_NORTH; i++){
		Car c;
		c.direction = 0;
		c.entrytime = difftime(timer,time(NULL));
		NorthQueueAddCar(c);
	}
	pthread_exit(NULL);
}

void EastQueueAddCar(Car c){
	eastlock.lock();
	eastqueue.push(c);
	eastlock.unlock();
}

void *ManageEast(void*){
	for(int i = 0; i < CARS_EAST; i++){
		Car c;
		c.direction = 0;
		c.entrytime = difftime(timer,time(NULL));
		EastQueueAddCar(c);
	}
	pthread_exit(NULL);
}

void SouthQueueAddCar(Car c){
	southlock.lock();
	southqueue.push(c);
	southlock.unlock();
}

void *ManageSouth(void*){
	for(int i = 0; i < CARS_SOUTH; i++){
		Car c;
		c.direction = 0;
		c.entrytime = difftime(timer,time(NULL));
		SouthQueueAddCar(c);
	}
	pthread_exit(NULL);
}

void WestQueueAddCar(Car c){
	westlock.lock();
	westqueue.push(c);
	westlock.unlock();
}

void *ManageWest(void*){
	for(int i = 0; i < CARS_WEST; i++){
		Car c;
		c.direction = 0;
		c.entrytime = difftime(timer,time(NULL));
		WestQueueAddCar(c);
	}
	pthread_exit(NULL);
}

int main(){
	// Global variables for the intersection thread and each direction thread
	pthread_t intersectionThread;
	pthread_t directionThreads[DIRECTIONS];
	//Start timer
	time(&timer);
	std::cout << "Start at time 0" << std::endl;
	// Create intersection thread and open the street
	pthread_create(&intersectionThread, NULL, ManageIntersection, NULL);

	// Start each car thread after a random sleep time
	pthread_create(&directionThreads[0], NULL, &ManageNorth, NULL);
	pthread_create(&directionThreads[1], NULL, &ManageEast, NULL);
	pthread_create(&directionThreads[2], NULL, &ManageSouth, NULL);
	pthread_create(&directionThreads[3], NULL, &ManageWest, NULL);
	pthread_exit(NULL);
}