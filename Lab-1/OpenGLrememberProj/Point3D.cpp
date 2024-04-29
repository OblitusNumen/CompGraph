#include "Point3D.h"
#include <math.h>

Point3D Point3D::rotate(double pivot[2], double angle) {
	double r = sqrt((x - pivot[0]) * (x - pivot[0]) + (y - pivot[1]) * (y - pivot[1]));
	double a0 = acos((pivot[0] - x) / r);
	if (y > pivot[1]) a0 = -a0;
	return Point3D(cos(a0 + angle) * r + pivot[0], sin(a0 + angle) * r + pivot[1], z);
}

Point3D Point3D::shift(double xShift, double yShift, double zShift) {
	return Point3D(x + xShift, y + yShift, z + zShift);
}

Point3D Point3D::subtract(Point3D* p) {
	return Point3D(x - p->x, y - p->y, z - p->z);
}

double Point3D::lengthSquared()
{
	return x * x + y * y + z * z;
}

double Point3D::length() {
	return sqrt(lengthSquared());
}

Vector3D Point3D::normalize() {
	double l = length();
	return Vector3D(x / l, y / l, z / l);
}

Vector3D Point3D::operator*(Vector3D v)
{
	return Vector3D(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
}

Vector3D Point3D::operator*(double m)
{
	return Vector3D(x * m, y * m, z * m);
}

Point3D Point3D::operator+(Point3D p)
{
	return Point3D(x + p.x, y + p.y, z + p.z);
}

Point3D Point3D::operator-()
{
	return Point3D(-x, -y, -z);
}

Point3D Point3D::safe()
{
	return Point3D(x + (x == 0) * .00001, y + (y == 0) * .00001, z + (z == 0) * .00001);
}

Vector3D Point3D::normal(Point3D* pR, Point3D* pL) {
	Vector3D vR = pR->subtract(this);
	Vector3D vL = pL->subtract(this);
	return vR * vL;
}
