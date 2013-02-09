from OpenGL.GL import *
from OpenGL.GLUT import *
from OpenGL.GLU import *
from random import random
from controller import kinect

cubes = []
cur_vel = [0.0, 0.0, 0.3]
new_cube_delay = 10
cube_delay_count = 10
refreshMillis = 1
tracker = kinect.KinectTracker()

def initGL():
    glClearColor(0.0,0.0,0.0,1.0)
    glClearDepth(1.0)
    glEnable(GL_DEPTH_TEST)
    glDepthFunc(GL_LEQUAL)

def draw_cube(x, y, z):
    glLoadIdentity()
    glTranslatef(x, y, z)
    glBegin(GL_QUADS)
    if y > 0:
        # Bottom face (y = -1.0)
        glColor3f(1.0, 0.5, 0.0)     # Orange
        glVertex3f( 1.0, -1.0,  1.0)
        glVertex3f(-1.0, -1.0,  1.0)
        glVertex3f(-1.0, -1.0, -1.0)
        glVertex3f( 1.0, -1.0, -1.0)
    elif y < 0:
        # Top face  (y = 1.0)
        glColor3f(0.0, 1.0, 0.0)     # Green
        glVertex3f( 1.0, 1.0, -1.0)
        glVertex3f(-1.0, 1.0, -1.0)
        glVertex3f(-1.0, 1.0,  1.0)
        glVertex3f( 1.0, 1.0,  1.0)

    if x > 0:
        # Left face (x = -1.0)
        glColor3f(0.0, 0.0, 1.0)     # Blue
        glVertex3f(-1.0,  1.0,  1.0)
        glVertex3f(-1.0,  1.0, -1.0)
        glVertex3f(-1.0, -1.0, -1.0)
        glVertex3f(-1.0, -1.0,  1.0)
    elif x < 0:
        # Right face (x = 1.0)
        glColor3f(1.0, 0.0, 1.0)     # Magenta
        glVertex3f(1.0,  1.0, -1.0)
        glVertex3f(1.0,  1.0,  1.0)
        glVertex3f(1.0, -1.0,  1.0)
        glVertex3f(1.0, -1.0, -1.0)

    # Front face  (z = 1.0)
    glColor3f(1.0, 0.0, 0.0)     # Red
    glVertex3f( 1.0,  1.0, 1.0)
    glVertex3f(-1.0,  1.0, 1.0)
    glVertex3f(-1.0, -1.0, 1.0)
    glVertex3f( 1.0, -1.0, 1.0)

    glEnd()

def display():
    global cubes, cur_vel
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
    glMatrixMode(GL_MODELVIEW)

    cubes_to_delete = []
    for i,cube in enumerate(cubes):
        cubes[i] = [cube[0] - cur_vel[0], cube[1] - cur_vel[1], cube[2] + cur_vel[2]]
        if cubes[i][2] > 1:
            cubes_to_delete.append(i)
        else:
            draw_cube(cubes[i][0], cubes[i][1], cubes[i][2])

    del_count = 0
    for i in cubes_to_delete:
        del cubes[i-del_count]
        del_count += 1

    glutSwapBuffers()

def add_cube(cubes_added):
    global cubes
    x = random() * 60 - 30
    y = random() * 60 - 30
    z = -50
    for cube in cubes_added:
        if abs(cube[0] - x) <= 2 and abs(cube[1] - y) <= 2:
            add_cube(cubes_added)
            return
    cubes_added.append([x, y, z])
    cubes.append([x, y, z])

def update_game():
    global cur_vel, cube_delay_count, new_cube_delay
    if cube_delay_count % new_cube_delay == 0:
        cubes_added = []
        for i in range(5):
            add_cube(cubes_added)
    if cube_delay_count % (new_cube_delay*4)== 0:
      tracker.update()
    cube_delay_count += 1

def timer(value):
    update_game()
    glutPostRedisplay()
    glutTimerFunc(refreshMillis, timer, 0)

def reshape(width, height):
    if height == 0:
        height = 1
    glViewport(0, 0, width, height)
    glMatrixMode(GL_PROJECTION)
    glLoadIdentity()
    gluPerspective(45.0, float(width)/float(height), 1.0, 50.0)

def handle_keypress(key, x, y):
    global cur_vel
    max_speed = 0.1
    if key == 'd':
        cur_vel[0] = max_speed
    elif key == 'a':
        cur_vel[0] = -max_speed
    if key == 'w':
        cur_vel[1] = max_speed
    elif key == 's':
        cur_vel[1] = -max_speed

if __name__ == '__main__':
    glutInit()
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE)
    glutInitWindowSize(640,480)
    glutCreateWindow("Shape")
    glutDisplayFunc(display)
    glutReshapeFunc(reshape)
    glutKeyboardFunc(handle_keypress)
    initGL()
    glutTimerFunc(0, timer, 0)
    glutMainLoop()

