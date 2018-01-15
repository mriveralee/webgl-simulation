#include "StretchedExtrusionCircle.h"

#include "GUILib/GLUtils.h"
#include "StretchedConstants.h"


static const GLfloat EXTRUSION_CIRCLE_POINT_SIZE = 160;
static const GLfloat EXTRUSION_CIRCLE_LINE_SIZE = 10.0;

double StretchedExtrusionCircle::CIRCLE_RADIUS = DEFAULT_EXTRUSION_CIRCLE_RADIUS;

StretchedExtrusionCircle::StretchedExtrusionCircle() : StretchedExtrusionCircle(P3D(0, 0, 0), DEFAULT_EXTRUSION_CIRCLE_RADIUS) {
	// Nothing to see here
}

StretchedExtrusionCircle::StretchedExtrusionCircle(P3D centerPt) : StretchedExtrusionCircle(centerPt, DEFAULT_EXTRUSION_CIRCLE_RADIUS) {
	// Nothing to see here 
}

StretchedExtrusionCircle::StretchedExtrusionCircle(P3D centerPt, double radius) {
	this->centerPt = centerPt;
	this->radius = radius;
	init();
}

StretchedExtrusionCircle::~StretchedExtrusionCircle() {
	// Nothing to see here
}

double StretchedExtrusionCircle::getRadius() {
	return radius;
}

void StretchedExtrusionCircle::setRadius(double r) {
	radius = r;
}

void StretchedExtrusionCircle::draw() {

	// Draw the bounding box lines + lines
	P3D b0, b1, b2, b3;
	b0 = P3D(centerPt[0] - radius, centerPt[1], centerPt[2] + radius);
	b1 = P3D(centerPt[0] + radius, centerPt[1], centerPt[2] + radius);
	b2 = P3D(centerPt[0] + radius, centerPt[1], centerPt[2] - radius);
	b3 = P3D(centerPt[0] - radius, centerPt[1], centerPt[2] - radius);

	// Draw the control points
	glPointSize(EXTRUSION_CIRCLE_POINT_SIZE);
	glColor4d(controlPtColor.red, controlPtColor.green, controlPtColor.blue, controlPtColor.alpha);
	glBegin(GL_POINTS);
	// TL
	glVertex3d(b0[0], b0[1], b0[2]);
	// TR
	glVertex3d(b1[0], b1[1], b1[2]);
	// BL
	glVertex3d(b2[0], b2[1], b2[2]);
	// BR
	glVertex3d(b3[0], b3[1], b3[2]);
	glEnd();

	glColor4d(lineColor.red, lineColor.green, lineColor.blue, lineColor.alpha);
	glLineWidth(EXTRUSION_CIRCLE_LINE_SIZE / 2);
	glBegin(GL_LINES);
	// T
	glVertex3d(b0[0], b0[1], b0[2]);
	glVertex3d(b1[0], b1[1], b1[2]);
	// RS
	glVertex3d(b1[0], b1[1], b1[2]);
	glVertex3d(b2[0], b2[1], b2[2]);
	// B
	glVertex3d(b2[0], b2[1], b2[2]);
	glVertex3d(b3[0], b3[1], b3[2]);
	// LS
	glVertex3d(b3[0], b3[1], b3[2]);
	glVertex3d(b0[0], b0[1], b0[2]);
	glEnd();

	// Draw the Center Point for moving
	glPointSize(EXTRUSION_CIRCLE_POINT_SIZE);
	glColor4d(controlPtColor.red, controlPtColor.green, controlPtColor.blue, controlPtColor.alpha);
	glBegin(GL_POINTS);
	glVertex3d(centerPt[0], centerPt[1], centerPt[2]);
	glEnd();

	// Draw the Circle curve
	glColor4d(curveColor.red, curveColor.green, lineColor.blue, lineColor.alpha);
	glLineWidth(EXTRUSION_CIRCLE_LINE_SIZE);
	glBegin(GL_LINE_LOOP);
	for (int ii = 0; ii < numSegments; ii++) {
		double theta = 2.0f * PI * double(ii) / double(numSegments); //get the current angle 
		double x = radius * cos(theta);//calculate the x component 
		double z = radius * sin(theta);//calculate the z component 
		glVertex3d(x + centerPt[0], centerPt[1], z + centerPt[2]);//output vertex 
	}
	glEnd();

	glLineWidth(DEFAULT_GL_LINE_WIDTH);
	glPointSize(DEFAULT_GL_POINT_SIZE);
}

std::vector<P3D> StretchedExtrusionCircle::pointsOnCurve() {
	std::vector<P3D> pts = std::vector<P3D>();
	if (numSegments <= 0) {
		return pts;
	}
	for (int ii = 0; ii < numSegments; ii++) {
		double theta = 2.0f * PI * double(ii) / numSegments;//get the current angle 
		double x = radius * cos(theta);//calculate the x component 
		double z = radius * sin(theta);//calculate the z component 
		P3D p = P3D(x + centerPt[0], centerPt[1], z + centerPt[2]);
		pts.push_back(p);
	}
	if ((int)pts.size() > 1) {
		pts.push_back(P3D(pts[0]));
	}
	return pts;
}

std::vector<P3D> StretchedExtrusionCircle::pointsForTriangulation() {
	std::vector<P3D> pts = pointsOnCurve();

		// Add inside points
	for (int ii = 0; ii < 30; ii++) {
		double m = 0.3 + 0.5 * ii/30.0;
		double r = m * radius;
		double theta = 2.0f * PI * double(ii) / numSegments;//get the current angle 
		double x = r * cos(theta);//calculate the x component 
		double z = r * sin(theta);//calculate the z component 
		P3D p = P3D(x + centerPt[0], centerPt[1], z + centerPt[2]);
		pts.push_back(p);
	}

	return pts;
}

void StretchedExtrusionCircle::updateCenterPoint(P3D point) {
	centerPt = point;
}

P3D StretchedExtrusionCircle::getCenterPoint() {
	return centerPt;
}

void StretchedExtrusionCircle::init() {
	numSegments = DEFAULT_EXTRUSION_CIRCLE_SEGMENTS;
	controlPtColor = CONTROL_POINTS_COLOR;
	lineColor = StretchedColor::GRAY;
	curveColor = StretchedColor::YELLOW;
}
