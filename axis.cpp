#include <iostream>
#include <GL/glut.h>

using namespace std;

float w, h, tip = 0, turn = 0, turn_speed = 5;

// Ensures project matrix does not distort from window size changes.
void reshape(int nw, int nh) {
	w = nw;
	h = nh;

	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(40.0, GLfloat(w) / GLfloat(h), 1.0, 10.0);
}

// Disable GLUT lighting elements. Used to disable rendering processes for debug objects.
void DisableLighting() {
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
	glDisable(GL_DEPTH_TEST);
}


// Enable GLUT lighting elements.
void EnableLighting() {
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
}

void Turn(int key, int x, int y) {
	int mod = glutGetModifiers();

	switch (mod) {
		case GLUT_ACTIVE_SHIFT: turn_speed = 1;
			break;
		default: turn_speed = 5;
			break;
	}

	switch (key) {
		case GLUT_KEY_RIGHT: turn += turn_speed;
			break;
		case GLUT_KEY_LEFT : turn -= turn_speed;
			break;
		case GLUT_KEY_UP : tip -= turn_speed;
			break;
		case GLUT_KEY_DOWN : tip += turn_speed;
			break;
	}
}

void DrawAxes() {
	float ORG[3] = {0, 0, 0};

	float XP[3] = {1, 0, 0};
	float YP[3] = {0, 1, 0};
	float ZP[3] = {0, 0, 1};

	DisableLighting(); // Prevent lighting from affecting debug axis.
	glPushMatrix();
//	glViewport(w - (w / 20), h - (h / 40), w / 5, h / 5);
	glViewport(w - (w / 6), h - (h / 5), w / 5, h / 5);

	glTranslatef(0, 0, -5);
	glRotatef(tip, 1, 0, 0);
	glRotatef(turn, 0, 1, 0);
//	glScalef(0.25, 0.25, 0.25);

	glLineWidth(2.0);

	glBegin(GL_LINES);
	glColor3f(1, 0, 0);   // x-axis is red (right).
	glVertex3fv(ORG);
	glVertex3fv(XP);
	glColor3f(0, 1, 0);   // y-axis is green (up).
	glVertex3fv(ORG);
	glVertex3fv(YP);
	glColor3f(0, 0, 1);   // z-axis is blue (towards camera).
	glVertex3fv(ORG);
	glVertex3fv(ZP);

	glEnd();
	glPopMatrix();
	EnableLighting(); // Re-enable lighting for other objects to be rendered.
	glViewport(0, 0, w, h);
}

void DrawTorus() {
	glPushMatrix();

	glTranslatef(0, 0, -5);
	glRotatef(tip, 1, 0, 0);
	glRotatef(turn, 0, 1, 0);

	glutSolidTorus(0.275, 0.85, 16, 40);

	glPopMatrix();
	glFlush();
}

void Display() {
	glViewport(0, 0, w, h);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

//	DrawTorus();
	DrawAxes();

	glutSwapBuffers();
}

// Performs application specific initialization.  It defines lighting
// parameters for light source GL_LIGHT0: black for ambient, yellow for
// diffuse, white for specular, and makes it a directional source
// shining along <-1, -1, -1>.  It also sets a couple material properties
// to make cyan colored objects with a fairly low shininess value.  Lighting
// and depth buffer hidden surface removal are enabled here.
void init() {
	GLfloat black[] = {0.0, 0.0, 0.0, 1.0};
	GLfloat yellow[] = {1.0, 1.0, 0.0, 1.0};
	GLfloat cyan[] = {0.0, 1.0, 1.0, 1.0};
	GLfloat white[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat direction[] = {1.0, 1.0, 1.0, 0.0};

	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, cyan);
	glMaterialfv(GL_FRONT, GL_SPECULAR, white);
	glMaterialf(GL_FRONT, GL_SHININESS, 30);

	glLightfv(GL_LIGHT0, GL_AMBIENT, black);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, yellow);
	glLightfv(GL_LIGHT0, GL_SPECULAR, white);
	glLightfv(GL_LIGHT0, GL_POSITION, direction);

	EnableLighting();
}

int main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitWindowSize(600, 400);
	glutInitWindowPosition(400, 300);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);
	glutCreateWindow("Notreal Engine");
	glutDisplayFunc(Display);
	glutIdleFunc(Display);
	glutReshapeFunc(reshape);
	glutSpecialFunc(Turn);

	float grey = 0.1;
    glClearColor(grey, grey, grey, 1);
	glEnable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	gluPerspective(40.0, 1.5, 1.0, 10.0);
	glMatrixMode(GL_MODELVIEW);
	init();
	glutMainLoop();
}