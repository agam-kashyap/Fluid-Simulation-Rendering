#include "../include/Draw.h"

#include <GL/glut.h>
#include <GL/gl.h>
#include <iostream>
#include <stdlib.h>
#include <time.h>

#define _USE_MATH_DEFINES
#include <math.h>

#include <config.h>
#include <sph.h>
#include "../include/MarchingCubes.h"
#include "../include/Obstacles.h"


static int width = 900;
static int height = 900;

static float angle = 360;

static SPH::Simulation* sph;

static Helper::Point3FVector mesh;

void renderSphere(float x, float y, float z, double radius, double velocity, double density, int subdivisions, GLUquadricObj* quadric)
{
    glPushMatrix();
    glTranslatef(x, y, z);
    float red = 0.f;
    float blue = 0.f;
    float green = 0.f;

    glColor3f(red, green, blue);

    //color depends on density
    green = 0.1f;
    // blue = 1.0f;
    double r = (density+sph->minDensity)/(sph->maxDensity + sph->minDensity);
    blue = r;
    // red = r;
    // color depends on velocity
    // if (velocity > SPH::Config::SpeedThreshold / 2.)
    // {
    //     blue = 1.0f;
    // }
    // else if (velocity > SPH::Config::SpeedThreshold / 4.)
    // {
    //     red = 0.1f;
    //     green = 0.1f;
    //     blue = 0.5f;
    // }
    // else
    // {
    //     blue = 0.1f;
    // }

    glColor3f(red, green, blue);
    gluSphere(quadric, radius, subdivisions, subdivisions);

    glPopMatrix();
}

void renderSphere_convenient(float x, float y, float z, double radius, double velocity, double density, int subdivisions)
{
    // the same quadric can be re-used for drawing many spheres
    GLUquadricObj* quadric = gluNewQuadric();
    gluQuadricNormals(quadric, GLU_SMOOTH);
    renderSphere(x, y, z, radius, velocity, density, subdivisions, quadric);
    gluDeleteQuadric(quadric);
}

void setOrthographicProjection()
{
    // switch to projection mode
    glMatrixMode(GL_PROJECTION);

    // save previous matrix which contains the
    // settings for the perspective projection
    glPushMatrix();

    // reset matrix
    glLoadIdentity();

    // set a 2D orthographic projection
    gluOrtho2D(0, width, height, 0);

    // switch back to modelview mode
    glMatrixMode(GL_MODELVIEW);
}

void resetPerspectiveProjection()
{
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

// void renderVerticalBitmapString(float x, float y, int bitmapHeight, void *font, char *string)
//{
//    char *c;
//    int i;
//
//    for (c = string, i = 0; *c != '\0'; i++, c++)
//    {
//        glRasterPos2f(x + bitmapHeight * i, y);
//        glutBitmapCharacter(font, *c);
//    }
//}

void MyDisplay(void)
{
    // clock_t t;
    // t = clock();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);

    

    sph->Run();

    const float cubeSize = static_cast<float>(SPH::Config::BoxWidth);

    glLoadIdentity();
    gluLookAt(3.0, 20.0, 12.0, 0.0, 0.0, 0.0, 0.0, 0.0, 10.0);
    glTranslatef(SPH::Config::BoxWidth/2,SPH::Config::BoxWidth/2,SPH::Config::BoxWidth/2);
    glRotatef(angle, 0, 0, -1);
    glTranslatef(-SPH::Config::BoxWidth/2, -SPH::Config::BoxWidth/2, -SPH::Config::BoxWidth/2);
    
    // Draw the obstacle
    // glBegin(GL_TRIANGLES);
    // for (const auto& triangle : mesh)
    // {
    //     glColor3f(1.0f / cubeSize, 1.5f * triangle.y / cubeSize, 2.5f * triangle.z / cubeSize);
    //     glVertex3f(triangle.x, triangle.y, triangle.z);
    // }
    // glEnd();

    for (auto& particle : sph->particles)
    {
        
        // std::cout << std::to_string(particle.position.x) << " " << std::to_string(particle.position.y) << " " << std::to_string(particle.position.z) << std::endl;
        
        renderSphere_convenient(static_cast<float>(particle.position.x), static_cast<float>(particle.position.y),
                                static_cast<float>(particle.position.z), particle.radius,
                                particle.velocity.calcNormSqr(), particle.density, 4);
    }

    glColor3f(1.0, 0.0, 0.0);

    glBegin(GL_LINES);
    glColor3f(1.0f, 0.0f, 0.0f);

    glVertex3f(0.f, 0.f, 0.f);
    glVertex3f(0.f, 0.f, cubeSize);

    // glVertex3f(0.f, 0.f, 0.f);
    // glVertex3f(0.f, cubeSize, 0.f);

    // glVertex3f(0.f, 0.f, 0.f);
    // glVertex3f(cubeSize, 0.f, 0.f);

    glVertex3f(cubeSize, cubeSize, cubeSize);
    glVertex3f(0.f, cubeSize, cubeSize);

    glVertex3f(cubeSize, cubeSize, cubeSize);
    glVertex3f(cubeSize, 0.f, cubeSize);

    glVertex3f(cubeSize, cubeSize, cubeSize);
    glVertex3f(cubeSize, cubeSize, 0.f);

    // glVertex3f(cubeSize, 0.f, 0.f);
    // glVertex3f(cubeSize, cubeSize, 0.f);

    glVertex3f(0.f, 0.f, cubeSize);
    glVertex3f(cubeSize, 0.f, cubeSize);

    glVertex3f(0.f, 0.f, cubeSize);
    glVertex3f(0.f, cubeSize, cubeSize);

    glVertex3f(0.f, cubeSize, cubeSize);
    glVertex3f(0.f, cubeSize, 0.f);

    glVertex3f(cubeSize, 0.f, cubeSize);
    glVertex3f(cubeSize, 0.f, 0.f);

    // glVertex3f(0.f, cubeSize, 0.f);
    // glVertex3f(cubeSize, cubeSize, 0.f);
    glEnd();

    glBegin(GL_TRIANGLES);
    glColor3f(0.5f, 0.7f, 0.5f);

    glVertex3f(0.f, 0.f, 0.f);
    glVertex3f(0.f, cubeSize, 0.f);
    glVertex3f(cubeSize, 0.f, 0.f);

    glVertex3f(0.f, cubeSize, 0.f);
    glVertex3f(cubeSize, 0.f, 0.f);
    glVertex3f(cubeSize, cubeSize, 0.f);
    glEnd();

    setOrthographicProjection();
    resetPerspectiveProjection();
    glutSwapBuffers();

    glFlush();
}

// reshape function
void reshape(int w, int h)
{
    const double aspect = static_cast<double>(w) / h;

    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // enable perspective projection with fovy, aspect, zNear and zFar
    gluPerspective(35.0, aspect, 0.1, 1000.0);
}

/// timer function
void timf(int /*value*/)
{
    // redraw windows
    glutPostRedisplay();

    // setup next timer
    glutTimerFunc(30, timf, 0);
}

void processNormalKeys(unsigned char key, int /*x*/, int /*y*/)
{
    switch (key)
    {
        case 27: // ESC
            exit(0);
    }
}

void updateGravity()
{
    //   |1     0           0| |x|   |        x        |   |x'|
    //   |0   cos θ    −sin θ| |y| = |y cos θ − z sin θ| = |y'|
    //   |0   sin θ     cos θ| |z|   |y sin θ + z cos θ|   |z'|
    SPH::Config::GravitationalAcceleration =
        Helper::Point3D(SPH::Config::InitGravitationalAcceleration.x,
                               SPH::Config::InitGravitationalAcceleration.y * cos(angle / 180 * M_PI) -
                                   SPH::Config::InitGravitationalAcceleration.z * sin(angle / 180 * M_PI),
                               SPH::Config::InitGravitationalAcceleration.y * sin(angle / 180 * M_PI) +
                                   SPH::Config::InitGravitationalAcceleration.z * cos(angle / 180 * M_PI));
}

void processSpecialKeys(int key, int /*xx*/, int /*yy*/)
{
    switch (key)
    {
        case GLUT_KEY_UP:
            angle -= 0.5;
            // updateGravity();
            break;
        case GLUT_KEY_DOWN:
            angle += 0.5;
            // updateGravity();
            break;
        case GLUT_KEY_HOME:
            angle = 360.0;
            SPH::Config::GravitationalAcceleration = Helper::Point3D(0.0, 0.0, -9.82);
            break;
    }
}

void Draw::MainDraw(int argc, char** argv)
{
    static const std::function<float(float, float, float)> obstacle = Helper::Shapes::Pawn;
    // sph = new SPH::Simulation(&obstacle);
    sph = new SPH::Simulation(nullptr);

    mesh = Helper::MarchingCubes::generateMesh(obstacle);

    // GLUT initialization
    glutInit(&argc, argv);

    // set up window size
    glutInitWindowSize(width, height);

    // set up window position
    glutInitWindowPosition(0, 0);

    // create GLUT window
    glutCreateWindow("SPH model");

    // set up display mode
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

    glEnable(GL_DEPTH_TEST);

    glutKeyboardFunc(processNormalKeys);

    glutSpecialFunc(processSpecialKeys);

    // run main display function
    glutDisplayFunc(MyDisplay);

    // run reshape function
    glutReshapeFunc(reshape);

    // set up timer for 40ms, about 25 fps
    glutTimerFunc(0, timf, 0);

    // set up color
    glClearColor(0.5, 0.5, 0.75, 0.6);

    // enter the GLUT event processing loop
    glutMainLoop();
}

