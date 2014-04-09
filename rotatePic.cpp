/*************************************************************************
	> File Name: rotatePic.cpp
	> Author: sijiewang
	> Mail: lnmcc@hotmail.com 
	> Blog: lnmcc.net 
	> Created Time: 2014年03月27日 星期四 12时05分45秒
 ************************************************************************/
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <png.h>
#include <setjmp.h>

using namespace std;

int pngWidth;
int pngHeight;
bool pngAlpha;
GLubyte *pngData = NULL;
GLuint texture;
int scrnWidth= 1600;
int scrnHeight = 900;
int degree = 0;

bool LoadPngImage(char *name, int *outWidth, int *outHeight, bool *outHasAlpha, GLubyte **outData) {
    png_structp png_ptr;
    png_infop info_ptr;
    unsigned int sig_read = false;
    int color_type, interface_type;
    FILE *fp;

    cout << "Open PNG file: " << name << endl;

    if((fp = fopen(name, "rb")) == NULL) {
        perror("Open PNG file: \n");
        return false;
    }

    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if(png_ptr == NULL) {
        cerr << "Create PNG struct error" << endl;
        fclose(fp);
        return false;
    } 

    info_ptr = png_create_info_struct(png_ptr);
    if(info_ptr == NULL) {
        cerr << "Create PNG info error" << endl;
        fclose(fp);
        return false;
    }

    if(setjmp(png_jmpbuf(png_ptr))) {
        cerr << "Setjmp error" << endl;
        png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
        pclose(fp);
        return false;
    }
    
    png_init_io(png_ptr, fp);
    png_set_sig_bytes(png_ptr, sig_read);
    png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING | PNG_TRANSFORM_EXPAND, png_voidp_NULL);
    *outWidth = info_ptr->width;
    *outHeight = info_ptr->height;

    switch(info_ptr->color_type) {
        case PNG_COLOR_TYPE_RGBA:
            *outHasAlpha= true;
            break;
        case PNG_COLOR_TYPE_RGB:
            *outHasAlpha = false;
            break;
        default:
            cout << "Color type: " << info_ptr->color_type << " not supported!" << endl;
            png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
            fclose(fp);
        return false;
    }

    unsigned int row_bytes = png_get_rowbytes(png_ptr, info_ptr);
    *outData = (unsigned char*)malloc(row_bytes * (*outHeight));
    png_bytepp row_pointer = png_get_rows(png_ptr, info_ptr);
    
    for(int i = 0; i < *outHeight; i++) {
        memcpy(*outData + (row_bytes * (*outHeight - i - 1)), row_pointer[i], row_bytes);
    }
    png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
    fclose(fp);
    cout << "Read PNG finished!" << endl;
    return true;
}

void init(void) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-scrnWidth / 2, scrnWidth / 2, -scrnHeight / 2, scrnHeight / 2);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glViewport(0, 0, scrnWidth, scrnHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    /*
    glShadeModel(GL_SMOOTH);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glFrontFace(GL_CCW);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
*/
    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, pngWidth, pngHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, pngData);
}

void disp(void) {
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glTranslated(250, 200, 0);
    glRotated(degree, 0, 0, 1);

    glBegin(GL_QUADS);
        glTexCoord2d(0, 0); glVertex2d(-250, -100);
        glTexCoord2d(1, 0); glVertex2d(250, -100);
        glTexCoord2d(1, 1); glVertex2d(250, 100);
        glTexCoord2d(0, 1); glVertex2d(-250, 100);
    glEnd();

    glutSwapBuffers(); 
    glPopMatrix();
}

void mouseFunc(int button, int state, int x, int y) {
    switch(button) {
    case GLUT_LEFT_BUTTON:
        if(state == GLUT_DOWN) {
            if(degree < 350) degree += 10;
            else degree = 0;
        }
        glutPostRedisplay();
        break;
    case GLUT_RIGHT_BUTTON:
        if(state == GLUT_DOWN) {
            if(degree > 10) degree -= 10;
            else degree = 360; 
        }
        glutPostRedisplay();
        break;
    default:
        break;
    }
}

int main(int argc, char* argv[]) {

    if(argc != 2) {
        cout << "Please select a PNG picture with alpha channel" << endl; 
        cout << "Usage: " << endl;
        cout << "    " << argv[0] << " file.png" << endl;
        exit(1);
    }

    LoadPngImage(argv[1], &pngWidth, &pngHeight, &pngAlpha, &pngData); 
    cout << "PNG Info: " << endl;
    cout << "    name: " << argv[1] << endl;
    cout << "    width: " << pngWidth << endl;
    cout << "    height: " << pngHeight << endl;
    cout << "    alpha: " << (pngAlpha ? "yes" : "no") << endl;

    if(!pngAlpha) {
        cout << "Sorry, Just support PNG with alpha channel" << endl;
        exit(1);
    }

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(scrnWidth, scrnHeight);
    glutCreateWindow("Rotate Picture");

    init();

    glutDisplayFunc(disp);
    glutMouseFunc(mouseFunc);
    glutMainLoop();

    free(pngData);
    return 0;
}

