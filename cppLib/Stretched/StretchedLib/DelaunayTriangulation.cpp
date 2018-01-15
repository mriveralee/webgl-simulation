#include "DelaunayTriangulation.h"

#include "GUILib/GLUtils.h"


static const GLfloat DELAUNAY_TRIANGULATION_LINE_WIDTH = 2;

DelaunayTriangulation::DelaunayTriangulation(std::vector<P3D> triangulationPts, std::vector<int> triangulationIndices) : DelaunayTriangulation(triangulationPts, triangulationIndices, std::vector<int>()) {
}

DelaunayTriangulation::DelaunayTriangulation(std::vector<P3D> triangulationPts, std::vector<int> triangulationIndices, std::vector<int> convexHullIndices) {
	this->triangulationPts = triangulationPts;
	this->triangulationIndices = triangulationIndices;
	this->convexHullIndices = convexHullIndices;
}

DelaunayTriangulation::DelaunayTriangulation() {
	// Nothing to see here
}

DelaunayTriangulation::~DelaunayTriangulation() {
	// Nothing to see here
}

void DelaunayTriangulation::setColor(StretchedColor c) {
	color = c;
}

std::vector<P3D> DelaunayTriangulation::getTriangulationPts() {
	return triangulationPts;
}

std::vector<int> DelaunayTriangulation::getTriangulationIndices() {
	return triangulationIndices;
}

std::vector<P3D> DelaunayTriangulation::getConvexHull() {
	std::vector<P3D> convexHullPts = std::vector<P3D>();
	for (int i = 0; i < (int)convexHullIndices.size(); i++) {
		convexHullPts.push_back(triangulationPts[convexHullIndices[i]]);
	}
	return convexHullPts;
}


std::vector<StretchedTriangle> DelaunayTriangulation::processTriangles() {
	std::vector<P3D> orderedPts = getOrderedTriangulationPoints();
	std::vector<StretchedTriangle> triangles = std::vector<StretchedTriangle>();
	int ptsSize = orderedPts.size();
	for (int i = 0; i < ptsSize - 2; i+=3) {
		P3D p1 = orderedPts[i];
		P3D p2 = orderedPts[i + 1];
		P3D p3 = orderedPts[i + 2];
		StretchedTriangle triangle = StretchedTriangle(p1, p2, p3);
		triangles.push_back(triangle);
	}
	return triangles;
}

std::vector<P3D> DelaunayTriangulation::getOrderedTriangulationPoints() {
	// Take the points and construct a list of points s.t. that the points are ordered the same way as
	// the triangulationIndices
	if (orderedTriangulationPts.size() != 0) {
		// Only do the construction once, and save the result for later
		return orderedTriangulationPts;
	}
	// Construct the points
	orderedTriangulationPts.clear();
	for (int i = 0; i < (int) triangulationIndices.size(); i++) {
		int ptIndex = triangulationIndices[i];
		if (ptIndex >= (int) triangulationPts.size()) {
			Logger::consolePrint("Duplicate points were removed this may result in triangulation errors...");
			continue;
		}
		orderedTriangulationPts.push_back(triangulationPts[ptIndex]);
	}
	return orderedTriangulationPts;
}

void DelaunayTriangulation::draw() {
	std::vector<P3D> orderedPts = getOrderedTriangulationPoints();
	// take the points, create an ordered list and draw it
	glLineWidth(DELAUNAY_TRIANGULATION_LINE_WIDTH);
	glColor4d(color.red, color.green, color.blue, color.alpha);
	glBegin(GL_LINES);
	int ptsSize = orderedPts.size();
	for (int i = 0; i < ptsSize - 2; i += 3) {
		P3D p1 = orderedPts[i];
		P3D p2 = orderedPts[i + 1];
		P3D p3 = orderedPts[i + 2];
		// Edge AB
		glVertex3d(p1[0], p1[1], p1[2]);
		glVertex3d(p2[0], p2[1], p2[2]);

		// Edge BC
		glVertex3d(p2[0], p2[1], p2[2]);
		glVertex3d(p3[0], p3[1], p3[2]);

		// Edge CA
		glVertex3d(p3[0], p3[1], p3[2]);
		glVertex3d(p1[0], p1[1], p1[2]);
	}
	glEnd();
	glLineWidth(1);
}
