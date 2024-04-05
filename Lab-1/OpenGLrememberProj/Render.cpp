#include "Render.h"

#include <Windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>
#include <math.h>
#include <stdio.h>

double t = 0;
double pi = 3.14159265358979;

int col = 0;
const int colors = 100000;
double color[colors];

//x,y,z,density,r,g,b
int lightSourceCount = 3;
double lightSources[][7] = { { 0, 0, 0, 0.2, .8, .2, .6 }, { 5, 10, 4, 0.03, .02, .06, .08 }, { 10, 5, 8, 0.4, .9, .9, .6 } };

int colorPolicy = 1;

double p0[]{ 6, 6 };
double p1[]{11, 4};
double p2[]{11, 4};
double p3[]{ 5, 7 };
double points0[8][2] = { {1, 0}, {6, 4}, {11, 1}, {12, 6}, {8, 8}, {5, 15}, {0, 12}, {5, 6} };
const int count = 3;
const int size = 8 + count * 2;

double from = 0;
double to = 10;
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
	glColor3d(r, g, b);
}

void setColor() {
	glColor3d(color[col++], color[col++], color[col++]);
}

void point(double x, double y, double z, double pivot[2], double angle) {
	double r = sqrt((x - pivot[0]) * (x - pivot[0]) + (y - pivot[1]) * (y - pivot[1]));
	double a0 = acos((pivot[0] - x) / r);
	if (y > pivot[1]) a0 = -a0;
	x = cos(a0 + angle) * r + pivot[0];
	y = sin(a0 + angle) * r + pivot[1];
	if (colorPolicy == 1) setColor(x, y, z);
	x += xShift;
	y += yShift;
	z += zShift;
	if (colorPolicy == 2) setColor(x, y, z);
	glVertex3d(x, y, z);
}

void drawPlane(double points[][2], int size, double p0[2], double plane, double pivot[2], double angle) {
	for (size_t i = 0; i < size; i++)
	{
		int j = (i + 1) % size;
		if (!colorPolicy) setColor();
		point(p0[0], p0[1], plane, pivot, angle);
		point(points[i][0], points[i][1], plane, pivot, angle);
		point(points[j][0], points[j][1], plane, pivot, angle);
	}
}

void drawSides(double points[][2], int size, double plane1, double plane2, double pivot[2], double a0, double a1) {
	for (size_t i = 0; i < size; i++)
	{
		int j = (i + 1) % size;
		if (!colorPolicy) setColor();
		point(points[i][0], points[i][1], plane1, pivot, a0);
		point(points[j][0], points[j][1], plane1, pivot, a0);
		point(points[j][0], points[j][1], plane2, pivot, a1);
		if (!colorPolicy) setColor();
		point(points[i][0], points[i][1], plane1, pivot, a0);
		point(points[i][0], points[i][1], plane2, pivot, a1);
		point(points[j][0], points[j][1], plane2, pivot, a1);
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
	drawPlane(points, size, p0, from, p3, angle0);
	for (size_t i = 0; i < steps + 1; i++)
	{
		drawSides(points, size, from + heightStep * i, from + heightStep * (i + 1), p3, angle0 + angleStep * i, angle0 + angleStep * (i + 1));
	}
	drawPlane(points, size, p0, to, p3, angle);
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