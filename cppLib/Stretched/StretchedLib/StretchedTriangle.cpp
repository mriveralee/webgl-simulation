#include "StretchedTriangle.h"

#include "GUILib/GLUtils.h"


#include "MathLib/V3D.h"

#include "StretchedColor.h"

static const double STRETCHED_TRIANGLE_LINE_WIDTH = 5.0;
static const StretchedColor STRETCHED_TRIANGLE_LINE_COLOR = StretchedColor::MAGENTA;

StretchedTriangle::StretchedTriangle(P3D pt1, P3D pt2, P3D pt3) {
	p1 = pt1;
	p2 = pt2;
	p3 = pt3;
}

StretchedTriangle::~StretchedTriangle() {
	// Nothing to see here
}

double StretchedTriangle::calculateArea(P3D p1, P3D p2, P3D p3) {
	V3D ab = p2 - p1;
	double abLength = ab.length();
	V3D ac = p3 - p1;
	double acLength = ac.length();
	if (abLength == 0 || acLength == 0) {
		Logger::consolePrint("Error: Length of sides in Triangle cannot zero!");
		return 0.0;
	}
	double dotprod = ab.dot(ac);
	double combinedLength = (abLength * acLength);
	double theta = acos(dotprod / combinedLength);
	return sin(theta) * combinedLength/2;
}

void StretchedTriangle::draw() {
	// Draw out triangles, counterclockwise
	glLineWidth(STRETCHED_TRIANGLE_LINE_WIDTH);
	glColor4d(
		STRETCHED_TRIANGLE_LINE_COLOR.red, 
		STRETCHED_TRIANGLE_LINE_COLOR.green, 
		STRETCHED_TRIANGLE_LINE_COLOR.blue, 
		STRETCHED_TRIANGLE_LINE_COLOR.alpha);
	glBegin(GL_LINES);
	// Edge AB
	glVertex3d(p1[0], p1[1], p1[2]);
	glVertex3d(p2[0], p2[1], p2[2]);
	// Edge BC
	glVertex3d(p2[0], p2[1], p2[2]);
	glVertex3d(p3[0], p3[1], p3[2]);
	// Edge CA
	glVertex3d(p3[0], p3[1], p3[2]);
	glVertex3d(p1[0], p1[1], p1[2]);
	glEnd();
	glLineWidth(1);

}
