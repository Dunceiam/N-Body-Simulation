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

#include "float3.h"

using namespace std;

#define NUM_PARTICLES   2
#define TIME            1000             // s
#define TIMESTEPS       10               // s-1
#define GRAV_CONSTANT   0.00000000006673 // m3 kg-1 s-2

#define UNIVERSIZE      1638350.0
#define MASS            5974200000000000000000000.0

struct Particle {
	float3 pos;
	float3 vel;
	float3 acl;
	float3 frc;
	float mass;
};

float rand2() {
  return (float) rand() / (float) RAND_MAX;
}

int main() {
	srand(time(0));
	
	Particle myParticles[NUM_PARTICLES];
	for (int x = 0; x < NUM_PARTICLES; ++x) {
		myParticles[x].pos = make_float3(rand2() * 2.0 - 1.0, rand2() * 2.0 - 1.0, rand2() * 2.0 - 1.0) * UNIVERSIZE;
		myParticles[x].vel = make_float3(0.0);
		myParticles[x].acl = make_float3(0.0);
		myParticles[x].frc = make_float3(0.0);
		myParticles[x].mass = MASS;
		cout << "[" << myParticles[x].pos.x << "], [" << myParticles[x].pos.y << "], [" << myParticles[x].pos.z << "]. " << "Mass: " << myParticles[x].mass << endl;
	}

	ofstream myfile("myfile.csv");
	if (!myfile.is_open()) {
	  cout << "Unable to open." << endl;
	  return -1;
	}
	
	for (int t = 0; t < TIME; ++t) {
	  for (int t2 = 0; t2 < TIMESTEPS; ++t2) {
		  for (int x = 0; x < NUM_PARTICLES; ++x) {
		    for (int y = 0; y < NUM_PARTICLES; ++y) {
          if (x != y) {
            
          }
        }
			}
		}
		
		for (int i = 0; i < NUM_PARTICLES; ++i) {
		  myfile << myParticles[i].pos.x << "," << myParticles[i].vel.x << "," << myParticles[i].frc.x << ",";
		}
		myfile << endl;
	}
	myfile.close();
}


