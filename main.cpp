#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif
#include <iostream>
#include <fstream>
#include <string>
#include <cstdint>

//global variables
GLuint texture;
int32_t image_width, image_height;
std::ifstream image_file;

//this function checks endianness
bool is_big_endian()
{
    int i=1;
    return !((char*)&i)[0];
}

//this function retrieves the appropriate value according to endianness
int32_t to_int32(char* buffer, int length)
{
    int32_t i=0;
    if(!is_big_endian())
        for(int j=0;j<length;j++)
            ((char*)&i)[j]=buffer[j];
    else
        for(int j=0;j<length;j++)
            ((char*)&i)[sizeof(int)-1-j]=buffer[j];
    return i;
}

void load(std::string filename)
{
    char* data;
    char file_info[4];
    image_file.open(filename.c_str(), std::ios::binary);
    if(image_file.bad())
    {
        std::cerr<<"error loading file.\n";
        return;
    }
    image_file.seekg(18,image_file.cur);//skip beginning of header
    image_file.read(file_info,4);//width
    image_width = to_int32(file_info,4);//convert raw data to integer
    image_file.read(file_info,4);//height
    image_height = to_int32(file_info,4);//convert raw data to integer
    image_file.seekg(28,image_file.cur);//skip rest of header
    int image_area = image_width*image_height;//calculate area now since it'll be used 3 times
    data = new char[image_area*3];//set the buffer size
    image_file.read(data, image_area*3);//get the pixel matrix
    image_file.close();
    //next we need to rearrange the color values from BGR to RGB
    for(int i=0; i<image_area; ++i)//iterate through each cell of the matrix
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
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, image_width, image_height, GL_RGB, GL_UNSIGNED_BYTE, data);
    delete[] data;
}

void render()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glBindTexture(GL_TEXTURE_2D, texture);
    glEnable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);
    glTexCoord2i(0, 0); glVertex2i(0, 0);
    glTexCoord2i(0, 1); glVertex2i(0, image_height);
    glTexCoord2i(1, 1); glVertex2i(image_width, image_height);
    glTexCoord2i(1, 0); glVertex2i(image_width, 0);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glFlush();
}

void initialize()
{
    glClearColor(1.0, 1.0, 1.0, 0.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, 640, 0, 480);
}

void change_size(int w, int h){}

void key_pressed(unsigned char key, int x, int y)
{
    if(key == 27)
        exit(0);
}

int main(int argc, char *argv[])
{
    std::string filename;
    if(argc==2)
        load(argv[1]);
    else
    {
        std::cerr<<"No bitmap file loaded. ";
        while(true)
        {
            std::cerr<<"Please supply filename or type 'quit'.\n";
            std::cin>>filename;
            if(filename=="quit")
                return 1;
            image_file.open(filename.c_str());
            if(image_file.good())
            {
                image_file.close();
                break;
            }
            else
                std::cerr<<filename<<" does not exist. \n";
        }
    }
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(640, 480);
    glutCreateWindow("bmpLoader");
    initialize();
    load(filename);
    glutDisplayFunc(render);
    glutReshapeFunc(change_size);
    glutKeyboardFunc(key_pressed);
    glutMainLoop();
    return 0;
}
