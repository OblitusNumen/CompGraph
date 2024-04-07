#pragma once

class Point3D;

typedef Point3D Vector3D;

class Point3D
{
public:
	const double x;
	const double y;
	const double z;

	Point3D(double xx, double yy, double zz) : x(xx), y(yy), z(zz) {}

	Point3D rotate(double pivot[2], double angle);

	Point3D shift(double, double, double);

	Point3D subtract(Point3D*);

	Vector3D normal(Point3D*, Point3D*);

	double length();

	Vector3D normalize();

	Vector3D operator*(Vector3D);

	Vector3D operator*(double);
};