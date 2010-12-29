#include <GL/freeglut.h>
#include <stdlib.h>
#include <time.h>

struct float3 {
  float x;
  float y;
  float z;
};

struct particle {
  float3 pos;
  float3 vel;
  float3 acl;
};

#define particle_radius 0.01
#define particle_slices 8
#define particle_bounds 4096.0

#define particles_size 5

particle particles[particles_size];

void updateParticles() {
  for (int i = 0; i < particles_size; i++) {
    particle* p = &particles[i];
    
    p->pos.x += p->vel.x;
    p->pos.y += p->vel.y;
    p->pos.z += p->vel.z;
    
    if (p->pos.x < -particle_bounds || p->pos.x > particle_bounds) p->vel.x = -p->vel.x;
    if (p->pos.y < -particle_bounds || p->pos.y > particle_bounds) p->vel.y = -p->vel.y;
    if (p->pos.z < -particle_bounds || p->pos.z > particle_bounds) p->vel.z = -p->vel.z;
    
    p->vel.x += p->acl.x;
    p->vel.y += p->acl.y;
    p->vel.z += p->acl.z;
  }
}

void drawParticles() {
  for (int i = 0; i < particles_size; i++) {
    particle* p = &particles[i];
  
    glPushMatrix();
      glTranslatef(p->pos.x / particle_bounds, p->pos.y / particle_bounds, p->pos.z / particle_bounds);
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
  gluLookAt(0.5, 0.0, 2.0,
            0.0, 0.0, 0.0,
            0.0, 1.0, 0.0);

  srand(time(NULL));
  for (int i = 0; i < particles_size; ++i) {
    particle* p = &particles[i];
  
    p->pos.x = particle_bounds * ((float) (rand() % 1024) / 512.0 - 1.0);
    p->pos.y = particle_bounds * ((float) (rand() % 1024) / 512.0 - 1.0);
    p->pos.z = particle_bounds * ((float) (rand() % 1024) / 512.0 - 1.0);
    p->vel.x = (float) (rand() % 4096) / 4096.0;
    p->vel.y = (float) (rand() % 4096) / 4096.0;
    p->vel.z = (float) (rand() % 4096) / 4096.0;
    p->acl.x = 0.0f;
    p->acl.y = -9.81f / particle_bounds;
    p->acl.z = 0.0f;
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
