#pragma once

#include "MathLib/P3D.h"

#include "DelaunayTriangulation.h"

enum DelaunayTriangulatorPlane2DType {
	XY_PLANE,
	XZ_PLANE,
	YZ_PLANE
};

// Takes a set of points (P3D) and converts them to a Delaunay Triangulation
class DelaunayTriangulator {

public:
	DelaunayTriangulator();
	~DelaunayTriangulator();

	// take a list of points and returns a delaunay triangulation of those points, SUP.
	DelaunayTriangulation triangulatePoints(std::vector<P3D> points, DelaunayTriangulatorPlane2DType planeType);

};

