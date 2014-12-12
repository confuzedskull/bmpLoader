#include <OpenGL/OpenGL.h>
#include <OpenGL/glu.h>
#include <GLUT/GLUT.h>
#include <stdlib.h>
#include <stdio.h>

GLuint load_bmp(const char*filename, int width, int height)
{
    GLuint texture;
    unsigned char* data;
    FILE* file;
    file = fopen(filename, "rb");
    if(file == NULL)
        return 0;
    data = (unsigned char*) malloc(width*height*3);
    fread(data, width*height*3, 1, file);
    fclose(file);
    for(int i=0; i<width*height; ++i)
    {
        int index = i*3;
        unsigned char B,R;
        B = data[index];
        R = data[index+2];
        data[index] = R;
        data[index+2] = B;
    }
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);
    free(data);
    return texture;
}
static GLuint texture;

void render()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glBindTexture(GL_TEXTURE_2D, texture);
    glEnable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);
    glTexCoord2i(0, 0); glVertex2f(0.0, 0.0);
    glTexCoord2i(0, 1); glVertex2f(0.0, 64.0);
    glTexCoord2i(1, 1); glVertex2f(64.0, 64.0);
    glTexCoord2i(1, 0); glVertex2f(64.0, 0.0);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glFlush();
}

void init()
{
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, 640, 0, 480);
    texture = load_bmp("confuzedskull.bmp", 256, 256);
}

void resize(int w, int h)
{
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
}

void key(unsigned char key, int x, int y)
{
    if(key == 27)
        exit(0);
}

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(640, 480);
    glutCreateWindow("bmpLoader");
    init();
    glutDisplayFunc(render);
    glutReshapeFunc(resize);
    glutKeyboardFunc(key);
    glutMainLoop();
    return 0;
}