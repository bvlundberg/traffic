/*	Author: 	Brandon Lundberg
	Filename:	traffic.cpp
	Purpose:	Implement a traffic light using multithreading
	Date: 		1 November 2015
*/
#define NUM_CARS 60
#define DIRECTIONS 4
#define CARS_NORTH 15
#define CARS_EAST 15
#define CARS_SOUTH 15
#define CARS_WEST 15
#define MAX_CARS_PASSING 3
#define INTERSECTION_SLEEP 1000
#define NEW_CAR_SLEEP 5000

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
// base timer
clock_t timer;
// Timer to account for time it takes for a car to drive through the intersection
double globalcounter;
// Lock for each queue
mutex northlock;
mutex eastlock;
mutex southlock;
mutex westlock;


class Intersection{
	public:
		int arrivalCount;	// Cars that arrive
		int passCount;		// Cars that pass through
		int directionFlag;	// Direction cars are moving in -  0: North/South, 1: East/West

		Intersection()
		{
			directionFlag = 0;
		};
		~Intersection() {};
};

class Car{
	public:
		int id;
		int direction;		// North: 0, East: 1, South:2, West: 3
		double entrytime;	// Time the car arrived at the intersection
		double exittime;	// Time the car finished driving through the intersection
	
		Car(int i, int d)
		{
			id = i;
			direction = d;
		}
};

// Queues for each direction
queue<Car> northqueue;
queue<Car> eastqueue;
queue<Car> southqueue;
queue<Car> westqueue;

// Process Car
// Sets the time that the car left the intersection and prints the result to the console
void ProcessCar(Car *c){
	c->exittime = ((clock() - timer) / 1000.0) + globalcounter;
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
// GoCarsGo
// Opens the intersection
// Removes a car (if possible) from the two queues passed in (multiple directions, bonus problem 1)
// Allows n times (number of passed allowed per green, Bonus problem 1)
// Closes the intersection
void GoCarsGo(Intersection *intersection, queue<Car> *a, queue<Car> *b){
	int carCount = 0;
	cout << "Opening Intersection at time " << ((clock() - timer) / 1000.0) + globalcounter << endl;
	globalcounter += 3.0;
	while((a->size() != 0 || b->size() != 0) && carCount < MAX_CARS_PASSING){	
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
	cout << "Closing Intersection at time " << ((clock() - timer) / 1000.0) + globalcounter << endl;
	usleep(INTERSECTION_SLEEP); // Time needed to switch light
}
// Depeding on the current direction allowed to pass, lock the resources needed to manipulate them
// Call the function to allow cars to pass
// Release the locks
// Change the direction for the next light change
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
// Add car functions
// Retrieve lock for queue
// Add car to queue
// Update car arrival time
// Release lock for queue
void NorthQueueAddCar(Car *c){
	northlock.lock();
	northqueue.push(*c);
	c->entrytime = ((clock() - timer) / 1000.0) + globalcounter;
	northlock.unlock();
}

void EastQueueAddCar(Car *c){
	eastlock.lock();
	eastqueue.push(*c);
	c->entrytime = ((clock() - timer) / 1000.0) + globalcounter;;
	eastlock.unlock();
}

void WestQueueAddCar(Car *c){
	westlock.lock();
	westqueue.push(*c);
	c->entrytime = ((clock() - timer) / 1000.0) + globalcounter;;
	westlock.unlock();
}

void SouthQueueAddCar(Car *c){
	southlock.lock();
	southqueue.push(*c);
	c->entrytime = ((clock() - timer) / 1000.0) + globalcounter;;
	southlock.unlock();
}
// Manage directions functions
// Create a car to add to the intersection
// Sleep a random amount of time before adding another car
void *ManageNorth(void*){
	srand(time(NULL));
	for(int i = 0; i < CARS_NORTH; i++){
		Car c(i, 0);
		NorthQueueAddCar(&c);
		std::cout.precision(2);
		std::cout << "Car " << i << " in North arrived at time " << fixed << c.entrytime << std::endl;
		usleep(rand() % NEW_CAR_SLEEP);
	}
	pthread_exit(NULL);
}


void *ManageEast(void*){
	srand(time(NULL));
	for(int i = 0; i < CARS_EAST; i++){
		Car c(i, 1);
		EastQueueAddCar(&c);
		std::cout.precision(2);
		std::cout << "Car " << i << " in East arrived at time " << fixed << c.entrytime << std::endl;
		usleep(rand() % NEW_CAR_SLEEP);
	}
	pthread_exit(NULL);
}



void *ManageSouth(void*){
	srand(time(NULL));
	for(int i = 0; i < CARS_SOUTH; i++){
		Car c(i, 2);
		SouthQueueAddCar(&c);
		std::cout.precision(2);
		std::cout << "Car " << i << " in South arrived at time " << fixed << c.entrytime << std::endl;
		usleep(rand() % NEW_CAR_SLEEP);
	}
	pthread_exit(NULL);
}

void *ManageWest(void*){
	srand(time(NULL));
	for(int i = 0; i < CARS_WEST; i++){
		Car c(i, 3);
		WestQueueAddCar(&c);
		std::cout.precision(2);
		std::cout << "Car " << i << " in West arrived at time " << fixed << c.entrytime << std::endl;
		usleep(rand() % NEW_CAR_SLEEP);
	}
	pthread_exit(NULL);
}

int main(){
	// Init global counter
	globalcounter = 0;
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
}