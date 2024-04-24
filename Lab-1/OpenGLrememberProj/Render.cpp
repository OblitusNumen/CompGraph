#include "Render.h"
#include <math.h>
#include <stdio.h>

#include "Point3D.h"

#include <sstream>
#include <iostream>

#include <Windows.h>
#include <windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>

#include "MyOGL.h"

#include "Camera.h"
#include "Light.h"
#include "Primitives.h"

#include "GUItextRectangle.h"

double t = 0;
double pi = 3.14159265358979;

int col = 0;
const int colors = 100000;
double color[colors];

int alpha = 1;
const int textureCount = 3;
int texture = 0;
GLuint texIds[textureCount];

//x,y,z,density,r,g,b
int lightSourceCount = 3;
double lightSources[][7] = { { 0, 0, 0, 0.2, .8, .2, .6 }, { 5, 10, 4, 0.03, .02, .06, .08 }, { 10, 5, 8, 0.4, .9, .9, .6 } };

int colorPolicy = 0;
int colorPolicyCount = 3;

double p0[]{ 6, 6 };
double p1[]{11, 4};
double p2[]{11, 4};
double p3[]{ 5, 7 };
double points0[8][2] = { {1, 0}, {6, 4}, {11, 1}, {12, 6}, {8, 8}, {5, 15}, {0, 12}, {5, 6} };
const int count = 3;
const int size = 8 + count * 2;

double from = 0;
double to = 16;
double angle0 = 0;
double maxAngle = pi / 2;
double sliceStep = .1;
int additionalSlices = 2;
double speed = 1;

double xShift = -5;
double yShift = -7;
double zShift = 0;

void center(double x0, double x1, double x2, double y0, double y1, double y2, double* resX, double* resY) {
	double cx1 = (x0 + x1) / 2;
	double cy1 = (y0 + y1) / 2;
	double k1 = (x0 - x1) / (y1 - y0);
	double cx2 = (x1 + x2) / 2;
	double cy2 = (y1 + y2) / 2;
	double k2 = (x1 - x2) / (y2 - y1);
	*resX = (-k1 * cx1 + k2 * cx2 + cy1 - cy2) / (k2 - k1);
	*resY = k1 * (*resX - cx1) + cy1;
}

double random() {
	return (double)rand() / RAND_MAX;
}

void setColor(double x, double y, double z) {
	double r = 0;
	double g = 0;
	double b = 0;
	for (size_t i = 0; i < lightSourceCount; i++)
	{
		double dist = (sqrt((lightSources[i][0] - x) * (lightSources[i][0] - x) + (lightSources[i][1] - y) * (lightSources[i][1] - y)
			+ (lightSources[i][2] - z) * (lightSources[i][2] - z)) + 1) * lightSources[i][3];
		r += lightSources[i][4] / dist;
		g += lightSources[i][5] / dist;
		b += lightSources[i][6] / dist;
	}
	glColor4d(r, g, b, .5);
}

void setColor() {
	glColor4d(color[col++], color[col++], color[col++], .5);
}

void point(Point3D p, Vector3D normal) {
	if (colorPolicy == 1) setColor(p.x, p.y, p.z);
	Point3D p1 = p.shift(xShift, yShift, zShift);
	if (colorPolicy == 2) setColor(p1.x, p1.y, p1.z);
	glNormal3d(normal.x, normal.y, normal.z);
	glVertex3d(p1.x, p1.y, p1.z);
}

void drawPlane(double points[][2], int size, double pc0[2], double plane, double pivot[2], double angle, int reverse) {
	Point3D pc = Point3D(pc0[0], pc0[1], plane).rotate(pivot, angle);
	for (size_t i = 0; i < size; i++)
	{
		int j = (i + 1) % size;
		if (!colorPolicy) setColor();
		Point3D p0 = Point3D(points[i][0], points[i][1], plane).rotate(pivot, angle);
		Point3D p1 = Point3D(points[j][0], points[j][1], plane).rotate(pivot, angle);
		glTexCoord2d(reverse + (1 - reverse * 2) * .5 / 12 * pc0[0], .5 / 15 * pc0[1]);
		point(pc, pc.normal(&p0, &p1) * (1 - 2 * reverse));
		glTexCoord2d(reverse + (1 - reverse * 2) * .5 / 12 * points[i][0], .5 / 15 * points[i][1]);
		point(p0, p0.normal(&p1, &pc) * (1 - 2 * reverse));
		glTexCoord2d(reverse + (1 - reverse * 2) * .5 / 12 * points[j][0], .5 / 15 * points[j][1]);
		point(p1, p1.normal(&pc, &p0) * (1 - 2 * reverse));
	}
}



void drawSides(double points[][2], int size, double plane0, double plane1, double pivot[2], double a0, double a1, double textureOffsetY0, double textureOffsetY1) {
	double sumLength = 0;
	double curLength = 0;
	double textureOffsetX = 0;
	if (plane0 > plane1)
	{
		double p = plane0;
		plane0 = plane1;
		plane1 = p;
		p = a0;
		a0 = a1;
		a1 = p;
	}
	for (size_t i = 0; i < size; i++)
	{
		int j = (i + 1) % size;
		Point3D p00_0 = Point3D(points[i][0], points[i][1], plane0).rotate(pivot, a0);
		Point3D p11_0 = Point3D(points[j][0], points[j][1], plane0).rotate(pivot, a0);
		double l = p11_0.subtract(&p00_0).length();
		sumLength += l;
	}
	for (size_t i = 0; i < size; i++)
	{
		int j = (i + 1) % size;
		if (!colorPolicy) setColor();
		Point3D p00_0 = Point3D(points[i][0], points[i][1], plane0).rotate(pivot, a0);
		Point3D p11_0 = Point3D(points[j][0], points[j][1], plane0).rotate(pivot, a0);
		Point3D p11_1 = Point3D(points[j][0], points[j][1], plane1).rotate(pivot, a1);
		double l = p11_0.subtract(&p00_0).length();
		glTexCoord2d(1. / sumLength * (curLength), .5 + .5 * textureOffsetY0);
		point(p00_0, p00_0.normal(&p11_0, &p11_1));
		glTexCoord2d(1. / sumLength * (curLength + l), .5 + .5 * textureOffsetY0);
		point(p11_0, p11_0.normal(&p11_1, &p00_0));
		glTexCoord2d(1. / sumLength * (curLength + l), .5 + .5 * textureOffsetY1);
		point(p11_1, p11_1.normal(&p00_0, &p11_0));
		if (!colorPolicy) setColor();
		Point3D p00_1 = Point3D(points[i][0], points[i][1], plane1).rotate(pivot, a1);
		glTexCoord2d(1. / sumLength * (curLength), .5 + .5 * textureOffsetY0);
		point(p00_0, p00_0.normal(&p11_1, &p00_1));
		//point(p00_0, p00_0.normal(&p11_0, &p11_1));
		glTexCoord2d(1. / sumLength * (curLength + l), .5 + .5 * textureOffsetY1);
		point(p11_1, p11_1.normal(&p00_1, &p00_0));
		//point(p11_1, p11_1.normal(&p00_0, &p11_0));
		glTexCoord2d(1. / sumLength * (curLength), .5 + .5 * textureOffsetY1);
		point(p00_1, p00_1.normal(&p00_0, &p11_1));
		curLength += l;
	}
}

void setColors() {
	if (col == 0)
	{
		for (size_t i = 0; i < colors; i++)
		{
			color[i] = random();
		}
	}
	col = 1;
}

void draw(double points[size][2]) {
	double angle = maxAngle * sin(t) + angle0;
	int steps = fabs(angle) / sliceStep + additionalSlices;
	double angleStep = angle / (steps + 1);
	double heightStep = (to - from) / (steps + 1);
	drawPlane(points, size, p0, from, p3, angle0, 1);
	for (size_t i = 0; i < steps + 1; i++)
	{
		drawSides(points, size, from + heightStep * i, from + heightStep * (i + 1), p3,
			angle0 + angleStep * i, angle0 + angleStep * (i + 1),
			i * heightStep / fabs(to - from), (i + 1) * heightStep / fabs(to - from));
	}
	drawPlane(points, size, p0, to, p3, angle, 0);
}

void loadTexture(char* filename, GLuint* texId) {
	RGBTRIPLE *texarray;

	//������ ��������, (������*������*4      4, ���������   ����, �� ������� ������������ �� 4 ����� �� ������� �������� - R G B A)
	char *texCharArray;
	int texW, texH;
	OpenGL::LoadBMP(filename, &texW, &texH, &texarray);
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);



	//���������� �� ��� ��������
	glGenTextures(1, texId);
	//������ ��������, ��� ��� ����� ����������� � ���������, ����� ����������� �� ����� ��
	glBindTexture(GL_TEXTURE_2D, *texId);

	//��������� �������� � �����������, � ���������� ��� ������  ��� �� �����
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);

	//�������� ������
	free(texCharArray);
	free(texarray);

	//������� ����
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glBindTexture(GL_TEXTURE_2D, 0);
}

GLuint getTextureId() {
	return texIds[texture];
}

void Render(double delta_time)
{
	t += delta_time * speed - (t >= 2 * pi) * 2 * pi;
	setColors();
	glBegin(GL_TRIANGLES);
	double points[size][2];
	int i = 0;
	int j = 0;
	for (; j < 3; i++, j++) {
		points[i][0] = points0[j][0];
		points[i][1] = points0[j][1];
	}
	double x, y, r;
	center(points0[2][0], p1[0], points0[3][0], points0[2][1], p1[1], points0[3][1], &x, &y);
	r = sqrt((x - p1[0]) * (x - p1[0]) + (y - p1[1]) * (y - p1[1]));
	double a0 = acos((points0[2][0] - x) / r);
	double a1 = acos((points0[3][0] - x) / r);
	if (y > points0[2][1]) a0 = -a0;
	if (y > points0[3][0]) a1 = -a1;
	if (a0 + 2 * pi < a1) a0 += 2 * pi;
	if (a0 - 2 * pi > a1) a0 -= 2 * pi;
	if (a0 < a1) a0 += 2 * pi;
	double astep = (a1 - a0) / (count + 1);
	for (int k = 0; k < count; k++, i++) {
		points[i][0] = cos((k + 1) * astep + a0) * r + x;
		points[i][1] = sin((k + 1) * astep + a0) * r + y;
	}
	for (; j < 6; i++, j++) {
		points[i][0] = points0[j][0];
		points[i][1] = points0[j][1];
	}
	center(points0[5][0], p2[0], points0[6][0], points0[5][1], p2[1], points0[6][1], &x, &y);
	r = sqrt((x - p2[0]) * (x - p2[0]) + (y - p2[1]) * (y - p2[1]));
	a0 = acos((points0[5][0] - x) / r);
	a1 = acos((points0[6][0] - x) / r);
	//if (y > points0[5][0]) a0 = -a0;
	//if (y > points0[6][0]) a1 = -a1;
	if (a0 + 2 * pi < a1) a0 += 2 * pi;
	if (a0 - 2 * pi > a1) a0 -= 2 * pi;
	//if (a0 > a1) a0 -= 2 * pi;
	astep = (a1 - a0) / (count + 1);
	for (int k = 0; k < count; k++, i++) {
		points[i][0] = cos((k + 1) * astep + a0) * r + x;
		points[i][1] = sin((k + 1) * astep + a0) * r + y;
	}
	for (; j < 8; i++, j++) {
		points[i][0] = points0[j][0];
		points[i][1] = points0[j][1];
	}
	draw(points);
	glEnd();
}

/////////////////////////////////////////////////

bool textureMode = true;
bool lightMode = true;

//����� ��� ��������� ������
class CustomCamera : public Camera
{
public:
	//��������� ������
	double camDist;
	//���� �������� ������
	double fi1, fi2;

	
	//������� ������ �� ���������
	CustomCamera()
	{
		camDist = 15;
		fi1 = 1;
		fi2 = 1;
	}

	
	//������� ������� ������, ������ �� ����� ��������, ���������� �������
	void SetUpCamera()
	{
		//�������� �� ������� ������ ������
		lookPoint.setCoords(0, 0, 0);

		pos.setCoords(camDist*cos(fi2)*cos(fi1),
			camDist*cos(fi2)*sin(fi1),
			camDist*sin(fi2));

		if (cos(fi2) <= 0)
			normal.setCoords(0, 0, -1);
		else
			normal.setCoords(0, 0, 1);

		LookAt();
	}

	void CustomCamera::LookAt()
	{
		//������� ��������� ������
		gluLookAt(pos.X(), pos.Y(), pos.Z(), lookPoint.X(), lookPoint.Y(), lookPoint.Z(), normal.X(), normal.Y(), normal.Z());
	}



}  camera;   //������� ������ ������


//����� ��� ��������� �����
class CustomLight : public Light
{
public:
	CustomLight()
	{
		//��������� ������� �����
		pos = Vector3(1, 1, 3);
	}

	
	//������ ����� � ����� ��� ���������� �����, ���������� �������
	void  DrawLightGhismo()
	{
		glDisable(GL_LIGHTING);

		
		glColor3d(0.9, 0.8, 0);
		Sphere s;
		s.pos = pos;
		s.scale = s.scale*0.08;
		s.Show();
		
		if (OpenGL::isKeyPressed('G'))
		{
			glColor3d(0, 0, 0);
			//����� �� ��������� ����� �� ����������
			glBegin(GL_LINES);
			glVertex3d(pos.X(), pos.Y(), pos.Z());
			glVertex3d(pos.X(), pos.Y(), 0);
			glEnd();

			//������ ���������
			Circle c;
			c.pos.setCoords(pos.X(), pos.Y(), 0);
			c.scale = c.scale*1.5;
			c.Show();
		}

	}

	void SetUpLight()
	{
		GLfloat amb[] = { 0.2, 0.2, 0.2, 0 };
		GLfloat dif[] = { 1.0, 1.0, 1.0, 0 };
		GLfloat spec[] = { .7, .7, .7, 0 };
		GLfloat position[] = { pos.X(), pos.Y(), pos.Z(), 1. };

		// ��������� ��������� �����
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		// �������������� ����������� �����
		// ������� ��������� (���������� ����)
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		// ��������� ������������ �����
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
		// ��������� ���������� ������������ �����
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

		glEnable(GL_LIGHT0);
	}


} light;  //������� �������� �����




//������ ���������� ����
int mouseX = 0, mouseY = 0;

void mouseEvent(OpenGL *ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//������ ���� ������ ��� ������� ����� ������ ����
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.01*dx;
		camera.fi2 += -0.01*dy;
	}

	
	//������� ���� �� ���������, � ����� ��� ����
	if (OpenGL::isKeyPressed('G') && !OpenGL::isKeyPressed(VK_LBUTTON))
	{
		LPPOINT POINT = new tagPOINT();
		GetCursorPos(POINT);
		ScreenToClient(ogl->getHwnd(), POINT);
		POINT->y = ogl->getHeight() - POINT->y;

		Ray r = camera.getLookRay(POINT->x, POINT->y);

		double z = light.pos.Z();

		double k = 0, x = 0, y = 0;
		if (r.direction.Z() == 0)
			k = 0;
		else
			k = (z - r.origin.Z()) / r.direction.Z();

		x = k*r.direction.X() + r.origin.X();
		y = k*r.direction.Y() + r.origin.Y();

		light.pos = Vector3(x, y, z);
	}

	if (OpenGL::isKeyPressed('G') && OpenGL::isKeyPressed(VK_LBUTTON))
	{
		light.pos = light.pos + Vector3(0, 0, 0.02*dy);
	}

	
}

void mouseWheelEvent(OpenGL *ogl, int delta)
{

	if (delta < 0 && camera.camDist <= 1)
		return;
	if (delta > 0 && camera.camDist >= 100)
		return;

	camera.camDist += 0.01*delta;

}

void keyDownEvent(OpenGL *ogl, int key)
{
	if (key == 'L')
	{
		lightMode = !lightMode;
	}

	if (key == 'T')
	{
		textureMode = !textureMode;
	}

	if (key == 'R')
	{
		camera.fi1 = 1;
		camera.fi2 = 1;
		camera.camDist = 15;

		light.pos = Vector3(1, 1, 3);
	}

	if (key == 'F')
	{
		light.pos = camera.pos;
	}

	if (key == 'N')
	{
		texture++;
		texture -= textureCount * (texture >= textureCount);
	}

	if (key == 'P')
	{
		colorPolicy++;
		colorPolicy -= colorPolicyCount * (colorPolicy >= colorPolicyCount);
	}

	if (key == 'A')
	{
		alpha = !alpha;
	}
}

void keyUpEvent(OpenGL *ogl, int key)
{
	
}



//����������� ����� ������ ��������
void initRender(OpenGL *ogl)
{
	//��������� �������

	//4 ����� �� �������� �������
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//��������� ������ ��������� �������
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//�������� ��������
	glEnable(GL_TEXTURE_2D);
	

	//������ ����������� ���������  (R G B)
	loadTexture("texture.bmp", &texIds[0]);
	loadTexture("texture1.bmp", &texIds[1]);
	loadTexture("texture2.bmp", &texIds[2]);


	//������ � ���� ����������� � "������"
	ogl->mainCamera = &camera;
	ogl->mainLight = &light;

	// ������������ �������� : �� ����� ����� ����� 1
	glEnable(GL_NORMALIZE);

	// ���������� ������������� ��� �����
	glEnable(GL_LINE_SMOOTH); 


	//   ������ ��������� ���������
	//  �������� GL_LIGHT_MODEL_TWO_SIDE - 
	//                0 -  ������� � ���������� �������� ���������(�� ���������), 
	//                1 - ������� � ���������� �������������� ������� ��������       
	//                �������������� ������� � ���������� ��������� ����������.    
	//  �������� GL_LIGHT_MODEL_AMBIENT - ������ ������� ���������, 
	//                �� ��������� �� ���������
	// �� ��������� (0.2, 0.2, 0.2, 1.0)

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);

	camera.fi1 = -1.3;
	camera.fi2 = 0.8;
}

void Render(OpenGL *ogl)
{



	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
	if (textureMode)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);


	//альфаналожение
	/////////////////////////////////////////////////////////////
	if (alpha) {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	/////////////////////////////////////////////////////////////


	//настройка материала
	GLfloat amb[] = { 0.2, 0.2, 0.1, 1. };
	GLfloat dif[] = { 0.4, 0.65, 0.5, 1. };
	GLfloat spec[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh = 0.1f * 256;


	//фоновая
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//дифузная
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//зеркальная
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec); \
		//размер блика
		glMaterialf(GL_FRONT, GL_SHININESS, sh);

	//чтоб было красиво, без квадратиков (сглаживание освещения)
	glShadeModel(GL_SMOOTH);
	//===================================
	//Прогать тут


	//������ ��������� ���������� ��������
	double A[2] = { -4, -4 };
	double B[2] = { 4, -4 };
	double C[2] = { 4, 4 };
	double D[2] = { -4, 4 };

	glBindTexture(GL_TEXTURE_2D, getTextureId());

	/*glColor3d(0.6, 0.6, 0.6);
	glBegin(GL_QUADS);

	glNormal3d(0, 0, 1);
	glTexCoord2d(0, 0);
	glVertex2dv(A);
	glTexCoord2d(1, 0);
	glVertex2dv(B);
	glTexCoord2d(1, 1);
	glVertex2dv(C);
	glTexCoord2d(0, 1);
	glVertex2dv(D);

	glEnd();*/


	//////////////////////////////////////////////
	Render(0.1);
	//////////////////////////////////////////////




	//����� ��������� ���������� ��������


   //��������� ������ ������

	
	glMatrixMode(GL_PROJECTION);	//������ �������� ������� ��������. 
	                                //(���� ��������� ��������, ����� �� ������������.)
	glPushMatrix();   //��������� ������� ������� ������������� (������� ��������� ������������� ��������) � ���� 				    
	glLoadIdentity();	  //��������� ��������� �������
	glOrtho(0, ogl->getWidth(), 0, ogl->getHeight(), 0, 1);	 //������� ����� ������������� ��������

	glMatrixMode(GL_MODELVIEW);		//������������� �� �����-��� �������
	glPushMatrix();			  //��������� ������� ������� � ���� (��������� ������, ����������)
	glLoadIdentity();		  //���������� �� � ������

	glDisable(GL_LIGHTING);



	GuiTextRectangle rec;		   //������� ����� ��������� ��� ������� ������ � �������� ������.
	rec.setSize(300, 150);
	rec.setPosition(10, ogl->getHeight() - 150 - 10);


	std::stringstream ss;
	ss << "T - Toggle textures" << std::endl;
	ss << "L - Toggle lighting" << std::endl;
	ss << "F - Toggle light from camera" << std::endl;
	ss << "G - move light horizontally" << std::endl;
	ss << "G+LMK move light vertically" << std::endl;
	ss << "light coordinates: (" << light.pos.X() << ", " << light.pos.Y() << ", " << light.pos.Z() << ")" << std::endl;
	ss << "cam coordinates: (" << camera.pos.X() << ", " << camera.pos.Y() << ", " << camera.pos.Z() << ")" << std::endl;
	ss << "camera params: R=" << camera.camDist << ", fi1=" << camera.fi1 << ", fi2=" << camera.fi2 << std::endl;
	ss << "N - Next texture" << std::endl;
	ss << "P - Next color policy" << std::endl;
	
	rec.setText(ss.str().c_str());
	rec.Draw();

	glMatrixMode(GL_PROJECTION);	  //��������������� ������� �������� � �����-��� �������� �� �����.
	glPopMatrix();


	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}