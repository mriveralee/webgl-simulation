#pragma once

#include "MathLib/P3D.h"
#include "StretchedColor.h"
#include "StretchedTriangle.h"

class DelaunayTriangulation {

public:

	DelaunayTriangulation(std::vector<P3D> pts, std::vector<int> triangulationIndices);
	DelaunayTriangulation(std::vector<P3D> pts, std::vector<int> triangulationIndices, std::vector<int> convexHullIndices);
	DelaunayTriangulation();
	~DelaunayTriangulation();

	std::vector<P3D> getOrderedTriangulationPoints();
	std::vector<StretchedTriangle> processTriangles();

	std::vector<P3D> DelaunayTriangulation::getTriangulationPts();
	std::vector<int> DelaunayTriangulation::getTriangulationIndices();
	std::vector<P3D> getConvexHull();

	void setColor(StretchedColor c);
	void draw();

private:
	StretchedColor color;
	std::vector<P3D> triangulationPts;
	std::vector<int> triangulationIndices;
	std::vector<P3D> orderedTriangulationPts;
	std::vector<int> convexHullIndices;

};

