/*  Sam Curley
 *  N-Body Gravitational Simulation
 *  0.0.9.a
 */

//0.0.6: Documented, changed back to static timesteps
//0.0.7: Benchmarked calculations with known results.
//0.0.8: Fixed unit error (grav cnst in m^3) and changed order of vel/accel/pos so I don't calc x-pos from new vel/accel.

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
	int numParticles = 6;
	int timesteps = 35000;  //number of timesteps
	float time = 10;  //time between timesteps
	float gravconstant = 0.000000000000667428;
	Particle myParticles[numParticles];

	for(int x=0; x<numParticles; x++)
	{
		int rand1 = 0;
		int rand2 = 0;
		int rand3 = 0;
		for(int z=0; z<100; z++)
			rand1 = rand1 + (rand()%32767); 			//Random number generators for distance, from 0km to 4.3 times the distance between the Earth and Moon (1638350km)
		for(int z=0; z<100; z++)
			rand2 = rand2 + (rand()%32767);
		for(int z=0; z<100; z++)
			rand3 = rand3 + (rand()%32767);
		myParticles[x].x = rand1 - 1638350;
		//	myParticles[x].y = rand2 - 1638350;
		//	myParticles[x].z = rand3 - 1638350;
		myParticles[x].y = 0; 							 //overwriting previous random generations
		myParticles[x].z = 0;
		myParticles[x].xv = 0;
		myParticles[x].yv = 0;
		myParticles[x].zv = 0;
		myParticles[x].mass = 5974200000000000000000000;    //Mass of the Earth
	//	cout << endl << "[" << myParticles[x].x << "], [" << myParticles[x].y << "], [" << myParticles[x].z << "]. " << "Mass: " << myParticles[x].mass;
	}
//	myParticles[0].x = -3000.0; 							 //overwriting previous random generations
//	myParticles[1].x = -2000.0;
//	myParticles[2].x = -1000.0;
//	myParticles[3].x = 1000.0;
//	myParticles[4].x = 2000.0;
//	myParticles[5].x = 3000.0;

	cout << endl << "[" << myParticles[0].x << "], [" << myParticles[0].y << "], [" << myParticles[0].z << "]. " << "Mass: " << myParticles[0].mass;
	cout << endl << "[" << myParticles[1].x << "], [" << myParticles[1].y << "], [" << myParticles[1].z << "]. " << "Mass: " << myParticles[1].mass;
	ofstream myfile ("myfile.csv");
	for(int z=0; z<timesteps; z++)
	{
		for(int x=0; x<numParticles; x++) 					//main loop, cycles through every particle
		{
			float xSum = 0.0; 								//sum of the total grav. forces acting on a particle
			float ySum = 0.0;
			float zSum = 0.0;
			for(int y=0; y<numParticles; y++)  			 //nested loop, cycles through every particle
			{
				if(y!=x)								 //prevent calculations by a particle on itself
				{
					float distance = ((sqrt(pow((myParticles[y].x - myParticles[x].x),2) + pow((myParticles[y].y - myParticles[x].y),2) + pow((myParticles[y].z - myParticles[x].z),2))));
					//distance calculation
					if(distance == 0)
						distance = 0.00000000001;		 //prevent division by zero
					float gravity = ((gravconstant*(myParticles[x].mass * myParticles[y].mass)) / pow(distance,2)); //force calculation (F=(G*M1*M2)/d^2)
					xSum += (gravity*((myParticles[y].x-myParticles[x].x)/distance));  //sum all the forces acting on the particle from every other particle
					ySum += (gravity*((myParticles[y].y-myParticles[x].y)/distance));  //into a net force
					zSum += (gravity*((myParticles[y].z-myParticles[x].z)/distance));
				}
			}
			myParticles[x].forcex = xSum;
			myParticles[x].forcey = ySum;
			myParticles[x].forcez = zSum;
		}
		for(int x=0; x<numParticles; x++)
		{
			myParticles[x].xaccel = (myParticles[x].forcex/myParticles[x].mass);
			myParticles[x].yaccel = (myParticles[x].forcey/myParticles[x].mass);
			myParticles[x].zaccel = (myParticles[x].forcez/myParticles[x].mass);
			myParticles[x].x = myParticles[x].x + (myParticles[x].xv * time) + (0.5*(myParticles[x].xaccel*pow(time,2))); 		//calculate new position
			myParticles[x].y = myParticles[x].y + (myParticles[x].yv * time) + (0.5*(myParticles[x].yaccel*pow(time,2)));
			myParticles[x].z = myParticles[x].z + (myParticles[x].zv * time) + (0.5*(myParticles[x].zaccel*pow(time,2)));
			myParticles[x].xv = myParticles[x].xv + (myParticles[x].xaccel*time); 												//calculate velocity
			myParticles[x].yv = myParticles[x].yv + (myParticles[x].yaccel*time);
			myParticles[x].zv = myParticles[x].zv + (myParticles[x].zaccel*time);
	//		myParticles[x].xv = (myParticles[x].xv + ((0.5*(xacceldupe + myParticles[x].xaccel))*time));
	//		myParticles[x].yv = (myParticles[x].yv + ((0.5*(yacceldupe + myParticles[x].yaccel))*time));
	//		myParticles[x].zv = (myParticles[x].zv + ((0.5*(zacceldupe + myParticles[x].zaccel))*time));
		//	myParticles[x].x = (myParticles[x].x + (myParticles[x].xv * time) + (0.5*(myParticles[x].xaccel * pow(time,2))));
		//	myParticles[x].y = (myParticles[x].y + (myParticles[x].yv * time) + (0.5*(myParticles[x].yaccel * pow(time,2))));
		//	myParticles[x].z = (myParticles[x].z + (myParticles[x].zv * time) + (0.5*(myParticles[x].zaccel * pow(time,2))));


		}
		if (myfile.is_open())
		{
		//	myfile << myParticles[0].x << "," << myParticles[0].xv << "," << myParticles[0].forcex << "," << myParticles[1].x << "," << myParticles[1].xv << "," << myParticles[1].forcex << endl;
			myfile << myParticles[0].x << "," << myParticles[1].x << "," << myParticles[2].x << "," << myParticles[3].x << "," << myParticles[4].x << "," << myParticles[5].x << endl;
		}
		else cout << "\nUnable to open";
	}
	myfile.close();
}


