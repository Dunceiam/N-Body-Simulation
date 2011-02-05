#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h>
#include <stdlib.h>
#include <TIME.h>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <math.h>
#include <windows.h>
#include <sstream>
#include <string.h>
#include <stdarg.h>
#include <cstdlib>

#include "float3.h"

#define WIDTH           1920
#define HEIGHT          1080
#define ROTATE_SPEED    0.01
#define HEIGHT_SPEED    0.1

#define NUM_THREADS     8
int TIME =              250000;                 // s   (seconds per frame)
int TIMESTEPS =         1;                        // s-1 (TIMESTEPS per TIME)
float UNIVERSIZE =      1000000000000.0;            // 1 trillion km
#define GRAV_CONSTANT   0.00000000006673         // m3 kg-1 s-2
#define MASS            99446000000000000000000000000.0          // half mass of the sun kg
#define VARMASS         198892000000000000000000000000.0;        // mass of the sun kg
#define VARVELOCITY     25000.0
#define SOFTEN          1000000000000.0            // km (empirically derived value should be slightly more than UNIVERSIZE)

#define RADIUS          0.003                                    //best value is 0.01
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

Particle* myParticles;
float viewAngle = 1.0f;
float viewDistance = 4.8f;
float viewHeight = 0.3f;
int frameCount, mode, yearCount, option, tell, NUM_PARTICLES  = 0;
char* pathname;
bool restart = false, quit = false;

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

void readParticles() {
   string line;
   int ident, n = 0;
   double coord = 0.0;
   ifstream myfile;
   myfile.open (pathname, ifstream::in);
   if(myfile.is_open()) {
         myfile.seekg(tell);
         getline(myfile,line);
   }
   tell = myfile.tellg();
   myfile.close();
   for(int x = 0; x < NUM_PARTICLES; x++) {
         for(int y = 0; y < 3; y++) {
               n = line.find_first_of(',');
               coord = atof(line.substr(0,n).c_str());
               line.erase(0,n);
               switch(ident) {
               case 0:
                  myParticles[x].pos.x = coord;
                  ident = 1;
                  break;
               case 1:
                  myParticles[x].pos.y = coord;
                  ident = 2;
                  break;
               case 2:
                  myParticles[x].pos.z = coord;
                  ident = 0;
                  break;
               }
         }
   }
}

void updateParticles() {
   if(mode != 2)
      {
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
         if(mode == 1) {
               ofstream outfile(pathname, ios::app);
               for(int x = 0; x < NUM_PARTICLES; x++) {
                     outfile << myParticles[x].pos.x << "," << myParticles[x].pos.y << "," << myParticles[x].pos.z << ",";
               }
               outfile << endl;
               outfile.close();
         }
      }
   else {
         readParticles();
   }
   frameCount++;
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
void stroke_output(GLfloat x, GLfloat y, GLfloat z, int ident, char *format,...) {
   va_list args;
   char buffer[200], *p;

   va_start(args, format);
   vsprintf(buffer, format, args);
   va_end(args);
   glPushMatrix();
   glTranslatef(x, y, z);
   glScalef(0.0005, 0.0005, 0.0005);
   //  GLfloat mat_emission[] = {0.2, 0.2, 0.2, 0.0};
   // glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, mat_emission);
   switch(ident) {
   case 1:
      glRotatef(180.0, 0.0, 1.0, 0.0);
      break;
   case 2:
      glRotatef(90.0, 0.0, 1.0, 0.0);
      break;
   case 3:
      glRotatef(270.0, 0.0, 1.0, 0.0);
      break;
   }
   for (p= buffer; *p; p++) {
         glutStrokeCharacter(GLUT_STROKE_ROMAN, *p);
   }
   glPopMatrix();
}

void drawRest() {
   glutWireCube(2.0f); // draw a cube with sides of 2
   int actualCount = (frameCount*(TIME/TIMESTEPS))/31536000;  //years
   char marcus[100];
   itoa(actualCount, marcus, 10);
   strcat(marcus," years");
   stroke_output(0.7, 0.9, 1.0, 0, marcus);
   stroke_output(-0.7, 0.9, -1.0, 1, marcus);
   stroke_output(1.0, 0.9, -0.7, 2, marcus);
   stroke_output(-1.0, 0.9, 0.7, 3, marcus);
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
   drawParticles(); // draw the particles
   drawRest(); // draw the rest of the scene

   glutSwapBuffers(); // finish drawing the new frame and put it on the screen
}

void init(void) {
   GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
   GLfloat mat_shininess[] = { 50.0 };
   GLfloat light0_position[] = { 1.0, 1.0, 1.0, 0.0 };
   GLfloat light0_diffuse[] = {1.0, 1.0, 1.0, 1.0};
   GLfloat light0_specular[] = {1.0, 1.0, 1.0, 1.0};
   GLfloat light1_position[] = { -1.0, -1.0, -1.0, 0.0 };
   GLfloat light1_diffuse[] = {1.0, 1.0, 1.0, 1.0};
   GLfloat light1_specular[] = {1.0, 1.0, 1.0, 1.0};
   glClearColor(0.0, 0.0, 0.0, 0.0);
   glShadeModel(GL_SMOOTH);
   glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
   glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
   glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);
   glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
   glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
   glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);
   glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
   glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

   glMatrixMode(GL_PROJECTION); // set up perspectives in projection view
   srand(time(NULL));

   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
   glEnable(GL_LIGHT1);
   glEnable(GL_DEPTH_TEST);
}

void menu(void) {
   if(!restart) {
         cout << "0: Display \n";
         cout << "1: Display (Write)\n";
         cout << "2: Display (Read)\n";
         cin >> mode;
         if(mode == 2) {
               cout << "File: ";
               cin >> pathname;
               string line;
               ifstream myfile;
               myfile.open (pathname, ifstream::in);
               if(myfile.is_open()) {
                     myfile.seekg(tell);
                     getline(myfile,line);
               }
               tell = myfile.tellg();
               myfile.close();
               int n = line.find_first_of(',');
               NUM_PARTICLES = atof(line.substr(0,n).c_str());
               line.erase(0,n);
               n = line.find_first_of(',');
               TIME = atof(line.substr(0,n).c_str());
               line.erase(0,n);
               n = line.find_first_of(',');
               TIMESTEPS = atof(line.substr(0,n).c_str());
               line.erase(0,n);
               n = line.find_first_of(',');
               UNIVERSIZE = atof(line.substr(0,n).c_str());
               myParticles = new Particle[NUM_PARTICLES];
         }
         else {
               cout << "\n 0. Horizontal Disk\n";
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
               cout << "\n Number of Particles: ";
               cin >> NUM_PARTICLES;
               myParticles = new Particle[NUM_PARTICLES];
         }
         if(mode == 1) {
               cout << "\nNumber of years: ";
               cin >> yearCount;
               cout << "\nFile: ";
               cin >> pathname;
               ofstream outfile(pathname, ios::app);
               outfile.open(pathname);
               outfile << NUM_PARTICLES << "," << TIME << "," << TIMESTEPS << "," << UNIVERSIZE;
               outfile.close();
         }
   }
   if(mode != 2) {
         //  DEFAULT = pos(rand, rand, rand)
         //          = vel(rand, rand, rand)
         //          = mass(rand), perpendic?
               bool xrand = true, yrand = true, zrand = true;
               bool vxrand = true, vzrand = true, vyrand = true, velocity = false;
               bool massrand = true, perp = false;
               switch(option) {
               case 0:
                  massrand = false, yrand = false;
                  break;
               case 2:
                  yrand = false;
                  break;
               case 4:
                  perp = true, massrand = false;
                  break;
               case 6:
                  perp = true;
                  break;
               case 1:
                  yrand = false, massrand = false, velocity = true;
                  break;
               case 3:
                  yrand = false, velocity = true;
                  break;
               case 5:
                  perp = true, massrand = false, velocity = true;
                  break;
               case 7:
                  perp = true, velocity = true;
                  break;
               case 8:
                  massrand = false;
                  break;
               case 9:
                  massrand = false, velocity = true;
                  break;
               case 10:
                  velocity = true;
                  break;
               }
               int switcher = 0;
               if(perp == true) {
                     for(int x = 0; x < NUM_PARTICLES; ++x) {
                           if(switcher == 0) {
                                 xrand = true, yrand = false, zrand = true;
                                 float r = rand2()*UNIVERSIZE;
                                 float angle = rand2()*3.14;
                                 float angle2 = rand2()*6.28;
                                 myParticles[x].pos = make_float3(xrand? sin(angle)*cos(angle2)*r :(sin(angle)*cos(angle2)*r)/10, yrand? sin(angle)*sin(angle2)*r :(sin(angle)*sin(angle2)*r)/10, zrand? cos(angle)*r :(cos(angle)*r)/10);
                                 if((myParticles[x].pos.x > 0) && (myParticles[x].pos.z < 0))
                                    vxrand = false, vzrand = false;
                                 else if((myParticles[x].pos.x > 0) && (myParticles[x].pos.z > 0))
                                    vxrand = true, vzrand = false;
                                 else if((myParticles[x].pos.x < 0) && (myParticles[x].pos.z > 0))
                                    vxrand = true, vzrand = true;
                                 else if((myParticles[x].pos.x < 0) && (myParticles[x].pos.z < 0))
                                    vxrand = false, vzrand = true;
                                 if(velocity)
                                    myParticles[x].vel = make_float3(vxrand? (rand2()) * VARVELOCITY :(rand2() - 1.0) * VARVELOCITY, 0.0,vzrand? (rand2()) * VARVELOCITY :(rand2() - 1.0) * VARVELOCITY);
                                 else
                                    myParticles[x].vel = make_float3(0.0);
                                 if(massrand)
                                    myParticles[x].mass = rand2() * VARMASS;
                                 if(massrand == false)
                                    myParticles[x].mass = MASS;
                                 myParticles[x].acl = make_float3(0.0);
                                 switcher = 1;
                           }
                           else {
                                 xrand = false; yrand = true, zrand = true;
                                 float r = rand2()*UNIVERSIZE;
                                 float angle = rand2()*3.14;
                                 float angle2 = rand2()*6.28;
                                 myParticles[x].pos = make_float3(xrand? sin(angle)*cos(angle2)*r :(sin(angle)*cos(angle2)*r)/10, yrand? sin(angle)*sin(angle2)*r :(sin(angle)*sin(angle2)*r)/10, zrand? cos(angle)*r :(cos(angle)*r)/10);
                                 if((myParticles[x].pos.y > 0) && (myParticles[x].pos.z < 0))
                                    vyrand = true, vzrand = true;
                                 else if((myParticles[x].pos.y > 0) && (myParticles[x].pos.z > 0))
                                    vyrand = false, vzrand = true;
                                 else if((myParticles[x].pos.y < 0) && (myParticles[x].pos.z > 0))
                                    vyrand = false, vzrand = false;
                                 else if((myParticles[x].pos.y < 0) && (myParticles[x].pos.z < 0))
                                    vyrand = true, vzrand = false;
                                 if(velocity)
                                    myParticles[x].vel = make_float3(0.0, vyrand? (rand2()) * VARVELOCITY :(rand2() - 1.0) * VARVELOCITY, vzrand? (rand2()) * VARVELOCITY :(rand2() - 1.0) * VARVELOCITY);
                                 else
                                    myParticles[x].vel = make_float3(0.0);
                                 if(massrand)
                                    myParticles[x].mass = rand2() * VARMASS;
                                 if(massrand == false)
                                    myParticles[x].mass = MASS;
                                 myParticles[x].acl = make_float3(0.0);
                                 switcher = 0;
                           }
                     }
               }
               else {
                     for (int x = 0; x < NUM_PARTICLES; ++x) {
                           //     myParticles[x].pos = make_float3(xrand? (rand2() * 2.0 - 1.0)* UNIVERSIZE :(rand2() * 2.0 - 1.0)* UNIVERSIZE/10, yrand? (rand2() * 2.0 - 1.0)* UNIVERSIZE :(rand2() * 2.0 - 1.0)* UNIVERSIZE/10, zrand? (rand2() * 2.0 - 1.0)* UNIVERSIZE :(rand2() * 2.0 - 1.0)* UNIVERSIZE/10);
                           float r = rand2()*UNIVERSIZE;
                           float angle = rand2()*3.14;
                           float angle2 = rand2()*6.28;
                           myParticles[x].pos = make_float3(xrand? sin(angle)*cos(angle2)*r :(sin(angle)*cos(angle2)*r)/10, yrand? sin(angle)*sin(angle2)*r :(sin(angle)*sin(angle2)*r)/10, zrand? cos(angle)*r :(cos(angle)*r)/10);
                           if((myParticles[x].pos.x > 0) && (myParticles[x].pos.z < 0))                  //back-left corner is false false
                              vxrand = false, vzrand = false;                                            //close-right corner is true true
                           else if((myParticles[x].pos.x > 0) && (myParticles[x].pos.z > 0))             //back-right corner is true false
                              vxrand = true, vzrand = false;                                             //close-left corner is false true
                           else if((myParticles[x].pos.x < 0) && (myParticles[x].pos.z > 0))             //neg x is to the left
                              vxrand = true, vzrand = true;                                              //pos z is close to me
                           else if((myParticles[x].pos.x < 0) && (myParticles[x].pos.z < 0))
                              vxrand = false, vzrand = true;
                           if(velocity)
                              myParticles[x].vel = make_float3(vxrand? (rand2()) * VARVELOCITY :(rand2() - 1.0) * VARVELOCITY, 0.0,vzrand? (rand2()) * VARVELOCITY :(rand2() - 1.0) * VARVELOCITY);
                           else
                              myParticles[x].vel = make_float3(0.0);
                           if(massrand)
                              myParticles[x].mass = rand2() * VARMASS;
                           if(massrand == false)
                              myParticles[x].mass = MASS;
                           myParticles[x].acl = make_float3(0.0);
                     }
               }

   }
   restart = false;
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
   case GLUT_KEY_F5:
      restart = true;
      frameCount = 0;
      menu();
      break;
   case GLUT_KEY_F8:
      quit = true;
   }
   glutMouseWheelFunc(processMouseWheel);
}

void calculateLoop() {
   while(quit = false || totalYears = false) {
         updateParticles();
   }
   glutDestroyWindow(1);
   delete[] myParticles;
   exit(0);
}

int main(int argc, char **argv) {
   menu();
   glutInit(&argc, argv); // init glut
   glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); // double frame buffer, rgb colours, and depth detection
   glutInitWindowSize(WIDTH, HEIGHT);
   glutCreateWindow("N-body Gravitational Simulation"); // title of the window
   glClearColor(1.0, 1.0, 1.0, 1.0);
   glColor3f(0.0, 0.0, 0.0);
   glutDisplayFunc(display); // function called to render the window
   glutIdleFunc(display); // function called when window is idle (no need to update)
   glutSpecialFunc(processSpecialKeys);
   glutMouseWheelFunc(processMouseWheel);
   init(); // set up variables
   CreateThread(NULL, 0,(LPTHREAD_START_ROUTINE) calculateLoop, 0, 0, NULL);
   glutMainLoop(); // go into an infinite loop
   return 0;
}
