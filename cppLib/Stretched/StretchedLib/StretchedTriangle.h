#pragma once

#include "MathLib/P3D.h"


// Assume the pts in the triangle are ordered counterclockwise
class StretchedTriangle {

public:
	P3D p1, p2, p3;

	StretchedTriangle(P3D pt1, P3D pt2, P3D pt3);
	~StretchedTriangle();

	static double calculateArea(P3D pt1, P3D pt2, P3D pt3);
	void draw();

};

