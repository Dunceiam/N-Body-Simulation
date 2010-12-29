#include <GL/freeglut.h>
#include <stdlib.h>
#include <time.h>

#include <iostream>
#include <stdio.h>
#include <math.h>
#include <fstream>

#include "float3.h"

#define NUM_PARTICLES   8
#define PER_RECORDING   1               // s
#define TIMESTEPS       100             // s-1
#define GRAV_CONSTANT   0.00000000006673 // m3 kg-1 s-2

#define UNIVERSIZE      10000.0
#define MASS            5974200000000000.0
#define SOFTEN          1000000.0

struct Particle {
	float3 pos;
	float3 vel;
	float3 acl;
	float3 frc;
	float mass;
};

#define particle_radius 0.05
#define particle_slices 8

float rand2() {
	return (float) rand() / (float) RAND_MAX;
}

Particle myParticles[NUM_PARTICLES];

void updateParticles() {
	float3 forces[NUM_PARTICLES][NUM_PARTICLES];
	for (int t2 = 0; t2 < TIMESTEPS; ++t2) {
		for (int x = 0; x < NUM_PARTICLES; ++x) {
		  myParticles[x].frc = make_float3(0.0);
		  
			for (int y = 0; y < NUM_PARTICLES; ++y) {
		    if (x != y) {
			    if (x < y) {
			      float distance = length(myParticles[y].pos - myParticles[x].pos);
				    float gravity = GRAV_CONSTANT * myParticles[x].mass * myParticles[y].mass / (pow(distance, 2.0) + SOFTEN);
				
				    forces[x][y] = gravity * (myParticles[y].pos - myParticles[x].pos) / distance;
			    } else { forces[x][y] = -forces[y][x]; }
			  }
			
			  myParticles[x].frc += forces[x][y];
			}
		}
		
		for (int x = 0; x < NUM_PARTICLES; ++x) {
		  myParticles[x].acl = myParticles[x].frc / myParticles[x].mass;
		  
		  float3 acl = myParticles[x].acl * (float) PER_RECORDING / (float) TIMESTEPS;
		  float3 vel = myParticles[x].vel * (float) PER_RECORDING / (float) TIMESTEPS;
			
			myParticles[x].vel += acl;
			myParticles[x].pos += vel;
			
			if (abs(myParticles[x].pos.x) > UNIVERSIZE) { myParticles[x].vel.x = -myParticles[x].vel.x; }
			if (abs(myParticles[x].pos.y) > UNIVERSIZE) { myParticles[x].vel.y = -myParticles[x].vel.y; }
			if (abs(myParticles[x].pos.z) > UNIVERSIZE) { myParticles[x].vel.z = -myParticles[x].vel.z; }
		}
	}
}

void drawParticles() {
  for (int i = 0; i < NUM_PARTICLES; i++) {
    Particle* p = &myParticles[i];
  
    glPushMatrix();
      glTranslatef(p->pos.x / UNIVERSIZE, p->pos.y / UNIVERSIZE, p->pos.z / UNIVERSIZE);
      glutSolidSphere(particle_radius, particle_slices, particle_slices);
    glPopMatrix();
  }
}

void drawRest() {
  glutWireCube(2.0f);
}

void display(void) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  updateParticles();
  drawParticles();
  drawRest();
  
  glutSwapBuffers();
}

void init(void) {
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(70.0, 1.0, 1.0, 10.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(0.5, 0.0, 2.5,
            0.0, 0.0, 0.0,
            0.0, 1.0, 0.0);

  srand(time(NULL));
  for (int x = 0; x < NUM_PARTICLES; ++x) {
		myParticles[x].pos = make_float3(rand2() * 2.0 - 1.0, rand2() * 2.0 - 1.0, rand2() * 2.0 - 1.0) * UNIVERSIZE;
		myParticles[x].vel = make_float3(rand2() * 2.0 - 1.0, rand2() * 2.0 - 1.0, rand2() * 2.0 - 1.0) * 2.0;
		myParticles[x].acl = make_float3(0.0);
		myParticles[x].frc = make_float3(0.0);
		myParticles[x].mass = MASS;
	}
}

int main(int argc, char **argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutCreateWindow("Dunceiam");
  glutDisplayFunc(display);
  glutIdleFunc(display);
  
  init();
  
  glutMainLoop();
  
  return 0;
}
