/*	Author: 	Brandon Lundberg
	Filename:	traffic.cpp
	Purpose:	Implement a traffic light using multithreading
	Date: 		2 December 2015
*/

#define NUM_CARS 50
#define PASS_PER_GREEN 5

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

//srand(time(NULL));
//rand()
int main()
{
	pthread_t intersectionThread;
	pthread_t carsThreads[NUM_CARS];

}

class Intersection{
		private:
		Lock lock;
		Cond carsPass;
		// The name of the queue is the direction the cars are headed
		Queue<Car> North;
		Queue<Car> East;
		Queue<Car> South;
		Queue<Car> West;

		int arrivalCount;
		int passCount;
		int passPerGreen; // n can pass per green light

		
	public:
		Intersection()
		{
			BuildRandomCars();
			OpenStreet();
		};
		~Intersection() {};
		void OpenStreet() 	// Loop to open street
		{
			
			printf("Street is open!");
			while(1)
			{
				WaitForCar();
			}

		} 			
		void EnterIntersection();	// Called by cars

	private:
		void BuildRandomCars()
		{
			// Get random seed (direction of a new car) 
			// srand(something);
			sleep(rand());
			for(int i = 0; i < 50; i++)
			{
				OriginalCars.push(new Car(rand()));
			}
		}
		void WaitForCar();
		void PrintIntersectionStats();

		bool ReachedPassingLimit();
};

class Car{
	private:
		int direction;	// North: 0, East: 1, South: 2, West: 3
	public:
		Car(int d){
			direction = d;
		}
	private:
		void donePassing();
};