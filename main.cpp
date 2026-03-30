#include <GL/glut.h>
#include <math.h>
#include <string.h>

// অ্যানিমেশন ও দিন-রাতের ভেরিয়েবল
float busMove = -300.0f;
float wheelAngle = 0.0f;
float cloudMove1 = 0.0f;
float cloudMove2 = 0.0f;
float timeCycle = 0.0f;
float darkness = 0.0f;

// কালার ইন্টারপোলেশন
void setColorInterpolated(float r1, float g1, float b1, float r2, float g2, float b2, float t) {
    glColor3f(r1 + (r2 - r1) * t, g1 + (g2 - g1) * t, b1 + (b2 - b1) * t);
}
void setColorInterpolatedAlpha(float r1, float g1, float b1, float a1, float r2, float g2, float b2, float a2, float t) {
    glColor4f(r1 + (r2 - r1) * t, g1 + (g2 - g1) * t, b1 + (b2 - b1) * t, a1 + (a2 - a1) * t);
}

// বেসিক শেপ
void drawCircle(float cx, float cy, float r, int num_segments) {
    glBegin(GL_POLYGON);
    for (int i = 0; i < num_segments; i++) {
        float theta = 2.0f * 3.1415926f * float(i) / float(num_segments);
        glVertex2f(r * cosf(theta) + cx, r * sinf(theta) + cy);
    }
    glEnd();
}

void drawRect(float x, float y, float w, float h) {
    glBegin(GL_QUADS);
    glVertex2f(x, y); glVertex2f(x + w, y);
    glVertex2f(x + w, y + h); glVertex2f(x, y + h);
    glEnd();
}

void drawGradientRect(float x, float y, float w, float h, float r1, float g1, float b1, float r2, float g2, float b2) {
    glBegin(GL_QUADS);
    glColor3f(r1, g1, b1); glVertex2f(x, y + h); glVertex2f(x + w, y + h);
    glColor3f(r2, g2, b2); glVertex2f(x + w, y); glVertex2f(x, y);
    glEnd();
}

void drawText(float x, float y, const char* text, void* font) {
    glRasterPos2f(x, y);
    for (int i = 0; i < strlen(text); i++) glutBitmapCharacter(font, text[i]);
}

// আকাশ, তারা, সূর্য, চাঁদ ও মেঘ
void drawSky() {
    glBegin(GL_QUADS);
    setColorInterpolated(0.15f, 0.5f, 0.85f, 0.05f, 0.05f, 0.12f, darkness);
    glVertex2f(0.0f, 900.0f); glVertex2f(1600.0f, 900.0f);
    setColorInterpolated(0.85f, 0.95f, 0.8f, 0.1f, 0.05f, 0.2f, darkness);
    glVertex2f(1600.0f, 150.0f); glVertex2f(0.0f, 150.0f);
    glEnd();
}

void drawStars() {
    if (darkness > 0.5f) {
        glColor4f(1.0f, 1.0f, 1.0f, (darkness - 0.5f) * 2.0f);
        glPointSize(2.0f); glBegin(GL_POINTS);
        glVertex2f(200, 800); glVertex2f(500, 750); glVertex2f(800, 850); glVertex2f(1100, 700);
        glVertex2f(1400, 820); glVertex2f(350, 650); glVertex2f(950, 680); glVertex2f(1250, 780);
        glVertex2f(150, 720); glVertex2f(1500, 650); glVertex2f(600, 820); glVertex2f(1350, 740);
        glEnd();
    }
}

void drawSunMoonClouds() {
    float sunY = 780.0f - (darkness * 400.0f);
    float moonY = 380.0f + (darkness * 400.0f);

    // সূর্যের গ্লো এবং বডি
    if (darkness < 0.9f) {
        glColor4f(1.0f, 0.85f, 0.3f, (1.0f - darkness) * 0.4f); drawCircle(950.0f, sunY, 80.0f, 100);
        glColor4f(0.98f, 0.8f, 0.2f, 1.0f - darkness); drawCircle(950.0f, sunY, 55.0f, 100);
    }
    // চাঁদের গ্লো এবং বডি
    if (darkness > 0.1f) {
        glColor4f(0.9f, 0.9f, 1.0f, darkness * 0.3f); drawCircle(300.0f, moonY, 65.0f, 100);
        glColor4f(0.9f, 0.9f, 0.95f, darkness); drawCircle(300.0f, moonY, 45.0f, 100);
        glColor4f(0.8f, 0.8f, 0.85f, darkness); drawCircle(315.0f, moonY + 10.0f, 10.0f, 30); drawCircle(290.0f, moonY - 15.0f, 8.0f, 30);
    }

    // মেঘ
    setColorInterpolated(1.0f, 1.0f, 1.0f, 0.25f, 0.25f, 0.3f, darkness);
    glPushMatrix(); glTranslatef(cloudMove1, 0.0f, 0.0f);
    drawCircle(1100.0f, 800.0f, 45.0f, 50); drawCircle(1170.0f, 830.0f, 60.0f, 50); drawCircle(1240.0f, 800.0f, 45.0f, 50); drawRect(1100.0f, 755.0f, 140.0f, 45.0f);
    glPopMatrix();

    glPushMatrix(); glTranslatef(cloudMove2, 0.0f, 0.0f);
    drawCircle(600.0f, 720.0f, 40.0f, 50); drawCircle(660.0f, 750.0f, 55.0f, 50); drawCircle(720.0f, 720.0f, 40.0f, 50); drawRect(600.0f, 680.0f, 120.0f, 40.0f);
    glPopMatrix();
}

// ক্লিন এবং মডার্ন মেইন বিল্ডিং
void drawPremiumBuildings() {
    // =====================================
    // 1. DIU MAIN TOWER
    // =====================================
    setColorInterpolated(0.85f, 0.88f, 0.9f, 0.2f, 0.25f, 0.3f, darkness); drawRect(120.0f, 150.0f, 220.0f, 450.0f);
    setColorInterpolated(0.6f, 0.65f, 0.7f, 0.1f, 0.15f, 0.2f, darkness); drawRect(340.0f, 150.0f, 40.0f, 450.0f); // Depth

    // ক্লিন গ্লাস প্যানেল
    if(darkness > 0.6f) drawGradientRect(140.0f, 170.0f, 180.0f, 410.0f, 1.0f, 0.9f, 0.6f, 0.8f, 0.6f, 0.2f);
    else drawGradientRect(140.0f, 170.0f, 180.0f, 410.0f, 0.4f, 0.7f, 0.9f, 0.2f, 0.4f, 0.6f);

    // ভার্টিকাল ডিভাইডার
    setColorInterpolated(0.85f, 0.88f, 0.9f, 0.2f, 0.25f, 0.3f, darkness);
    drawRect(220.0f, 170.0f, 20.0f, 410.0f);

    // DIU Sign
    drawRect(160.0f, 620.0f, 140.0f, 45.0f); // Sign base
    if (darkness > 0.5f) glColor3f(0.0f, 1.0f, 1.0f); else glColor3f(0.1f, 0.3f, 0.7f);
    drawText(200.0f, 635.0f, "D I U", GLUT_BITMAP_HELVETICA_18);

    // =====================================
    // 2. FOOD COURT PAVILION
    // =====================================
    setColorInterpolated(0.95f, 0.9f, 0.85f, 0.25f, 0.25f, 0.25f, darkness); drawRect(450.0f, 150.0f, 280.0f, 220.0f);
    setColorInterpolated(0.8f, 0.4f, 0.2f, 0.4f, 0.15f, 0.15f, darkness);
    glBegin(GL_POLYGON); glVertex2f(430.0f, 370.0f); glVertex2f(750.0f, 370.0f); glVertex2f(720.0f, 430.0f); glVertex2f(460.0f, 430.0f); glEnd(); // ছাদ

    // সলিড ক্লিন গ্লাস
    if(darkness > 0.6f) drawGradientRect(470.0f, 170.0f, 240.0f, 180.0f, 1.0f, 0.85f, 0.5f, 0.7f, 0.5f, 0.2f);
    else drawGradientRect(470.0f, 170.0f, 240.0f, 180.0f, 0.5f, 0.8f, 0.95f, 0.2f, 0.5f, 0.7f);

    // Food Court Glowing Sign
    if(darkness > 0.5f) glColor3f(1.0f, 0.6f, 0.0f); else glColor3f(0.8f, 0.3f, 0.1f);
    drawRect(520.0f, 385.0f, 140.0f, 30.0f);
    glColor3f(1.0f, 1.0f, 1.0f); drawText(535.0f, 395.0f, "FOOD COURT", GLUT_BITMAP_HELVETICA_12);

    // =====================================
    // 3. PREMIUM HITECH HOSPITAL (NEW LOOK)
    // =====================================
    // মেইন বডি (Smooth Light Gray)
    setColorInterpolated(0.9f, 0.92f, 0.95f, 0.2f, 0.25f, 0.3f, darkness);
    drawRect(800.0f, 150.0f, 260.0f, 450.0f);

    // সাইড ডেপথ (Side Shadow)
    setColorInterpolated(0.7f, 0.75f, 0.8f, 0.15f, 0.18f, 0.2f, darkness);
    drawRect(1060.0f, 150.0f, 40.0f, 450.0f);

    // সেন্ট্রাল গ্লাস ওয়াল (বিশাল মডার্ন জানালা)
    if(darkness > 0.6f) drawGradientRect(830.0f, 150.0f, 130.0f, 400.0f, 0.0f, 0.5f, 0.8f, 0.0f, 0.2f, 0.4f); // Glowing Blue
    else drawGradientRect(830.0f, 150.0f, 130.0f, 400.0f, 0.3f, 0.6f, 0.8f, 0.1f, 0.3f, 0.5f);

    // গ্লাসের ওপর ফ্লোর ডিভাইডার
    setColorInterpolated(0.8f, 0.82f, 0.85f, 0.15f, 0.2f, 0.25f, darkness);
    for(int y = 200; y < 550; y += 50) {
        drawRect(830.0f, (float)y, 130.0f, 6.0f);
    }

    // ডান পাশের ছোট জানালাগুলো
    for(int y = 180; y < 550; y += 60) {
        if(darkness > 0.6f && y%120 == 0) drawGradientRect(980.0f, y, 60.0f, 30.0f, 1.0f, 0.9f, 0.6f, 0.8f, 0.5f, 0.2f);
        else drawGradientRect(980.0f, y, 60.0f, 30.0f, 0.3f, 0.5f, 0.7f, 0.1f, 0.2f, 0.3f);
    }

    // মডার্ন টপ ওভারহ্যাং / হেলিপ্যাড এরিয়া
    setColorInterpolated(0.6f, 0.65f, 0.7f, 0.15f, 0.2f, 0.25f, darkness);
    drawRect(780.0f, 600.0f, 340.0f, 20.0f);
    if(darkness > 0.5f) { glColor3f(1.0f, 0.0f, 0.0f); drawCircle(790.0f, 625.0f, 5.0f, 20); drawCircle(1110.0f, 625.0f, 5.0f, 20); } // Blinking safety lights

    // প্রিমিয়াম গ্লাস বক্স (রেড ক্রসের জন্য)
    if(darkness > 0.6f) glColor4f(0.8f, 0.9f, 1.0f, 0.8f); else glColor3f(0.7f, 0.8f, 0.9f);
    drawRect(845.0f, 460.0f, 100.0f, 100.0f);

    // রেড ক্রস (+)
    float glow = darkness > 0.6f ? 1.0f : 0.8f;
    glColor4f(glow, 0.1f, 0.1f, 1.0f);
    drawRect(885.0f, 480.0f, 20.0f, 60.0f); // Vertical
    drawRect(865.0f, 500.0f, 60.0f, 20.0f); // Horizontal

    // Hospital Sign (Modern Layout)
    if (darkness > 0.5f) glColor3f(0.0f, 0.8f, 1.0f); else glColor3f(0.1f, 0.3f, 0.6f);
    drawRect(850.0f, 570.0f, 160.0f, 30.0f); // Sign base
    glColor3f(1.0f, 1.0f, 1.0f); drawText(860.0f, 580.0f, "HITECH HOSPITAL", GLUT_BITMAP_HELVETICA_12);

    // =====================================
    // 4. TECH TOWER (Rightmost Skyscraper)
    // =====================================
    setColorInterpolated(0.25f, 0.3f, 0.35f, 0.1f, 0.1f, 0.15f, darkness); drawRect(1180.0f, 150.0f, 200.0f, 580.0f);
    // সলিড গ্লোয়িং বডি (লাইনের বদলে)
    if(darkness > 0.6f) drawGradientRect(1200.0f, 150.0f, 160.0f, 580.0f, 0.0f, 0.8f, 1.0f, 0.0f, 0.2f, 0.5f);
    else drawGradientRect(1200.0f, 150.0f, 160.0f, 580.0f, 0.3f, 0.5f, 0.7f, 0.1f, 0.2f, 0.4f);

    // অ্যান্টেনা
    setColorInterpolated(0.5f, 0.5f, 0.5f, 0.3f, 0.3f, 0.3f, darkness); drawRect(1270.0f, 730.0f, 15.0f, 120.0f);
    if(darkness > 0.5f) { glColor4f(1.0f, 0.0f, 0.0f, 0.5f); drawCircle(1277.0f, 850.0f, 15.0f, 20); } // Red Glow
    if(darkness > 0.5f) glColor3f(1.0f, 0.0f, 0.0f); else glColor3f(0.4f, 0.0f, 0.0f);
    drawCircle(1277.0f, 850.0f, 6.0f, 20); // Bulb
}

// ল্যাম্পপোস্ট এবং লাইট কোন (Light Cone)
void drawStreetLights() {
    for(int x = 150; x < 1500; x += 350) {
        // খুঁটি এবং মাথা
        setColorInterpolated(0.35f, 0.35f, 0.35f, 0.15f, 0.15f, 0.15f, darkness);
        drawRect(x - 4.0f, 150.0f, 8.0f, 140.0f);
        setColorInterpolated(0.7f, 0.7f, 0.7f, 0.3f, 0.3f, 0.3f, darkness);
        drawRect(x - 20.0f, 290.0f, 40.0f, 10.0f);

        // রাতে আলো জ্বলবে এবং রাস্তার ওপর লাইট কোন (Light cone) পড়বে
        if (darkness > 0.6f) {
            glColor4f(1.0f, 1.0f, 0.8f, 1.0f); drawCircle(x, 285.0f, 8.0f, 20); // বাল্ব
            glColor4f(1.0f, 0.9f, 0.4f, 0.6f); drawCircle(x, 285.0f, 25.0f, 30); // আভা

            // রাস্তার ওপর আলোর রশ্মি
            glColor4f(1.0f, 0.9f, 0.4f, 0.2f);
            glBegin(GL_POLYGON);
            glVertex2f(x - 10.0f, 285.0f); glVertex2f(x + 10.0f, 285.0f);
            glVertex2f(x + 80.0f, 100.0f); glVertex2f(x - 80.0f, 100.0f);
            glEnd();
        }
    }
}

// রাস্তা এবং ফুটপাত
void drawRoad() {
    setColorInterpolated(0.3f, 0.7f, 0.4f, 0.15f, 0.25f, 0.15f, darkness); drawRect(0.0f, 130.0f, 1600.0f, 20.0f);
    setColorInterpolated(0.6f, 0.6f, 0.6f, 0.2f, 0.2f, 0.2f, darkness); drawRect(0.0f, 120.0f, 1600.0f, 10.0f);
    setColorInterpolated(0.2f, 0.2f, 0.2f, 0.05f, 0.05f, 0.05f, darkness); drawRect(0.0f, 0.0f, 1600.0f, 120.0f);
    setColorInterpolated(1.0f, 1.0f, 1.0f, 0.4f, 0.4f, 0.4f, darkness); for(int i = 0; i < 1600; i += 120) drawRect((float)i, 55.0f, 60.0f, 6.0f);
}

// চাকা
void drawWheel(float x, float y) {
    glPushMatrix(); glTranslatef(x, y, 0.0f); glRotatef(wheelAngle, 0.0f, 0.0f, 1.0f);
    glColor3f(0.05f, 0.05f, 0.05f); drawCircle(0.0f, 0.0f, 22.0f, 40); // টায়ার
    glColor3f(0.85f, 0.85f, 0.85f); drawCircle(0.0f, 0.0f, 14.0f, 30); // সিলভার রিম
    glColor3f(0.2f, 0.2f, 0.2f);
    for(int i = 0; i < 360; i += 60) {
        glPushMatrix(); glRotatef(i, 0, 0, 1); drawRect(-3.0f, 0.0f, 6.0f, 14.0f); glPopMatrix();
    }
    glColor3f(0.0f, 0.8f, 1.0f); drawCircle(0.0f, 0.0f, 5.0f, 20); // সায়ান হাব (EV লুক)
    glPopMatrix();
}

// সাইবার/মডার্ন EV বাস (ক্লিন ডিজাইন)
void drawBus() {
    glPushMatrix(); glTranslatef(busMove, 0.0f, 0.0f);

    // মেইন বডি (ডার্ক গ্রে/সিলভার ট্রানজিশন)
    setColorInterpolated(0.3f, 0.35f, 0.4f, 0.15f, 0.15f, 0.2f, darkness); drawRect(100.0f, 30.0f, 260.0f, 90.0f);

    // আল্ট্রা-মডার্ন প্যানোরামিক উইন্ডো (স্লিক ব্ল্যাক/ব্লু গ্লাস)
    if(darkness > 0.6f) drawGradientRect(110.0f, 65.0f, 240.0f, 40.0f, 0.8f, 0.8f, 0.6f, 0.9f, 0.9f, 0.3f);
    else drawGradientRect(110.0f, 65.0f, 240.0f, 40.0f, 0.1f, 0.15f, 0.2f, 0.2f, 0.3f, 0.4f);

    // EV Neon Strip (বাসের নিচে জ্বলজ্বল করা দাগ)
    if(darkness > 0.4f) glColor3f(0.0f, 1.0f, 0.8f); else glColor3f(0.0f, 0.6f, 0.5f);
    drawRect(100.0f, 55.0f, 260.0f, 4.0f);

    // হেডলাইট এবং বিম
    if(darkness > 0.6f) {
        glColor4f(1.0f, 1.0f, 0.8f, 0.6f); glBegin(GL_POLYGON);
        glVertex2f(360.0f, 55.0f); glVertex2f(800.0f, 10.0f); glVertex2f(800.0f, 90.0f); glVertex2f(360.0f, 65.0f); glEnd();
        glColor3f(1.0f, 1.0f, 1.0f);
    } else glColor3f(0.9f, 0.9f, 0.9f);
    drawRect(355.0f, 45.0f, 5.0f, 25.0f); // Headlight LED

    // চাকা
    drawWheel(160.0f, 30.0f); drawWheel(300.0f, 30.0f);
    glPopMatrix();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    drawSky();
    drawStars();
    drawSunMoonClouds();

    drawPremiumBuildings(); // পেছনের বিল্ডিং রিমুভ করা হয়েছে
    drawStreetLights();
    drawRoad();
    drawBus();

    if(darkness > 0.5f) glColor3f(1.0f, 1.0f, 1.0f); else glColor3f(0.2f, 0.2f, 0.2f);
    drawText(650.0f, 860.0f, "ULTRA CLEAN HITECH CITY", GLUT_BITMAP_HELVETICA_18);

    glutSwapBuffers();
}

void timer(int value) {
    busMove += 6.0f; wheelAngle -= 12.0f;
    if(busMove > 1700.0f) busMove = -400.0f;
    cloudMove1 += 0.5f; if(cloudMove1 > 600.0f) cloudMove1 = -1600.0f;
    cloudMove2 += 0.3f; if(cloudMove2 > 1000.0f) cloudMove2 = -800.0f;

    timeCycle += 0.005f;
    darkness = (sin(timeCycle) + 1.0f) / 2.0f;

    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

void init() {
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, 1600.0, 0.0, 900.0);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_MULTISAMPLE);
    glutInitWindowSize(1200, 675);
    glutCreateWindow("Ultra Clean Hitech City");

    init();
    glutDisplayFunc(display);
    glutTimerFunc(0, timer, 0);

    glutMainLoop();
    return 0;
}
