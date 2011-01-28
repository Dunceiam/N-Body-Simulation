#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <stdio.h>
#include <math.h>
#include <windows.h>

#include "float3.h"

#define WIDTH           1920
#define HEIGHT          1080
#define ROTATE_SPEED    0.01
#define HEIGHT_SPEED    0.1

#define NUM_THREADS     8
#define NUM_PARTICLES   1000
#define TIME            86400                 // s   (seconds per frame)
#define TIMESTEPS       1              		 // s-1 (timesteps per time)
#define GRAV_CONSTANT   0.00000000006673  	 // m3 kg-1 s-2
#define UNIVERSIZE      1000000000000.0            // km
#define MASS            99446000000000000000000000000.0 	 // kg
#define VARMASS         198892000000000000000000000000.0;
#define VARVELOCITY     10000.0
#define SOFTEN          1000000000000.0            // km (empirically derived value should be slightly more than UNIVERSIZE)

#define RADIUS          0.003   				 //best value is 0.01
#define SLICES          8

struct Particle {
   float3 pos;
   float3 vel;
   float3 acl;
   float mass;
};

using namespace std;

// Generates random float from 0.0 to 1.0
float rand2() {
   float temp = (float) rand() / (float) RAND_MAX;
   if(temp == 0.0)
      rand2();
   return temp;
}

Particle myParticles[NUM_PARTICLES];
float viewAngle = 1.0f;
float viewDistance = 4.8f;
float viewHeight = 0.3f;

void *updateParticlesThread(void *ptr) {
   int i = *((int*) ptr); // I am thread i
   int from = i * (NUM_PARTICLES / NUM_THREADS);
   int to = from + (NUM_PARTICLES / NUM_THREADS); // Same as (i + 1) * (PARTICLES * THREADS)

   for (int t2 = 0; t2 < TIMESTEPS; ++t2) {
         for (int x = from; x < to; ++x) {
               Particle* p = &myParticles[x]; // using a pointer to reduce typing
               p->acl = make_float3(0.0); // set acl to 0

               for (int y = 0; y < NUM_PARTICLES; ++y) {
                     Particle* p2 = &myParticles[y]; // using a pointer to reduce typing
                     if (x != y) {
                           float distance = length(p2->pos - p->pos);
                           float gravity = GRAV_CONSTANT * p->mass * p2->mass / (pow(distance, 2.0) + SOFTEN * SOFTEN);
                           p->acl += gravity * (p2->pos - p->pos) / distance; // add the force to the acceleration
                     }
               }

               p->acl /= p->mass; // divide by mass to get the real acceleration (it was force up to this point)
         }
         for (int x = from; x < to; ++x) {
               Particle* p = &myParticles[x]; // using a pointer to reduce typing

               float deltat = (float) TIME / (float) TIMESTEPS;

               float3 vel = p->vel;

               p->vel += p->acl * deltat;
               p->pos += vel * deltat + 0.5 * p->acl * pow(deltat, 2.0);

               // if the position is outside the bounds AND the position and velocity have the same sign (so the velocity is taking the object outside the box), then flip the velocity sign
          /*     if (abs(p->pos.x) > UNIVERSIZE && p->pos.x / p->vel.x > 0.0) { p->vel.x = -p->vel.x; }
               if (abs(p->pos.y) > UNIVERSIZE && p->pos.y / p->vel.y > 0.0) { p->vel.y = -p->vel.y; }
               if (abs(p->pos.z) > UNIVERSIZE && p->pos.z / p->vel.z > 0.0) { p->vel.z = -p->vel.z; } */
         }
   }
   return 0;
}

void updateParticles() {
   HANDLE threads[NUM_THREADS]; // Array with all of the threads
   int ids[NUM_THREADS];

   for (int i = 0; i < NUM_THREADS; ++i) {
         // Can't pass in just "i" to the function below, as it will keep getting updated
         ids[i] = i;

         // Create each of the threads, passing in the function to call (updateParticlesThread) and the parameter (i)
         threads[i] = CreateThread(NULL, 0,(LPTHREAD_START_ROUTINE) updateParticlesThread, (void*) &ids[i], 0, NULL);
   }

   // All threads are running at full speed at this point

   for (int i = 0; i < NUM_THREADS; ++i) {
         // Wait for each thread to finish
         WaitForSingleObject(threads[i], INFINITE);
   }
}

void drawParticles() {
   for (int i = 0; i < NUM_PARTICLES; i++) {
         Particle* p = &myParticles[i]; // using a pointer to reduce typing

         glPushMatrix();
         // draw a sphere at the origin (0,0,0) and translate it to its final position
         glTranslatef(p->pos.x / UNIVERSIZE, p->pos.y / UNIVERSIZE, p->pos.z / UNIVERSIZE);
         glutSolidSphere(RADIUS, SLICES, SLICES);
         glPopMatrix();
   }
}

void drawRest() {
   glutWireCube(2.0f); // draw a cube with sides of 2
}

void display(void) {
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear the last frame
   glLoadIdentity();
      gluPerspective(50.0, 1.777, 0.01, 10.0); // fov, apsect ratio, znear, zfar
      glMatrixMode(GL_MODELVIEW); // set up camera and the scene in model view
      glLoadIdentity();
      gluLookAt(cos(viewAngle) * viewDistance, tan(viewHeight) * viewDistance, sin(viewAngle) * viewDistance,  // origin of the camera
            0.0, 0.0, 0.0,  // coordinates the camera is looking at
            0.0, 1.0, 0.0); // the up direction
   updateParticles(); // do the calculations on the particles
   drawParticles(); // draw the particles
   drawRest(); // draw the rest of the scene

   glutSwapBuffers(); // finish drawing the new frame and put it on the screen
}

void init(void) {
   GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
   GLfloat mat_shininess[] = { 50.0 };
   GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };
   glClearColor(0.0, 0.0, 0.0, 0.0);
   glShadeModel(GL_SMOOTH);

   glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
   glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
   glLightfv(GL_LIGHT0, GL_POSITION, light_position);

   glMatrixMode(GL_PROJECTION); // set up perspectives in projection view
   srand(time(NULL));

   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
   glEnable(GL_DEPTH_TEST);
}

void processMouseWheel(int button, int dir, int x, int y)
{
    if (dir > 0) {
        viewDistance -= HEIGHT_SPEED;
    }
    else {
        viewDistance += HEIGHT_SPEED;
    }
    return;
}

void processSpecialKeys(int key, int x, int y) {
   switch(key) {
   case GLUT_KEY_LEFT:
      viewAngle += ROTATE_SPEED;
      break;
   case GLUT_KEY_RIGHT:
      viewAngle -= ROTATE_SPEED;
      break;
   case GLUT_KEY_UP:
      viewHeight += ROTATE_SPEED;
      break;
   case GLUT_KEY_DOWN:
      viewHeight -= ROTATE_SPEED;
      break;
   }
   glutMouseWheelFunc(processMouseWheel);
}

void menu(void) {
   int option, perpendic = 0;
   cout << " 0. Horizontal Disk\n";
   cout << " 1. Horizontal Disk w/ Velocity\n";
   cout << " 2. Horizontal Disk (Variable Mass)\n";
   cout << " 3. Horizontal Disk w/ Velocity (Variable Mass)\n";
   cout << " 4. Horizontal + Vertical Disk\n";
   cout << " 5. Horizontal + Vertical Disk w/ Velocity \n";
   cout << " 6. Horizontal + Vertical Disk (Variable Mass)\n";
   cout << " 7. Horizontal + Vertical Disk w/ Velocity (Variable Mass)\n";
   cout << " 8. Random\n";
   cout << " 9. Random w/ Velocity\n";
   cout << "10. Random w/ Velocity (Variable Mass)\n";
   cout << "Option: ";
   cin >> option;
   switch(option) {
   case 0:
      for (int x = 0; x < NUM_PARTICLES; ++x) {
            myParticles[x].pos = make_float3((rand2() * 2.0 - 1.0)* UNIVERSIZE, (rand2() * 2.0 - 1.0)* UNIVERSIZE/10, (rand2() * 2.0 - 1.0)* UNIVERSIZE);
            myParticles[x].vel = make_float3(0.0);
            myParticles[x].acl = make_float3(0.0);
            myParticles[x].mass = MASS;
      }
      break;
   case 2:
      for (int x = 0; x < NUM_PARTICLES; ++x) {
            myParticles[x].pos = make_float3((rand2() * 2.0 - 1.0)* UNIVERSIZE, (rand2() * 2.0 - 1.0)* UNIVERSIZE/10, (rand2() * 2.0 - 1.0)* UNIVERSIZE);
            myParticles[x].vel = make_float3(0.0);
            myParticles[x].acl = make_float3(0.0);
            myParticles[x].mass = rand2() * VARMASS;
      }
      break;
   case 4:
      for (int x = 0; x < NUM_PARTICLES; ++x) {
            if(perpendic == 0) {
                  myParticles[x].pos = make_float3((rand2() * 2.0 - 1.0)* UNIVERSIZE, (rand2() * 2.0 - 1.0)* UNIVERSIZE/10, (rand2() * 2.0 - 1.0)* UNIVERSIZE); //flat
                  myParticles[x].vel = make_float3(0.0);
                  myParticles[x].acl = make_float3(0.0);
                  myParticles[x].mass = MASS;
                  perpendic = 1;
            }
            else {
                  myParticles[x].pos = make_float3((rand2() * 2.0 - 1.0)* UNIVERSIZE/10, (rand2() * 2.0 - 1.0)* UNIVERSIZE , (rand2() * 2.0 - 1.0)* UNIVERSIZE);  //tall
                  myParticles[x].vel = make_float3(0.0);
                  myParticles[x].acl = make_float3(0.0);
                  myParticles[x].mass = MASS;
                  perpendic = 0;
            }
      }
   case 6:
      for (int x = 0; x < NUM_PARTICLES; ++x) {
            if(perpendic == 0) {
                  myParticles[x].pos = make_float3((rand2() * 2.0 - 1.0)* UNIVERSIZE, (rand2() * 2.0 - 1.0)* UNIVERSIZE/10, (rand2() * 2.0 - 1.0)* UNIVERSIZE); //flat
                  myParticles[x].vel = make_float3(0.0);
                  myParticles[x].acl = make_float3(0.0);
                  myParticles[x].mass = rand2() * VARMASS;
                  perpendic = 1;
            }
            else {
                  myParticles[x].pos = make_float3((rand2() * 2.0 - 1.0)* UNIVERSIZE/10, (rand2() * 2.0 - 1.0)* UNIVERSIZE , (rand2() * 2.0 - 1.0)* UNIVERSIZE);  //tall
                  myParticles[x].vel = make_float3(0.0);
                  myParticles[x].acl = make_float3(0.0);
                  myParticles[x].mass = rand2() * VARMASS;
                  perpendic = 0;
            }
      }
      break;
   case 1:
      for (int x = 0; x < NUM_PARTICLES; ++x) {
            myParticles[x].pos = make_float3((rand2() * 2.0 - 1.0)* UNIVERSIZE, (rand2() * 2.0 - 1.0)* UNIVERSIZE/10, (rand2() * 2.0 - 1.0)* UNIVERSIZE);
            myParticles[x].vel = make_float3((rand2() * 2.0 - 1.0) * VARVELOCITY, 0.0, (rand2() * 2.0 - 1.0) * VARVELOCITY);
            myParticles[x].acl = make_float3(0.0);
            myParticles[x].mass = MASS;
      }
      break;
   case 3:
      for (int x = 0; x < NUM_PARTICLES; ++x) {
            myParticles[x].pos = make_float3((rand2() * 2.0 - 1.0)* UNIVERSIZE, (rand2() * 2.0 - 1.0)* UNIVERSIZE/10, (rand2() * 2.0 - 1.0)* UNIVERSIZE);
            myParticles[x].vel = make_float3((rand2() * 2.0 - 1.0) * VARVELOCITY, 0.0, (rand2() * 2.0 - 1.0) * VARVELOCITY);
            myParticles[x].acl = make_float3(0.0);
            myParticles[x].mass = rand2() * VARMASS;
      }
      break;
   case 5:
      for (int x = 0; x < NUM_PARTICLES; ++x) {
            if(perpendic == 0) {
                  myParticles[x].pos = make_float3((rand2() * 2.0 - 1.0)* UNIVERSIZE, (rand2() * 2.0 - 1.0)* UNIVERSIZE/10, (rand2() * 2.0 - 1.0)* UNIVERSIZE); //flat
                  myParticles[x].vel = make_float3((rand2() * 2.0 - 1.0) * VARVELOCITY, 0.0, (rand2() * 2.0 - 1.0) * VARVELOCITY);
                  myParticles[x].acl = make_float3(0.0);
                  myParticles[x].mass = MASS;
                  perpendic = 1;
            }
            else {
                  myParticles[x].pos = make_float3((rand2() * 2.0 - 1.0)* UNIVERSIZE/10, (rand2() * 2.0 - 1.0)* UNIVERSIZE , (rand2() * 2.0 - 1.0)* UNIVERSIZE);  //tall
                  myParticles[x].vel = make_float3(0.0, (rand2() * 2.0 - 1.0) * VARVELOCITY, (rand2() * 2.0 - 1.0) * VARVELOCITY);
                  myParticles[x].acl = make_float3(0.0);
                  myParticles[x].mass = MASS;
                  perpendic = 0;
            }
      }
      break;
   case 7:
      for (int x = 0; x < NUM_PARTICLES; ++x) {
            if(perpendic == 0) {
                  myParticles[x].pos = make_float3((rand2() * 2.0 - 1.0)* UNIVERSIZE, (rand2() * 2.0 - 1.0)* UNIVERSIZE/10, (rand2() * 2.0 - 1.0)* UNIVERSIZE); //flat
                  myParticles[x].vel = make_float3((rand2() * 2.0 - 1.0) * VARVELOCITY, 0.0, (rand2() * 2.0 - 1.0) * VARVELOCITY);
                  myParticles[x].acl = make_float3(0.0);
                  myParticles[x].mass = rand2() * VARMASS;
                  perpendic = 1;
            }
            else {
                  myParticles[x].pos = make_float3((rand2() * 2.0 - 1.0)* UNIVERSIZE/10, (rand2() * 2.0 - 1.0)* UNIVERSIZE , (rand2() * 2.0 - 1.0)* UNIVERSIZE);  //tall
                  myParticles[x].vel = make_float3(0.0, (rand2() * 2.0 - 1.0) * VARVELOCITY, (rand2() * 2.0 - 1.0) * VARVELOCITY);
                  myParticles[x].acl = make_float3(0.0);
                  myParticles[x].mass = rand2() * VARMASS;
                  perpendic = 0;
            }
      }
      break;
   case 8:
      for (int x = 0; x < NUM_PARTICLES; ++x) {
            myParticles[x].pos = make_float3(rand2() * 2.0 - 1.0, rand2() * 2.0 - 1.0, rand2() * 2.0 - 1.0) * UNIVERSIZE;
            myParticles[x].vel = make_float3(0.0);
            myParticles[x].acl = make_float3(0.0);
            myParticles[x].mass = MASS;
      }
      break;
   case 9:
      for (int x = 0; x < NUM_PARTICLES; ++x) {
            myParticles[x].pos = make_float3(rand2() * 2.0 - 1.0, rand2() * 2.0 - 1.0, rand2() * 2.0 - 1.0) * UNIVERSIZE;
            myParticles[x].vel = make_float3(rand2() * 2.0 - 1.0, rand2() * 2.0 - 1.0, rand2() * 2.0 - 1.0) * VARVELOCITY;
            myParticles[x].acl = make_float3(0.0);
            myParticles[x].mass = MASS;
      }
      break;
   case 10:
      for (int x = 0; x < NUM_PARTICLES; ++x) {
            myParticles[x].pos = make_float3(rand2() * 2.0 - 1.0, rand2() * 2.0 - 1.0, rand2() * 2.0 - 1.0) * UNIVERSIZE;
            myParticles[x].vel = make_float3(rand2() * 2.0 - 1.0, rand2() * 2.0 - 1.0, rand2() * 2.0 - 1.0) * VARVELOCITY;
            myParticles[x].acl = make_float3(0.0);
            myParticles[x].mass = rand2() * VARMASS;
      }
      break;
   }
}

int main(int argc, char **argv) {
   menu();
   glutInit(&argc, argv); // init glut
   glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); // double frame buffer, rgb colours, and depth detection
   glutInitWindowSize(WIDTH, HEIGHT);
   glutCreateWindow("N-body Gravitational Simulation"); // title of the window
   glutDisplayFunc(display); // function called to render the window
   glutIdleFunc(display); // function called when window is idle (no need to update)
   glutSpecialFunc(processSpecialKeys);
   glutMouseWheelFunc(processMouseWheel);
   init(); // set up variables
   glutMainLoop(); // go into an infinite loop
   return 0;
}
