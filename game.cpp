#include <iostream>
#include <vector>
#include <GL/glut.h>
#include <stdio.h>
#include <math.h>

using namespace std;

float refreshMillis = 1;
int cube_delay_count = 0;
int new_cube_delay = 2;
vector< vector<float> > cubes;
float init_vel[] = {0.0, 0.0, 1.0};
vector<float> cur_vel (init_vel, init_vel + sizeof(init_vel) / sizeof(float));
bool* key_states = new bool[256];
float ship_x = 0;
float ship_y = -2;
float ship_z = -8;
int score = 0;
float flash = 0;
float shoot_speed = 3;
float shoot_x = ship_x;
float shoot_y = ship_y;
float shoot_z = 0;

/* Initialize OpenGL Graphics */
void initGL() {
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set background color to black and opaque
  glClearDepth(1.0f);                   // Set background depth to farthest
  glEnable(GL_DEPTH_TEST);   // Enable depth testing for z-culling
  glDepthFunc(GL_LEQUAL);    // Set the type of depth-test
}

void draw_ship(float x, float y, float z) {
  glLoadIdentity();
  glTranslatef(x, y, z);
  float roll = -40.0*sin(4*cur_vel[0]);
  float pitch = -50.0*sin(4*cur_vel[1]);
  glRotatef(roll, 0,0,-1);
  glRotatef(pitch, 1,0,0);
  glBegin(GL_TRIANGLES);

  // right face
  glColor3f(0.7f, 0.7f, 1.0f);
  glVertex3f( 0.0f, 0.0f, -1.0f);
  glVertex3f( 1.0f, 0.0f,  1.0f);
  glVertex3f( 0.0f, 0.3f,  0.0f);

  // left face
  glVertex3f( 0.0f, 0.0f, -1.0f);
  glVertex3f(-1.0f, 0.0f,  1.0f);
  glVertex3f( 0.0f, 0.3f,  0.0f);

  // back face
  glColor3f(0.3f, 0.3f, 1.0f);
  glVertex3f( 1.0f, 0.0f,  1.0f);
  glVertex3f(-1.0f, 0.0f,  1.0f);
  glVertex3f( 0.0f, 0.3f,  0.0f);
  glEnd();
}

void draw_shoot(float x, float y, float z) {
  glLoadIdentity();
  glTranslatef(x, y, z);
  float roll = -40.0*sin(4*cur_vel[0]);
  float pitch = -50.0*sin(4*cur_vel[1]);
  glRotatef(roll, 0,0,-1);
  glRotatef(pitch, 1,0,0);
  glBegin(GL_TRIANGLES);

  // right face
  glColor3f(1, 0, 0);
  glVertex3f( 0.0f, 0.0f, -1.0f);
  glVertex3f(-0.1f, 0.0f, 0.0f);
  glVertex3f( 0.1f, 0.0f, 0.0f);

  glVertex3f( 0.0f, 0.0f, 1.0f);
  glVertex3f(-0.1f, 0.0f, 0.0f);
  glVertex3f( 0.1f, 0.0f, 0.0f);
  glEnd();
}



bool draw_cube(float x, float y, float z, float r, float g, float b) {
  //ghetto collision detection
  if ((abs(z - shoot_z) < 0.5) && (abs(y - shoot_y) < 0.5) && (abs(x - shoot_x) < 0.5)) {
    score += 100;
    return true;
  }

  //ghetto collision detection
  if ((abs(z - ship_z) < 0.5) && (abs(y - ship_y) < 0.5) && (abs(x - ship_x) < 0.5))
    score = 0;

  float red = cos(z*0.01 + (x*x + y*y)*0.0001*4);
  float ged = cos(z*0.01 + (x*x + y*y)*0.0002*4);
  float bed = cos(z*0.01 + (x*x + y*y)*0.0003*4);

  r = red*(1-fabs(z/400.0f));
  g = ged*(1-fabs(z/400.0f));
  b = bed*(1-fabs(z/400.0f));

  glLoadIdentity();
  glTranslatef(x, y, z);
  glBegin(GL_QUADS);

  if (y < 0) {
    // Top face (y = 1.0f)
    // Define vertices in counter-clockwise (CCW) order with normal pointing out
    glColor3f(0.5*r, 0.5*g, 0.5*b);     // Green
    glVertex3f( 1.0f, 1.0f, -1.0f);
    glVertex3f(-1.0f, 1.0f, -1.0f);
    glVertex3f(-1.0f, 1.0f,  1.0f);
    glVertex3f( 1.0f, 1.0f,  1.0f);
  } else if (y > 0) {
    // Bottom face (y = -1.0f)
    glColor3f(0.5*r, 0.5*g, 0.5*b);     // Orange
    glVertex3f( 1.0f, -1.0f,  1.0f);
    glVertex3f(-1.0f, -1.0f,  1.0f);
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glVertex3f( 1.0f, -1.0f, -1.0f);
  }

  if (x > 0) {
    // Left face (x = -1.0f)
    glColor3f(0.5*r, 0.5*g, 0.5*b);     // Blue
    glVertex3f(-1.0f,  1.0f,  1.0f);
    glVertex3f(-1.0f,  1.0f, -1.0f);
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glVertex3f(-1.0f, -1.0f,  1.0f);
  } else if (x < 0) {
    // Right face (x = 1.0f)
    glColor3f(0.5*r, 0.5*g, 0.5*b);     // Magenta
    glVertex3f(1.0f,  1.0f, -1.0f);
    glVertex3f(1.0f,  1.0f,  1.0f);
    glVertex3f(1.0f, -1.0f,  1.0f);
    glVertex3f(1.0f, -1.0f, -1.0f);
  }

  // Front face  (z = 1.0f)
  glColor3f(r, g, b);                   // Red
  glVertex3f( 1.0f,  1.0f, 1.0f);
  glVertex3f(-1.0f,  1.0f, 1.0f);
  glVertex3f(-1.0f, -1.0f, 1.0f);
  glVertex3f( 1.0f, -1.0f, 1.0f);

  glEnd();

  return false;
}

void display() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glMatrixMode(GL_MODELVIEW);
  int num_to_delete = 0;
  for (int i = 0; i < cubes.size(); i++) {
    float coords[] = {cubes[i][0] + cur_vel[0], cubes[i][1] + cur_vel[1],
      cubes[i][2] + cur_vel[2], cubes[i][3], cubes[i][4], cubes[i][5]};
    vector<float> new_cube (coords, coords + sizeof(coords) / sizeof(float));
    if (new_cube[2] > 1) {
      num_to_delete++;
    } else {
      cubes[i] = new_cube;
      if (draw_cube(new_cube[0], new_cube[1], new_cube[2], new_cube[3], new_cube[4], new_cube[5]))
        cubes[i][2] = 10;
    }
  }

  if (shoot_z <= ship_z && shoot_z < 150) {
    draw_shoot(shoot_x, shoot_y, shoot_z);
    shoot_z = shoot_z - shoot_speed;
  }

  //your location
  draw_ship(ship_x, ship_y, ship_z);

  for (int i = 0; i < num_to_delete; i++) {
    cubes.pop_back();
  }
  glutSwapBuffers();
}

void add_cube(vector< vector<float> > cubes_added) {
  float rnd = ((float)rand()/(float)RAND_MAX);
  float x;
  float y;
  if (rnd < 0.5) {
    x = ((float)rand()/(float)RAND_MAX) * 200.0 - 100.0;
    y = ((float)rand()/(float)RAND_MAX) * 200.0 - 100.0;
  } else if (rnd < 0.75) {
    x = ((float)rand()/(float)RAND_MAX) * 300.0 - 150.0;
    y = ((float)rand()/(float)RAND_MAX) * 300.0 - 150.0;
  } else {
    x = ((float)rand()/(float)RAND_MAX) * 400.0 - 200.0;
    y = ((float)rand()/(float)RAND_MAX) * 400.0 - 200.0;
  }
 float z = -200.0;
  float r = ((float)rand()/(float)RAND_MAX);
  float g = ((float)rand()/(float)RAND_MAX);
  float b = ((float)rand()/(float)RAND_MAX);

  for (int i = 0; i < cubes_added.size(); i++) {
    if (abs(cubes_added[i][0] - x) <= 2 && abs(cubes_added[i][1] - y) <= 2) {
      add_cube(cubes_added);
      return;
    }
  }
  float coords[] = {x, y, z, r, g, b};
  vector<float> cube (coords, coords + sizeof(coords) / sizeof(float));
  cubes_added.push_back(cube);
  cubes.insert(cubes.begin(), cube);
}

void update_state() {
  if (key_states['a'])
    cur_vel[0] = min(cur_vel[0] + 0.15, 0.5);
  else if (key_states['d'])
    cur_vel[0] = max(cur_vel[0] - 0.15, -0.5);
  else
    cur_vel[0] = 0.8*cur_vel[0];

  if (key_states['s'])
    cur_vel[1] = min(cur_vel[1] + 0.15, 0.5);
  else if (key_states['w'])
    cur_vel[1] = max(cur_vel[1] - 0.15, -0.5);
  else
    cur_vel[1] = 0.8*cur_vel[1];

  if (!(key_states['w'] || key_states['a'] || key_states['s'] || key_states['d']))
    flash = 0.8*flash;
  else
    flash = 1;

  if (key_states[' '])
    shoot_z = ship_z;
}

void update_game() {
  if (cube_delay_count % new_cube_delay == 0) {
    vector< vector<float> > cubes_added;
    for (int i = 0; i < 20; i++) {
      add_cube(cubes_added);
    }
  }
  cube_delay_count+=1;
  if (cube_delay_count % 10 == 0)
    score+=1;

  update_state();
  printf("%d\n", score);
}

void key_pressed(unsigned char key, int x, int y) {
  if (key == 'q')
    exit(0);
  else
    key_states[key] = true;
}

void key_up(unsigned char key, int x, int y) {
  key_states[key] = false;
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
   gluPerspective(120.0f, aspect, 2.0f, 200.0f);
}

int main(int argc, char** argv) {
  glutInit(&argc, argv);            // Initialize GLUT
  glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE); // Enable double buffered mode
  glutInitWindowSize(640, 480);   // Set the window's initial width & height
  glutCreateWindow("Kinect Runner");   // Create window with the given title
  glutDisplayFunc(display);       // Register callback handler for window re-paint event
  glutReshapeFunc(reshape);       // Register callback handler for window re-size event
  glutKeyboardFunc(key_pressed);  // Register callback handler for key press
  glutKeyboardUpFunc(key_up);     // Register callback handler for key up
  initGL();                       // Our own OpenGL initialization
  glutTimerFunc(0, timer, 0);
  update_game();
  glutMainLoop();                 // Enter the infinite event-processing loop
  return 0;
}
