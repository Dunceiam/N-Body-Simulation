/*
 * Simulation2.cpp
 *
 *  Created on: Dec 28, 2010
 *      Author: Sam Curley
 *      Version: 0.1.0.a
 */

#include <iostream>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <fstream>
using namespace std;

struct Particle
{
	float x;
	float y;
	float z;
	float xv;
	float yv;
	float zv;
	float xaccel;
	float yaccel;
	float zaccel;
	float forcex;
	float forcey;
	float forcez;
	float mass;
};

int main() {
	srand(time(0));
	int numParticles = 2;
	int timesteps = 35000;  									//number of timesteps
	float time = 10;  											//time between timesteps
	float gravconstant = 0.000000000000667428;
	Particle myParticles[numParticles];
	for(int x=0; x<numParticles; x++)  							//initialize particles
	{
		int rand1 = 0;
		int rand2 = 0;
		int rand3 = 0;
		for(int z=0; z<100; z++)
		{
			rand1 = rand1 + (rand()%32767); 					//Random number generators for distance, from 0km to 4.3 times the distance between the Earth and Moon (1638350km)
			rand2 = rand2 + (rand()%32767);
			rand3 = rand3 + (rand()%32767);
		}
		myParticles[x].x = rand1 - 1638350;
		myParticles[x].y = rand2 - 1638350;
		myParticles[x].z = rand3 - 1638350;
		myParticles[x].mass = 5974200000000000000000;    		//Mass of the Earth divided by 1000
		cout << endl << "[" << myParticles[x].x << "], [" << myParticles[x].y << "], [" << myParticles[x].z << "]. " << "Mass: " << myParticles[x].mass;
	}
	//	myParticles[0].x = -2000.0; 								 //overwriting previous random generations for testing purposes
	//	myParticles[1].x = 2000.0;
	//	myParticles[0].y = 0;
	//	myParticles[0].z = 0;
	//	myParticles[1].y = 0;
	//	myParticles[1].z = 0;

	ofstream myfile ("myfile.csv");
	for(int z=0; z<timesteps; z++)
	{
		for(int x=0; x<numParticles; x++) 					//main loop, cycles through every particle
		{

			for(int y=0; y<numParticles; y++)  				//nested loop, cycles through every particle
			{
				if(y!=x)								 	//prevent calculations by a particle on itself
				{
					//gravity and distance calculations
				}
			}
		}
		for(int x=0; x<numParticles; x++)
		{
			//update positions, velocities.
		}
		if (myfile.is_open())
		{
			myfile << myParticles[0].x << "," << myParticles[0].xv << "," << myParticles[0].forcex << "," << myParticles[1].x << "," << myParticles[1].xv << "," << myParticles[1].forcex << endl;
		}
		else cout << "\nUnable to open";
	}
	myfile.close();
}


