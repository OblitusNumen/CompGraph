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

//////////////
//double zzz = 0;
//Point3D ppp;
//Point3D ppp1;
//Point3D ppp2;
//Point3D ppp3;
//Point3D ppp4;
////////////

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
double lightSources[][7] = { { 0, 0, 0, 0.2, .8, .2, .6 }, { -2, 0, -4, 0.03, .02, .06, .08 }, { 1, -5, 3, 0.4, .9, .9, .6 } };

int colorPolicy = 0;
int colorPolicyCount = 3;

double speed = .1;

//surface
const int dotCount = 30;
const int controlPC = 5;
double heights[controlPC][controlPC] = { {1, 5, 3, 4, 9}, {5, 2, 7, 8, 0}, {9, 3, 5, 1, 10}, {3, 7, 3, 5, 5}, {2, 8, 1, 0, 4} };
//double heights[controlPC][controlPC] = { {0, 0, 0, 0}, {5, 2, 7, 8}, {9, 3, 5, 1}, {3, 7, 3, 5} };
double fromX = 0;
double fromY = 0;
double length = 20;
double minDistSquaredControl = 1;

double xShift = 0;
double yShift = 0;
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
	glColor4d(color[col++], color[col++], color[col++], 1.);
}

void point(Point3D p, Vector3D normal) {
	if (colorPolicy == 1) setColor(p.x, p.y, p.z);
	Point3D p1 = p.shift(xShift, yShift, zShift);
	if (colorPolicy == 2) setColor(p1.x, p1.y, p1.z);
	glNormal3d(normal.x, normal.y, normal.z);
	glVertex3d(p1.x, p1.y, p1.z);
}

Point3D intersect(Point3D planeP, Vector3D planeN, Point3D lineP, Vector3D line) {
	planeP = planeP.safe();
	planeN = planeN.safe();
	lineP = lineP.safe();
	line = line.safe();
	double y0 = -lineP.x / line.x * line.y + lineP.y;
	double z0 = -lineP.x / line.x * line.z + lineP.z;
	double d = (planeN.x * planeP.x + planeN.y * planeP.y + planeN.z * planeP.z);
	double x = (-planeN.y * y0 - planeN.z * z0 + d) / (planeN.x + planeN.y * line.y / line.x + planeN.z * line.z / line.x);
	point(Point3D(x, x * line.y / line.x + y0, x * line.z / line.x + z0), Vector3D());
	return Point3D(x, x * line.y / line.x + y0, x * line.z / line.x + z0);
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

Point3D getBezierPoint(Point3D** points, int pCount, double t) {
	if (pCount == 1) return *(points[0]);
	Point3D** newPoints = new Point3D*[pCount - 1];
	for (int i = 0; i < pCount - 1; i++)
	{
		newPoints[i] = new Point3D((*(points[i]) * t) + (*(points[i + 1]) * (1 - t)));
	}
	Point3D point = getBezierPoint(newPoints, pCount - 1, t);
	for (int i = 0; i < pCount - 1; i++)
	{
		delete newPoints[i];
	}
	delete[] newPoints;
	return point;
}

void plane(Vector3D v1, Vector3D v2, Vector3D v3, Point3D offset, int light) {
	Vector3D n = (v1 + v2 + v3).normal(&(v1 + v2 + v3 * -1), &(v1 + v2 * -1 + v3 * -1)) * -light;
	setColor();
	point(v1 + v2 + v3 + offset, n);
	point(v1 + v2 + v3 * -1 + offset, n);
	point(v1 + v2 * -1 + v3 * -1 + offset, n);
	setColor();
	point(v1 + v2 + v3 + offset, n);
	setColor();
	point(v1 + v2 * -1 + v3 + offset, n);
	setColor();
	point(v1 + v2 * -1 + v3 * -1 + offset, n);
}

void drawCube(Point3D p1, Point3D p2, Point3D p3) {
	glBegin(GL_TRIANGLES);
	Vector3D v1 = p2.subtract(&p1).normalize() * .2;
	Vector3D v2 = p1.normal(&p2, &p3).normalize() * .2;
	Vector3D v3 = (v1 * v2).normalize() * .2;
	plane(v1, v2, v3, p1, 1);
	plane(v2, v3, v1, p1, 1);
	plane(v3, v1, v2, p1, 1);
	plane(-v1, -v2, -v3, p1, -1);
	plane(-v2, -v3, -v1, p1, -1);
	plane(-v3, -v1, -v2, p1, -1);
	glEnd();
}

double bezierHeight(double* points, int pCount, double t) {
	if (pCount == 1) return points[0];
	double* newPoints = new double[pCount - 1];
	for (int i = 0; i < pCount - 1; i++)
	{
		newPoints[i] = (points[i] * (1 - t)) + (points[i + 1] * (t));
	}
	double point = bezierHeight(newPoints, pCount - 1, t);
	delete[] newPoints;
	return point;
}

void drawSurface() {
	/*glBegin(GL_POINTS);
	for (int row = 0; row < controlPC; row++)
	{
		for (int curPoint = 0; curPoint < controlPC; curPoint++) {
			glVertex3d(length * (row) / controlPC, length * (curPoint) / controlPC, heights[curPoint][row]);
		}
	}
	glEnd();*/

	glBegin(GL_LINES);
	for (int row = 0; row < controlPC; row++)
	{
		for (int curPoint = 0; curPoint < controlPC; curPoint++) {
			if (curPoint != 0)
			{
				glVertex3d(length * (row) / (controlPC - 1), length * (curPoint) / (controlPC - 1), heights[curPoint][row]);
				glVertex3d(length * (row) / (controlPC - 1), length * (curPoint - 1) / (controlPC - 1), heights[curPoint - 1][row]);
			}
			if (row != 0) {
				glVertex3d(length * (row) / (controlPC - 1), length * (curPoint) / (controlPC - 1), heights[curPoint][row]);
				glVertex3d(length * (row - 1) / (controlPC - 1), length * (curPoint) / (controlPC - 1), heights[curPoint][row - 1]);
			}
		}
	}
	glEnd();

	glBegin(GL_TRIANGLES);
	double* line = new double[dotCount];
	for (int row = 0; row < dotCount; row++)
	{
		double controlL1[controlPC];
		for (int i = 0; i < controlPC; i++)
		{
			controlL1[i] = bezierHeight(heights[i], controlPC, (double)row / (dotCount - 1));
		}
		double prev;
		for (int curPoint = 0; curPoint < dotCount; curPoint++) {
			double current = bezierHeight(controlL1, controlPC, (double)curPoint / (dotCount - 1));
			if (curPoint != 0) {
				if (row != 0) {
					{
						Point3D p1(length * (row - 1) / (dotCount - 1), length * (curPoint - 1) / (dotCount - 1), line[curPoint - 1]);
						Point3D p2(length * (row - 1) / (dotCount - 1), length * (curPoint) / (dotCount - 1), line[curPoint]);
						Point3D p3(length * (row) / (dotCount - 1), length * (curPoint) / (dotCount - 1), current);
						Vector3D n = p1.normal(&p3, &p2);
						glTexCoord2d((double)(row - 1) / (dotCount - 1), (double)(curPoint - 1) / (dotCount - 1));
						point(p1, n);
						glTexCoord2d((double)(row - 1) / (dotCount - 1), (double)(curPoint) / (dotCount - 1));
						point(p2, n);
						glTexCoord2d((double)(row) / (dotCount - 1), (double)(curPoint) / (dotCount - 1));
						point(p3, n);
					}
					{
						Point3D p1(length * (row - 1) / (dotCount - 1), length * (curPoint - 1) / (dotCount - 1), line[curPoint - 1]);
						Point3D p2(length * (row) / (dotCount - 1), length * (curPoint) / (dotCount - 1), current);
						Point3D p3(length * (row) / (dotCount - 1), length * (curPoint - 1) / (dotCount - 1), prev);
						Vector3D n = p1.normal(&p3, &p2);
						glTexCoord2d((double)(row - 1) / (dotCount - 1), (double)(curPoint - 1) / (dotCount - 1));
						point(p1, n);
						glTexCoord2d((double)(row) / (dotCount - 1), (double)(curPoint) / (dotCount - 1));
						point(p2, n);
						glTexCoord2d((double)(row) / (dotCount - 1), (double)(curPoint - 1) / (dotCount - 1));
						point(p3, n);
					}
					/*line[point - 1];
					line[point];
					current;

					line[point - 1];
					current;
					prev;*/
				}
				line[curPoint - 1] = prev;
			}
			prev = current;
		}
		line[dotCount - 1] = prev;
	}
	delete line;
	glEnd();
}

void Render(double delta_time)
{
	t += delta_time * speed - (t >= 2 * pi) * 2 * pi;
	glBegin(GL_LINES);
	setColors();

	//curve
	const int pCount = 100;
	const int vertexCount = 8;
	Point3D* points[vertexCount]{ &Point3D{-6, -3, -8}, &Point3D{-2, -1, -1}, &Point3D{0, -11, -17}, &Point3D{-10, -4, -8},
		&Point3D{-2, -10, -19}, &Point3D{-12, -17, -11}, &Point3D{-10, 0, 0}, &Point3D{-6, -1, -8} };

	setColor();
	for (int i = 0; i < vertexCount - 1; i++)
	{
		point(*points[i], Vector3D());
		point(*points[i + 1], Vector3D());
	}

	setColor();
	Point3D p1 = getBezierPoint(points, vertexCount, (double)0 / pCount);
	Point3D p2;
	for (int i = 0; i < pCount; i++)
	{
		p2 = getBezierPoint(points, vertexCount, (double)(i + 1) / pCount);
		point(p1, Vector3D());
		setColor();
		point(p2, Vector3D());
		p1 = p2;
	}
	glEnd();

	p1 = getBezierPoint(points, vertexCount, (double)((sin(t) + 1) / 2));
	p2 = getBezierPoint(points, vertexCount, (double)(sin(t) + 1 - (1. / pCount)) / 2);
	Point3D p3 = getBezierPoint(points, vertexCount, (double)(sin(t) + 1 - (2. / pCount)) / 2);
	drawCube(p1, p2, p3);

	drawSurface();
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
int px = -1, py = -1;

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

	//moving bezier points
	if (OpenGL::isKeyPressed(VK_LBUTTON))
	{
		if (px == -1) {
			//choose point here
			Vector3 cn = camera.pos;
			Vector3 cp = camera.pos;
			Point3D cameraPos = Point3D(cp.X(), cp.Y(), cp.Z());
			Vector3D planeN(cn.X(), cn.Y(), cn.Z());
			Point3D planeP = planeN + cameraPos;
			LPPOINT POINT = new tagPOINT();
			GetCursorPos(POINT);
			ScreenToClient(ogl->getHwnd(), POINT);
			POINT->y = ogl->getHeight() - POINT->y;
			Ray r = camera.getLookRay(POINT->x, POINT->y);
			Vector3D cursor(r.direction.X(), r.direction.Y(), r.direction.Z());
			Point3D cursorProj = intersect(planeP, planeN, cameraPos, cursor);
			double minDistSquared = intersect(planeP, planeN, cameraPos, Point3D(0, 0, heights[0][0]).subtract(&cameraPos)).subtract(&cursorProj).lengthSquared();
			int i = 0;
			int j = 0;
			for (int k = 0; k < controlPC; k++)
			{
				for (int w = 0; w < controlPC; w++) {
					double distSquared = intersect(planeP, planeN, cameraPos,
						Point3D(length * (w) / (controlPC - 1), length * (k) / (controlPC - 1), heights[k][w])
						.subtract(&cameraPos)).subtract(&cursorProj).lengthSquared();
					if (minDistSquared > distSquared)
					{
						i = k;
						j = w;
						minDistSquared = distSquared;
					}
				}
			}
			if (minDistSquared < minDistSquaredControl)
			{
				px = i;
				py = j;
			}
		}
		else {
			//move point here
			Vector3 cn = camera.pos;
			Vector3 cp = camera.pos;
			Point3D cameraPos = Point3D(cp.X(), cp.Y(), cp.Z());
			Vector3D planeN(cn.X(), cn.Y(), 0);
			LPPOINT POINT = new tagPOINT();
			GetCursorPos(POINT);
			ScreenToClient(ogl->getHwnd(), POINT);
			POINT->y = ogl->getHeight() - POINT->y;
			Ray r = camera.getLookRay(POINT->x, POINT->y);
			Vector3D cursor(r.direction.X(), r.direction.Y(), r.direction.Z());
			Point3D movable(length * (py) / (controlPC - 1), length * (px) / (controlPC - 1), heights[px][py]);
			Point3D intersection = intersect(movable, planeN, cameraPos, cursor);
			heights[px][py] = intersection.z;
		}
	}
	else {
		px = -1;
		py = -1;
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