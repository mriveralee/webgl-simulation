#pragma once

#include "MathLib/Plane.h"
#include "MathLib/P3D.h"
#include "MathLib/Ray.h"

#include "StretchedColor.h"

// Assumes this is flat on the plance (X-z)
class StretchedFlatSurface {

public:
	StretchedFlatSurface(P3D centerPt);
	StretchedFlatSurface(double size, P3D centerPt);
	StretchedFlatSurface(double length, double width, P3D centerPt);
	~StretchedFlatSurface();

	bool isWithinSurface(P3D point);
	void draw();

	P3D pointClosestToPlane(Ray ray);
	P3D transformClickedPointToSurface(P3D clickedPoint);

	double getWidth();
	double getLength();
	double getX();
	double getY();
	double getZ();
	P3D getCenter();
	V3D getNormal();
	Plane getPlane();
	StretchedColor getColor();

	void setColor(StretchedColor color);

private:
	StretchedColor color;
	Plane plane;
	P3D center;
	std::vector<P3D> corners;
	V3D normal;	// We assume this is our global up direction
	double length;
	double width;

	// Fxns
	void computeCorners();

};

