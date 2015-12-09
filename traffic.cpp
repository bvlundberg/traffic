/*	Author: 	Brandon Lundberg
	Filename:	traffic.cpp
	Purpose:	Implement a traffic light using multithreading
	Date: 		2 December 2015
*/
// g++ traffic.cpp -std=c++11 -pthread
#define NUM_CARS 60
#define DIRECTIONS 4
#define CARS_NORTH 15
#define CARS_EAST 15
#define CARS_SOUTH 15
#define CARS_WEST 15
#define MAX_CARS_PASSING 5
#define DRIVE_THROUGH_INTERSECTION 300000
#define NEW_CAR_SLEEP 300000

// 0: North
// 1: East
// 2: South
// 3: West
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <ctime>
#include <mutex>
#include <queue>
#include <unistd.h>

using namespace std;
// Some global variables
clock_t timer;
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
			directionFlag = 0;
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
		int id;
		int direction;	// North: 0, East: 1, South:2, West: 3
		double entrytime;
		double exittime;
	
		Car(){}
		void donePassing();
};

queue<Car> northqueue;
queue<Car> eastqueue;
queue<Car> southqueue;
queue<Car> westqueue;

void ProcessCar(Car *c){
	c->exittime = (clock() - timer) / 1000.0;
	std::cout.precision(2);
	switch(c->direction){
		
		case 0:
			std::cout << "Car " << c->id << " from the North finised at time " << fixed << c->exittime << std::endl;
			break;
		case 1:
			std::cout << "Car " << c->id << " from the East finised at time  " << fixed << c->exittime << std::endl;
			break;
		case 2:
			std::cout << "Car " << c->id << " from the South finised at time " << fixed << c->exittime << std::endl;
			break;
		case 3:
			std::cout << "Car " << c->id << " from the West finised at time  " << fixed << c->exittime << std::endl;
			break;
		default:
			std::cout << "Not sure where this car came from!" << std::endl;
			break;
	}

	return;
}

void GoCarsGo(Intersection *intersection, queue<Car> *a, queue<Car> *b){
	int carCount = 0;
	cout << "Opening Intersection at time " << (clock() - timer) / 1000.0 << endl;
	while((a->size() != 0 || b->size() != 0) && carCount < MAX_CARS_PASSING){
		usleep(DRIVE_THROUGH_INTERSECTION);	// 3 seconds to get through intersection
		if(a->size() != 0){
			Car c1 = a->front();
			ProcessCar(&c1);
			a->pop();
		
			intersection->passCount++;
		}
		if(b->size() != 0){
			Car c2 = b->front();
			ProcessCar(&c2);
			b->pop();

			intersection->passCount++;			
		}
		carCount++;
	}
	cout << "Closing Intersection at time " << (clock() - timer) / 1000.0 << endl;
}

void *ManageIntersection(void*){
	Intersection intersection;
	while(intersection.passCount < NUM_CARS){
		// Allow cars in that direction to pass
		switch(intersection.directionFlag){
			case 0: 
				lock(northlock, southlock);
				GoCarsGo(&intersection, &northqueue, &southqueue);
				northlock.unlock();
				southlock.unlock();
				break;
			case 1:
				lock(eastlock, westlock);
				GoCarsGo(&intersection, &eastqueue, &westqueue);
				eastlock.unlock();
				westlock.unlock();
				break;
			default:
				cout << "Bad direction" << endl;
				break;
		}
		// Stop cars in that direction from passings
		intersection.directionFlag = !intersection.directionFlag;
		// usleep(1000);
	}
	pthread_exit(NULL);
}
//lock(mutex1, mutex2)
void NorthQueueAddCar(Car *c){
	northlock.lock();
	northqueue.push(*c);
	c->entrytime = (clock() - timer) / 1000.0;
	northlock.unlock();
}

void *ManageNorth(void*){
	for(int i = 0; i < CARS_NORTH; i++){
		Car c;
		c.direction = 0;
		c.id = i;
		NorthQueueAddCar(&c);
		std::cout.precision(2);
		std::cout << "Car " << i << " in North arrived at time " << fixed << c.entrytime << std::endl;
		usleep(NEW_CAR_SLEEP);
	}
	pthread_exit(NULL);
}

void EastQueueAddCar(Car *c){
	eastlock.lock();
	eastqueue.push(*c);
	c->entrytime = (clock() - timer) / 1000.0;
	eastlock.unlock();
}

void *ManageEast(void*){
	for(int i = 0; i < CARS_EAST; i++){
		Car c;
		c.id = i;
		c.direction = 1;
		EastQueueAddCar(&c);
		std::cout.precision(2);
		std::cout << "Car " << i << " in East arrived at time " << fixed << c.entrytime << std::endl;
		usleep(NEW_CAR_SLEEP);
	}
	pthread_exit(NULL);
}

void SouthQueueAddCar(Car *c){
	southlock.lock();
	southqueue.push(*c);
	c->entrytime = (clock() - timer) / 1000.0;
	southlock.unlock();
}

void *ManageSouth(void*){
	for(int i = 0; i < CARS_SOUTH; i++){
		Car c;
		c.id = i;
		c.direction = 2;
		SouthQueueAddCar(&c);
		std::cout.precision(2);
		std::cout << "Car " << i << " in South arrived at time " << fixed << c.entrytime << std::endl;
		usleep(NEW_CAR_SLEEP);
	}
	pthread_exit(NULL);
}

void WestQueueAddCar(Car *c){
	westlock.lock();
	westqueue.push(*c);
	c->entrytime = (clock() - timer) / 1000.0;
	westlock.unlock();
}

void *ManageWest(void*){
	for(int i = 0; i < CARS_WEST; i++){
		Car c;
		c.id = i;
		c.direction = 3;
		WestQueueAddCar(&c);
		std::cout.precision(2);
		std::cout << "Car " << i << " in West arrived at time " << fixed << c.entrytime << std::endl;
		usleep(NEW_CAR_SLEEP);
	}
	pthread_exit(NULL);
}

int main(){
	// Global variables for the intersection thread and each direction thread
	pthread_t intersectionThread;
	pthread_t directionThreads[DIRECTIONS];
	//Start timer
	timer = clock();
	std::cout << "Start at time 0" << std::endl;
	// Create intersection thread and open the street
	std::cout << "Create intersection thread" << std::endl;
	pthread_create(&intersectionThread, NULL, ManageIntersection, NULL);

	// Start each car thread after a random sleep time
	std::cout << "Create car threads" << std::endl;
	pthread_create(&directionThreads[0], NULL, &ManageNorth, NULL);
	pthread_create(&directionThreads[1], NULL, &ManageEast, NULL);
	pthread_create(&directionThreads[2], NULL, &ManageSouth, NULL);
	pthread_create(&directionThreads[3], NULL, &ManageWest, NULL);

	pthread_exit(NULL);
	// cout << "Ending at time " << (clock() - timer) / 1000.0 << endl;
}