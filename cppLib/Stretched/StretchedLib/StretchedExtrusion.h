#pragma once

#include "MathLib/P3D.h"

#include "StretchedColor.h"

// This class defines a set of points which form a the extrusion
// We use this to export the points to for D.Triangulation, as well
// as displaying the completed extrusion in the design window
class StretchedExtrusion {

public:
	StretchedExtrusion();
	~StretchedExtrusion();

	// Ordered list of points
	std::vector<P3D> points;
	StretchedColor color;

	void addPoint(P3D pt);
	void clearPoints();

	void draw();

};

