#pragma once

#include "MathLib/P3D.h"
#include "MathLib/BezierCurve.h"

#include "StretchedColor.h"


// This is used to show a circle extrusion that is being edited in 
// the design window. Upon completion the curve is added to the extrusion curves

const double DEFAULT_EXTRUSION_CIRCLE_RADIUS = 1.5;
const int DEFAULT_EXTRUSION_CIRCLE_SEGMENTS = 30;

class StretchedExtrusionCircle {

private:
	double radius;
	int numSegments;

	void init();

public:
	static double CIRCLE_RADIUS;
	P3D centerPt;

	StretchedColor controlPtColor;
	StretchedColor lineColor;
	StretchedColor curveColor;

	StretchedExtrusionCircle();
	StretchedExtrusionCircle(P3D centerPt);
	StretchedExtrusionCircle(P3D centerPt, double radius);
	~StretchedExtrusionCircle();

	double getRadius();
	void setRadius(double r);

	void draw();
	std::vector<P3D> pointsOnCurve();
	std::vector<P3D> pointsForTriangulation();
	void updateCenterPoint(P3D point);

	P3D getCenterPoint();

};
