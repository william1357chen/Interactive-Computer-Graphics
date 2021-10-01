#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>

#ifdef __APPLE__  // include Mac OS X verions of headers
#include <GLUT/glut.h>
#else // non-Mac OS X operating systems
#include <GL/glut.h>
#endif

#define XOFF          50
#define YOFF          50
#define WINDOW_WIDTH  600
#define WINDOW_HEIGHT 600


/*
Answers for part B of homework
(b) The origin of the x and y axes is at the bottom left of the screen.
    x value increases going to the right, decreases going to the left.
    y value increases going to the top, decreases going to the bottom.
    You can display (the program compiles and runs) pixels at (x, y) 
    where either x < 0 or y < 0 but the viewers wouldn't be able 
    to see it because they are outside of the screen. Similarly, drawing
    pixels at (x, y) where either x < WINDOW_WIDTH or y < WIDTH_HEIGHT
    has the same effect.
    The center of the square point (no matter PointSize) is the x, y
    values we passed in. 
*/


void draw_circle(int x, int y, int r);
void circlePoint(int p, int q, int x, int y);
void scale_down(int max_abs_coord);

void display(void);
void idle(void);
void myinit(void);


/* Function to handle file input; modification may be needed */
void file_in(void);

enum Option{PARTC, PARTD, PARTE};

// globals
Option option;
int partCArray[3];
std::vector<std::vector<int>> inputVector;
int K = 500;
int counter = 1;

/*-----------------
The main function
------------------*/
int main(int argc, char** argv)
{
    // Create three different options to show parts (c) (d) and (e)
    // option a is part c, option b is part d, and option c is part e
    std::cout << "Choose one of the options by inputting the corresponding character\n"
        << "(a) draw a circle\n(b) draw circles from input_circles.txt\n"
        << "(c)draw circle animation from input_circles.txt\n";
    std::cout << "Your option = ";
    char c;
    while (std::cin >> c){
        if (c == 'a') { 
            option = PARTC;
            // Ask for the x, y, r for in part c
            std::cout << "\n\nPlease input three integers for x, y, and r of a circle\n";
            int num;
            std::cout << "x = ";
            std::cin >> num;
            partCArray[0] = num;
            std::cout << "y = ";
            std::cin >> num;
            partCArray[1] = num;
            std::cout << "r = ";
            std::cin >> num;
            partCArray[2] = num;
            break; 
        }
        else if (c == 'b') { 
            option = PARTD;
            file_in();
            break; 
        }
        else if (c == 'c') { 
            option = PARTE;
            file_in();
            break; 
        }
        else {
            std::cout << "Please enter a valid option\n";
        }
    }


    glutInit(&argc, argv);

    /* Use both double buffering and Z buffer */
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    glutInitWindowPosition(XOFF, YOFF);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("Assignment 1");
    glutDisplayFunc(display);
    glutIdleFunc(idle);

    myinit();
    glutMainLoop();

    return 0;
}

/*----------
file_in(): file input function. All checks if scale_down is needed.
------------*/
void file_in(void)
{
    std::ifstream ifs("input_circles.txt");
    if (!ifs) {
        std::cerr << "Could not open the file.\n";
        exit(1);
    }
    // variables for input
    int x;
    int y;
    int r;
    int num_circles;
    // Keeping track of the maximum coordinates
    int temp_max;
    int max_x;
    int max_y;
    int max_abs_coord;
    ifs >> num_circles;
    // Read the circle data whilst checking for max_abs_coord
    for (size_t i = 0; i < num_circles; i++) {
        std::vector<int> temp_vector;
        ifs >> x;
        ifs >> y;
        ifs >> r;
        // checking for the maximum x that exceeds screen width
        temp_max = std::max(x + r, abs(x - r));
        max_x = std::max(max_x, temp_max);
        // checking for the maximum y that exceeds screen width
        temp_max = std::max(y + r, abs(y - r));
        max_y = std::max(max_y, temp_max);
        // checking for the maximum x or y that exceeds screen width
        max_abs_coord = std::max(max_x, max_y);
        // push circle data in inner vector
        temp_vector.push_back(x);
        temp_vector.push_back(y);
        temp_vector.push_back(r);
        // append vector to our inputVector
        inputVector.push_back(temp_vector);
    }
    int w = WINDOW_WIDTH / 2;
    // call scale_down if it is needed
    if (max_abs_coord > w) {
        scale_down(max_abs_coord);
    }

    // For converting from world coordinates to screen coordinates
    for (size_t i = 0; i < inputVector.size(); i++) {
        inputVector[i][0] += w;
        inputVector[i][1] += w;
    }
}
// Scale down all the circles to make it the proportional size and shape
void scale_down(int max_abs_coord) {
    int w = WINDOW_WIDTH / 2;
    for (size_t i = 0; i < inputVector.size(); i++) {
        inputVector[i][0] = (inputVector[i][0] * w) / max_abs_coord;
        inputVector[i][1] = (inputVector[i][1] * w) / max_abs_coord;
        inputVector[i][2] = (inputVector[i][2] * w) / max_abs_coord;
    }
}

/*---------------------------------------------------------------------
display(): This function is called once for _every_ frame.
---------------------------------------------------------------------*/
void display(void)
{

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glColor3f(1.0f, 0.84f, 0.0f);         /* draw in golden yellow */

    // for different options draw different circles
    switch (option) {
    case PARTC:
        // part c just draws a circle
        draw_circle(partCArray[0], partCArray[1], partCArray[2]);
        break;
    case PARTD:
        // part d just draws all circles in inputVector
        for (size_t i = 0; i < inputVector.size(); i++) {
            std::vector<int> temp_vec = inputVector[i];
            draw_circle(temp_vec[0], temp_vec[1], temp_vec[2]);
        }
        break;
    case PARTE:
        // part e just draws all circles in inputVector as well
        for (size_t i = 0; i < inputVector.size(); i++) {
            std::vector<int> temp_vec = inputVector[i];
            int r = (temp_vec[2] * counter) / K;
            draw_circle(temp_vec[0], temp_vec[1], r);
        }
        break;
    }
    glFlush();                            /* render graphics */
    glutSwapBuffers();                    /* swap buffers */
}

void idle(void) {
    switch (option) {
        // part c and d doesn't require update
    case PARTC:
        break;
    case PARTD:
        break;
    case PARTE:
        // only need to update for part e
        if (counter >= K) {
            counter = 1;
        }
        display();
        counter++;
        break;
    }
}


/*---------------------------------------------------------------------
myinit(): Set up attributes and viewing
---------------------------------------------------------------------*/
void myinit()
{
    glClearColor(0.0f, 0.0f, 0.92f, 0.0f);    /* blue background*/

    /* set up viewing */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, WINDOW_WIDTH, 0.0, WINDOW_HEIGHT);
    glMatrixMode(GL_MODELVIEW);
}

// Function that draws a circle with Bresenham’s scan-conversion algorithm for circles
void draw_circle(int x, int y, int r) {
    // I want x and y to be the current point
    // So I changed the center of circle to point (p, q)
    // initiliaze variables
    int p = x;
    int q = y;
    x = 0;
    y = r;
    int d = 3 - 2 * r;
    // draw first 8 points
    circlePoint(p, q, x, y);
    while (y >= x)
    {
        x++;
        // computer and check for decision variable d
        if (d < 0){ 
            d = d + 4 * x + 6;
        }
        else {
            y--;
            d = d + 4 * (x - y) + 10;
        }
        // draw the 8 corresponding points
        circlePoint(p, q, x, y);
    }
}

// p stands for x center and q stands for y center
// x and y are the values for the current point
// draw 8 symmetric points
void circlePoint(int p, int q, int x, int y) {
    glPointSize(1.0); 
    glBegin(GL_POINTS);
    glVertex2i(p + x, q + y);
    glVertex2i(p - x, q + y);
    glVertex2i(p + x, q - y);
    glVertex2i(p - x, q - y);
    glVertex2i(p + y, q + x);
    glVertex2i(p - y, q + x);
    glVertex2i(p + y, q - x);
    glVertex2i(p - y, q - x);
    glEnd();
}