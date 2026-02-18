#include <GL/glut.h>
#include <math.h>
#include <iostream>

// Screen dimensions
int width = 1200;
int height = 700;

// Function to draw a single pixel
void drawPixel(int x, int y) {
    glBegin(GL_POINTS);
    glVertex2i(x, y);
    glEnd();
}

// -------------------------------------------------------
// 1. DDA Line Drawing Algorithm (Used for filling and gradients)
// -------------------------------------------------------
void DDA(int x1, int y1, int x2, int y2) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    float steps;

    if (abs(dx) > abs(dy))
        steps = abs(dx);
    else
        steps = abs(dy);

    float Xinc = dx / steps;
    float Yinc = dy / steps;

    float x = x1;
    float y = y1;

    for (int i = 0; i <= steps; i++) {
        drawPixel(round(x), round(y));
        x += Xinc;
        y += Yinc;
    }
}

// -------------------------------------------------------
// 2. Bresenham Line Drawing Algorithm (Used for outlines and details)
// -------------------------------------------------------
void Bresenham(int x1, int y1, int x2, int y2) {
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    while (true) {
        drawPixel(x1, y1);
        if (x1 == x2 && y1 == y2) break;
        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x1 += sx; }
        if (e2 < dx) { err += dx; y1 += sy; }
    }
}

// Helper function to draw a filled rectangle using DDA
void drawFilledRect(int x1, int y1, int x2, int y2, float r, float g, float b) {
    glColor3f(r, g, b);
    for(int i = y1; i <= y2; i++) {
        DDA(x1, i, x2, i);
    }
}

// Helper function to draw rectangle outline using Bresenham
void drawRectOutline(int x1, int y1, int x2, int y2, float r, float g, float b) {
    glColor3f(r, g, b);
    Bresenham(x1, y1, x2, y1); // Bottom
    Bresenham(x2, y1, x2, y2); // Right
    Bresenham(x2, y2, x1, y2); // Top
    Bresenham(x1, y2, x1, y1); // Left
}

// -------------------------------------------------------
// 3. Midpoint Circle Algorithm
// -------------------------------------------------------
void MidpointCircle(int xc, int yc, int r) {
    int x = 0, y = r;
    int d = 3 - 2 * r;
    while (x <= y) {
        drawPixel(xc + x, yc + y); drawPixel(xc - x, yc + y);
        drawPixel(xc + x, yc - y); drawPixel(xc - x, yc - y);
        drawPixel(xc + y, yc + x); drawPixel(xc - y, yc + x);
        drawPixel(xc + y, yc - x); drawPixel(xc - y, yc - x);
        if (d < 0) d += 4 * x + 6;
        else { d += 4 * (x - y) + 10; y--; }
        x++;
    }
}

// Helper function to draw a filled circle
void drawFilledCircle(int xc, int yc, int r, float red, float green, float blue) {
    glColor3f(red, green, blue);
    for(int i = 0; i <= r; i++) {
        MidpointCircle(xc, yc, i);
    }
}

// Function to draw the sky gradient
void drawSkyGradient() {
    for (int y = 150; y <= height; y++) {
        // Interpolation: Blue at top, whitish towards horizon
        float ratio = (float)(y - 150) / (height - 150);
        float r = 0.529 + ratio * (0.8 - 0.529);
        float g = 0.808 + ratio * (0.9 - 0.808);
        float b = 0.922 + ratio * (1.0 - 0.922);
        glColor3f(r, g, b);
        DDA(0, y, width, y);
    }
}


// -------------------------------------------------------
// Main Display Function
// -------------------------------------------------------
void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    // 1. Sky (Gradient)
    drawSkyGradient();

    // 2. Sun (Rays removed)
    drawFilledCircle(900, 600, 40, 1.0, 0.8, 0.0); // Yellow Sun

    // 3. Clouds (with shadow effect)
    drawFilledCircle(200, 580, 30, 0.9, 0.9, 0.9); // Shadow
    drawFilledCircle(240, 590, 35, 0.9, 0.9, 0.9);
    drawFilledCircle(280, 580, 30, 0.9, 0.9, 0.9);
    drawFilledCircle(200, 600, 30, 1.0, 1.0, 1.0); // Main Cloud
    drawFilledCircle(240, 610, 35, 1.0, 1.0, 1.0);
    drawFilledCircle(280, 600, 30, 1.0, 1.0, 1.0);


    // 4. Ground and Road
    // Grass gradient
    for(int y=120; y<150; y++) {
         float ratio = (float)(y - 120) / 30.0;
         glColor3f(0.1 + ratio*0.1, 0.6 + ratio*0.1, 0.1 + ratio*0.1);
         DDA(0, y, width, y);
    }
    drawFilledRect(0, 0, 1200, 120, 0.3, 0.3, 0.3); // Road
    // Road dividers (White stripes)
    glColor3f(1.0, 1.0, 1.0);
    for(int i=50; i<1200; i+=120) {
        drawFilledRect(i, 55, i+60, 65, 1.0, 1.0, 1.0);
    }


    // 5. Building - 1 (DIU - Left)
    drawFilledRect(50, 120, 220, 500, 0.7, 0.5, 0.5); // Main Body
    drawRectOutline(50, 120, 220, 500, 0.4, 0.3, 0.3); // Outline
    // Windows
    for(int y=140; y<480; y+=60) {
        drawFilledRect(70, y, 120, y+40, 0.8, 0.8, 0.9); // Glass
        drawRectOutline(70, y, 120, y+40, 0.5, 0.5, 0.6); // Frame
        drawFilledRect(140, y, 190, y+40, 0.8, 0.8, 0.9);
        drawRectOutline(140, y, 190, y+40, 0.5, 0.5, 0.6);
    }
    // DIU Text (Using lines)
    glColor3f(0.9, 0.9, 1.0); Bresenham(50, 450, 220, 450); // Strip
    glColor3f(0.0, 0.0, 0.6); // Dark Blue Text
    Bresenham(100, 460, 100, 490); Bresenham(100, 460, 115, 475); Bresenham(115, 475, 100, 490); // D
    Bresenham(130, 460, 130, 490); // I
    Bresenham(145, 460, 145, 490); Bresenham(145, 460, 160, 460); Bresenham(160, 460, 160, 490); // U

    // 6. Building - 2 (Food Court - Middle)
    drawFilledRect(400, 120, 650, 350, 0.8, 0.7, 0.4); // Body
    drawRectOutline(400, 120, 650, 350, 0.5, 0.4, 0.2);
    drawFilledRect(390, 350, 660, 380, 0.4, 0.2, 0.1); // Roof
    // Large Glass Windows
    drawFilledRect(420, 140, 630, 300, 0.6, 0.8, 1.0);
    drawRectOutline(420, 140, 630, 300, 0.3, 0.5, 0.7);
    // Grill/Frames on glass
    glColor3f(0.3, 0.5, 0.7);
    Bresenham(525, 140, 525, 300); // Vertical middle line
    Bresenham(420, 220, 630, 220); // Horizontal middle line

    // 7. Building - 3 (Hospital - Right)
    drawFilledRect(850, 120, 1050, 550, 0.85, 0.9, 0.95); // Body (Whitish)
    drawRectOutline(850, 120, 1050, 550, 0.6, 0.7, 0.7);
    // Windows
    for(int y=150; y<480; y+=50) {
        drawFilledRect(870, y, 940, y+30, 0.6, 0.7, 0.8);
        drawFilledRect(960, y, 1030, y+30, 0.6, 0.7, 0.8);
    }
    // --- Hospital Plus Sign (+) ---
    glColor3f(0.9, 0.0, 0.0); // Bright Red
    // Drawn with multiple Bresenham lines for thickness
    // Horizontal Bar (-)
    Bresenham(930, 515, 970, 515);
    Bresenham(930, 516, 970, 516);
    Bresenham(930, 517, 970, 517);
    // Vertical Bar (|)
    Bresenham(950, 495, 950, 535);
    Bresenham(951, 495, 951, 535);
    Bresenham(949, 495, 949, 535);


    // 8. Trees (Bushy)
    int treeX = 1100, treeY = 200;
    drawFilledRect(treeX, 120, treeX+20, treeY, 0.4, 0.2, 0.1); // Trunk
    // Leaves (Overlapping circles)
    drawFilledCircle(treeX+10, treeY+20, 40, 0.0, 0.5, 0.0);
    drawFilledCircle(treeX-15, treeY+10, 35, 0.0, 0.55, 0.0);
    drawFilledCircle(treeX+35, treeY+10, 35, 0.0, 0.45, 0.0);
    drawFilledCircle(treeX+10, treeY+50, 30, 0.1, 0.6, 0.1);


    // 9. Bus
    int busX = 650, busY = 40;
    drawFilledRect(busX, busY, busX+180, busY+80, 0.9, 0.1, 0.1); // Body
    drawRectOutline(busX, busY, busX+180, busY+80, 0.6, 0.0, 0.0);
    // Windows
    drawFilledRect(busX+10, busY+40, busX+50, busY+70, 0.7, 0.8, 0.9);
    drawFilledRect(busX+60, busY+40, busX+110, busY+70, 0.7, 0.8, 0.9);
    drawFilledRect(busX+120, busY+40, busX+170, busY+70, 0.7, 0.8, 0.9);
    // Wheels
    drawFilledCircle(busX+30, busY, 18, 0.1, 0.1, 0.1);
    drawFilledCircle(busX+150, busY, 18, 0.1, 0.1, 0.1);
    drawFilledCircle(busX+30, busY, 8, 0.6, 0.6, 0.6); // Rims
    drawFilledCircle(busX+150, busY, 8, 0.6, 0.6, 0.6);

    glFlush();
}

void init() {
    glClearColor(1.0, 1.0, 1.0, 0.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, width, 0.0, height);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(width, height);
    glutInitWindowPosition(50, 50);
    glutCreateWindow("Hitech City Final - Joyassroy Barua");
    init();
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}
