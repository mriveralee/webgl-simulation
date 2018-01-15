#include "StretchedExtrusionBezierCurve.h"

#include "GUILib/GLUtils.h"
#include "StretchedConstants.h"

static const GLfloat POINT_SIZE = 160;
static const GLfloat OUTLINE_LINE_WIDTH = 5;
static const GLfloat NORMAL_LINE_WIDTH = 10;

StretchedExtrusionBezierCurve::StretchedExtrusionBezierCurve(P3D startPt) : BezierCurve(startPt) {
	init();
}

StretchedExtrusionBezierCurve::StretchedExtrusionBezierCurve() : BezierCurve() {
	init();
}

StretchedExtrusionBezierCurve::~StretchedExtrusionBezierCurve() {
	// Nothing to see here
}

void StretchedExtrusionBezierCurve::init() {
	lineColor = StretchedColor(0.2, 0.2, 0.2, 0.3);
	controlPtColor = CONTROL_POINTS_COLOR;
	curveColor = StretchedColor::YELLOW;
}

void StretchedExtrusionBezierCurve::draw() {
	// TODO: optimize curve computation by storing the completed curve points for segments unless the curve has changed
	P3D start = getStart();
	P3D end = getEnd();
	int controlPtCount = getControlPointCount();
	P3D sumPt = P3D() + start + end;
	// Draw the start, end and control points;
	glPointSize(POINT_SIZE);
	glColor4d(controlPtColor.red, controlPtColor.green, controlPtColor.blue, controlPtColor.alpha);
	glBegin(GL_POINTS);
	glVertex3d(start[0], start[1], start[2]);
	for (int i = 0; i < controlPtCount; i++) {
		// Draw the Control Points
		P3D pt = getControlPoint(i);
		glVertex3d(pt[0], pt[1], pt[2]);
		sumPt += pt;
	}
	glVertex3d(end[0], end[1], end[2]);
	glEnd();

	// Draw the Lines connecting points
	glLineWidth(OUTLINE_LINE_WIDTH);
	glColor4d(lineColor.red, lineColor.green, lineColor.blue, lineColor.alpha);
	glBegin(GL_LINES);
	P3D pt = getControlPoint(0);
	glVertex3d(start[0], start[1], start[2]);
	glVertex3d(pt[0], pt[1], pt[2]);
	for (int i = 0; i < controlPtCount; i++) {
		// Draw the Control Points
		pt = getControlPoint(i);
		glVertex3d(pt[0], pt[1], pt[2]);
	}
	glVertex3d(pt[0], pt[1], pt[2]);
	glVertex3d(end[0], end[1], end[2]);
	glEnd();

	// Draw the Center Point for moving
	glPointSize(POINT_SIZE);
	glColor4d(controlPtColor.red, controlPtColor.green, controlPtColor.blue, controlPtColor.alpha);
	P3D centroid = sumPt / (controlPtCount + 2);
	glBegin(GL_POINTS);
	glVertex3d(centroid[0], centroid[1], centroid[2]);
	glEnd();

	// Finally draw the bezier curve
	glLineWidth(NORMAL_LINE_WIDTH);
	std::vector<P3D> curvePts = pointsOnCurveForSegments(DEFAULT_EXTRUSION_BEZIER_CURVE_SEGMENTS);
	glColor4d(curveColor.red, curveColor.green, curveColor.blue, curveColor.alpha);
	glBegin(GL_LINES);
	int curvePtsCount = curvePts.size();
	for (int i = 0; i < curvePtsCount; i++) {
		// Draw the Control Points
		P3D pt1 = curvePts[i];
		glVertex3d(pt1[0], pt1[1], pt1[2]);
		if (i < curvePtsCount - 1) {
			P3D pt2 = curvePts[i + 1];
			glVertex3d(pt2[0], pt2[1], pt2[2]);
		}
	}
	glEnd();

	// Return the point size to normal
	glPointSize(1);
	glLineWidth(1);
}
