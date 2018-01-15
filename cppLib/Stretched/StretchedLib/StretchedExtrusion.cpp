#include "StretchedExtrusion.h"
#include "GUILib/GLUtils.h"

const bool DRAW_EXTRUSION_POINTS = true;
const double POINT_SIZE = 4;
const double LINE_WIDTH = 170;

StretchedExtrusion::StretchedExtrusion() {
	// Nothing to see here :)
	color = StretchedColor::DARK_PURPLE;
	color.alpha = 1.0;
}

StretchedExtrusion::~StretchedExtrusion() {
	// Nothing to see here
}

void StretchedExtrusion::addPoint(P3D pt) {
	points.push_back(pt);
}

void StretchedExtrusion::clearPoints() {
	points.clear();
}

void StretchedExtrusion::draw() {
	// Draw the points
	glColor4d(color.red, color.green, color.blue, color.alpha);
	glPointSize(POINT_SIZE);
	glLineWidth(LINE_WIDTH);
	int ptsCount = points.size();
	if (DRAW_EXTRUSION_POINTS) {
		glBegin(GL_POINTS);
		for (int i = 0; i < ptsCount; i++) {
			P3D pt = points[i];
			glVertex3d(pt[0], pt[1], pt[2]);
			if (i < ptsCount - 1) {
				pt = points[i + 1];
				glVertex3d(pt[0], pt[1], pt[2]);
			}
		}
		glEnd();
	}
	// Draw the lines 
	glBegin(GL_LINES);
	for (int i = 0; i < ptsCount; i++) {
		P3D pt = points[i];
		glVertex3d(pt[0], pt[1], pt[2]);
		if (i < ptsCount - 1) {
			pt = points[i + 1];
			glVertex3d(pt[0], pt[1], pt[2]);
		}
	}
	glEnd();
	glPointSize(1);
	glLineWidth(1);
}
