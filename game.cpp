#include <iostream>
#include <vector>
#include <GL/glut.h>

using namespace std;

float refreshMillis = 10;
int cube_delay_count = 0;
int new_cube_delay = 10;
vector< vector<float> > cubes;
float init_vel[] = {0.0, 0.0, 1.0};
vector<float> cur_vel (init_vel, init_vel + sizeof(init_vel) / sizeof(float));

/* Initialize OpenGL Graphics */
void initGL() {
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set background color to black and opaque
  glClearDepth(1.0f);                   // Set background depth to farthest
  glEnable(GL_DEPTH_TEST);   // Enable depth testing for z-culling
  glDepthFunc(GL_LEQUAL);    // Set the type of depth-test
}

void draw_cube(float x, float y, float z) {
  glLoadIdentity();
  glTranslatef(x, y, z);
  glBegin(GL_QUADS);

  if (y < 0) {
    // Top face (y = 1.0f)
    // Define vertices in counter-clockwise (CCW) order with normal pointing out
    glColor3f(0.0f, 1.0f, 0.0f);     // Green
    glVertex3f( 1.0f, 1.0f, -1.0f);
    glVertex3f(-1.0f, 1.0f, -1.0f);
    glVertex3f(-1.0f, 1.0f,  1.0f);
    glVertex3f( 1.0f, 1.0f,  1.0f);
  } else if (y > 0) {
    // Bottom face (y = -1.0f)
    glColor3f(1.0f, 0.5f, 0.0f);     // Orange
    glVertex3f( 1.0f, -1.0f,  1.0f);
    glVertex3f(-1.0f, -1.0f,  1.0f);
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glVertex3f( 1.0f, -1.0f, -1.0f);
  }

  if (x > 0) {
    // Left face (x = -1.0f)
    glColor3f(0.0f, 0.0f, 1.0f);     // Blue
    glVertex3f(-1.0f,  1.0f,  1.0f);
    glVertex3f(-1.0f,  1.0f, -1.0f);
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glVertex3f(-1.0f, -1.0f,  1.0f);
  } else if (x < 0) {
    // Right face (x = 1.0f)
    glColor3f(1.0f, 0.0f, 1.0f);     // Magenta
    glVertex3f(1.0f,  1.0f, -1.0f);
    glVertex3f(1.0f,  1.0f,  1.0f);
    glVertex3f(1.0f, -1.0f,  1.0f);
    glVertex3f(1.0f, -1.0f, -1.0f);
  }

  // Front face  (z = 1.0f)
  glColor3f(1.0f, 0.0f, 0.0f);     // Red
  glVertex3f( 1.0f,  1.0f, 1.0f);
  glVertex3f(-1.0f,  1.0f, 1.0f);
  glVertex3f(-1.0f, -1.0f, 1.0f);
  glVertex3f( 1.0f, -1.0f, 1.0f);

  glEnd();
}

void display() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glMatrixMode(GL_MODELVIEW);
  int num_to_delete = 0;
  for (int i = 0; i < cubes.size(); i++) {
    float coords[] = {cubes[i][0] + cur_vel[0], cubes[i][1] + cur_vel[1],
      cubes[i][2] + cur_vel[2]};
    vector<float> new_cube (coords, coords + sizeof(coords) / sizeof(float));
    if (new_cube[2] > 1) {
      num_to_delete++;
    } else {
      draw_cube(new_cube[0], new_cube[1], new_cube[2]);
      cubes[i] = new_cube;
    }
  }
  for (int i = 0; i < num_to_delete; i++) {
    cubes.pop_back();
  }
  glutSwapBuffers();
}

void add_cube(vector< vector<float> > cubes_added) {
  float x = ((float)rand()/(float)RAND_MAX) * 60.0 - 30.0;
  float y = ((float)rand()/(float)RAND_MAX) * 60.0 - 30.0;
  float z = -100.0;
  for (int i = 0; i < cubes_added.size(); i++) {
    if (abs(cubes_added[i][0] - x) <= 2 && abs(cubes_added[i][1] - y) <= 2) {
      add_cube(cubes_added);
      return;
    }
  }
  float coords[] = {x, y, z};
  vector<float> cube (coords, coords + sizeof(coords) / sizeof(float));
  cubes_added.push_back(cube);
  cubes.insert(cubes.begin(), cube);
}

void update_game() {
  if (cube_delay_count % new_cube_delay == 0) {
    vector< vector<float> > cubes_added;
    for (int i = 0; i < 20; i++) {
      add_cube(cubes_added);
    }
  }
  cube_delay_count++;
}

void timer(int value) {
  update_game();
  glutPostRedisplay();
  glutTimerFunc(refreshMillis, timer, 0);
}

void reshape(GLsizei width, GLsizei height) {
   if (height == 0) height = 1;
   GLfloat aspect = (GLfloat)width / (GLfloat)height;
   glViewport(0, 0, width, height);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluPerspective(45.0f, aspect, 1.0f, 100.0f);
}

int main(int argc, char** argv) {
  glutInit(&argc, argv);            // Initialize GLUT
  glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE); // Enable double buffered mode
  glutInitWindowSize(640, 480);   // Set the window's initial width & height
  glutCreateWindow("Kinect Runner");   // Create window with the given title
  glutDisplayFunc(display);       // Register callback handler for window re-paint event
  glutReshapeFunc(reshape);       // Register callback handler for window re-size event
  initGL();                       // Our own OpenGL initialization
  glutTimerFunc(0, timer, 0);
  update_game();
  glutMainLoop();                 // Enter the infinite event-processing loop
  return 0;
}
