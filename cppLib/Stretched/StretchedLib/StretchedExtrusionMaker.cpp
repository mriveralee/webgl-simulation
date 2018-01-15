#include "StretchedExtrusionMaker.h"
#include "GUILib/GLUtils.h"


static const double MIN_CLICK_PROXIMITY_DIST = 0.02;
static const GLfloat SELECTED_POINT_SIZE = 35;


StretchedExtrusionMaker::StretchedExtrusionMaker() {
	selectedPtColor = StretchedColor::YELLOW;
}

StretchedExtrusionMaker::~StretchedExtrusionMaker() {
	// Nothing to see here
}

StretchedExtrusion* StretchedExtrusionMaker::createExtrusion() {
	StretchedExtrusion *extrusion = new StretchedExtrusion();
	std::vector<P3D> curvePts = std::vector<P3D>();
	switch (makerObjectType) {
	case BEZIER_CURVE:
		curvePts = extrusionCurve.pointsOnCurveForSegments(extrusionSegmentResolution);
		break;
	case CIRCLE:
		curvePts = extrusionCircle.pointsForTriangulation();
		break;
	default:
		break;
	}
	
	for (int i = 0; i < (int) curvePts.size(); i++) {
		extrusion->addPoint(curvePts[i]);
	}
	return extrusion;
}

void StretchedExtrusionMaker::makeCurve(P3D startPt, StretchedExtrusionMakerObjectType objectType) {
	if (objectType == CIRCLE) {
		extrusionCircle = StretchedExtrusionCircle(startPt, StretchedExtrusionCircle::CIRCLE_RADIUS);
	}
	else if (objectType == BEZIER_CURVE) {
		extrusionCurve = StretchedExtrusionBezierCurve(startPt);
	}
	makerObjectType = objectType;
}

void StretchedExtrusionMaker::clearCurve() {
	extrusionCurve = StretchedExtrusionBezierCurve();
	extrusionCircle = StretchedExtrusionCircle();
	setSelectedPointId(STRETCHED_EXTRUSION_NONE_ID);
}

void StretchedExtrusionMaker::setExtrusionSegmentResolution(int resolution) {
	if (resolution <= 0) {
		resolution = DEFAULT_EXTRUSION_BEZIER_CURVE_SEGMENTS;
	}
	extrusionSegmentResolution = resolution;
}

void StretchedExtrusionMaker::setExtrusionCircleRadius(double circleRadius) {
	StretchedExtrusionCircle::CIRCLE_RADIUS = circleRadius;
	extrusionCircle.setRadius(circleRadius);
}

double StretchedExtrusionMaker::getExtrusionCircleRadius()
{
	return StretchedExtrusionCircle::CIRCLE_RADIUS;
}

StretchedExtrusionMakerState StretchedExtrusionMaker::getState() {
	return state;
}

int StretchedExtrusionMaker::getSelectedPointId() {
	return selectedPtId;
}

void StretchedExtrusionMaker::setSelectedPointId(int selectedId) {
	if (selectedId < STRETCHED_EXTRUSION_NONE_ID
		|| selectedId >= extrusionCurve.getControlPointCount()) {
		selectedId = STRETCHED_EXTRUSION_NONE_ID;
	}
	selectedPtId = selectedId;
};

void StretchedExtrusionMaker::updateSelectedPointIdFromPoint(P3D point) {
	std::vector<P3D> pts;
	double minDist = 10000000000;
	int nextId = STRETCHED_EXTRUSION_NONE_ID;

	if (makerObjectType == BEZIER_CURVE) {
		// Check start point
		P3D p = extrusionCurve.getStart();
		double dist = (point - p).length();
		if (dist < MIN_CLICK_PROXIMITY_DIST && dist < minDist) {
			nextId = STRETCHED_EXTRUSION_START_PT_ID;
			minDist = dist;
		}

		// Check End Point
		p = extrusionCurve.getEnd();
		dist = (point - p).length();
		if (dist < MIN_CLICK_PROXIMITY_DIST && dist < minDist) {
			nextId = STRETCHED_EXTRUSION_END_PT_ID;
			minDist = dist;
		}
		// Check Control Points
		for (int i = 0; i < extrusionCurve.getControlPointCount(); i++) {
			p = extrusionCurve.getControlPoint(i);
			dist = (point - p).length();
			if (dist < MIN_CLICK_PROXIMITY_DIST && dist < minDist) {
				nextId = i;
				minDist = dist;
			}
		}
	}
	else if (makerObjectType == CIRCLE) {
		// Check center point
		P3D p = extrusionCircle.getCenterPoint();
		double dist = (point - p).length();
		if (dist < MIN_CLICK_PROXIMITY_DIST && dist < minDist) {
			nextId = STRETCHED_EXTRUSION_CENTER_PT_ID;
			minDist = dist;
		}
	}
	selectedPtId = nextId;
}

void StretchedExtrusionMaker::updatedSelectedPointPosition(P3D point) {
	if (selectedPtId == STRETCHED_EXTRUSION_NONE_ID 
		|| selectedPtId >= extrusionCurve.getControlPointCount()) {
		return;
	}
	if (selectedPtId == STRETCHED_EXTRUSION_START_PT_ID) {
		extrusionCurve.setStart(point);
	} else if (selectedPtId == STRETCHED_EXTRUSION_END_PT_ID) {
		extrusionCurve.setEnd(point);
	} else if (selectedPtId == STRETCHED_EXTRUSION_CENTER_PT_ID) {
		if (makerObjectType == CIRCLE) {
			extrusionCircle.updateCenterPoint(point);
		} else {
			// Do nothing
		}	
	} else {
		extrusionCurve.setControlPoint(point, selectedPtId);
	}
}

bool StretchedExtrusionMaker::onMouseButtonEvent(int button, int action, int mods, double xPos, double yPos)
{
	return false;
}

bool StretchedExtrusionMaker::onMouseMoveEvent(double xPos, double yPos)
{
	return false;
}

void StretchedExtrusionMaker::setState(StretchedExtrusionMakerState newState) {
	state = newState;
}

void StretchedExtrusionMaker::draw() {
	switch (state) {
		case COMPLETED:
			return;
		case NONE:
			// Not editing, nothing selected etc
			// draw nothing
			return;
		case EDITING:
			if (makerObjectType == CIRCLE) {
				extrusionCircle.draw();
			}
			else if (makerObjectType == BEZIER_CURVE) {
				extrusionCurve.draw();
			}
			drawSelectedPoint();
	}
}

void StretchedExtrusionMaker::drawSelectedPoint() {
	// Nothing to draw if nothing is selected
	if (selectedPtId <= STRETCHED_EXTRUSION_NONE_ID 
		|| selectedPtId >= extrusionCurve.getControlPointCount()) {
		return;
	}
	P3D selectedPt;
	// We have to draw either a selected point, or let points be movable etc
	if (selectedPtId == STRETCHED_EXTRUSION_START_PT_ID) {
		selectedPt = extrusionCurve.getStart();
	} else if (selectedPtId == STRETCHED_EXTRUSION_END_PT_ID) {
		selectedPt = extrusionCurve.getEnd();
	} else if (selectedPtId == STRETCHED_EXTRUSION_CENTER_PT_ID) {
		selectedPt = extrusionCircle.getCenterPoint();
	} else {
		// Must be a control point
		selectedPt = extrusionCurve.getControlPoint(selectedPtId);
	}
	// Draw the point
	// Draw the start, end and control points;
	glPointSize((GLfloat)SELECTED_POINT_SIZE);
	glColor4d(selectedPtColor.red, selectedPtColor.green, selectedPtColor.blue, selectedPtColor.alpha);
	glBegin(GL_POINTS);
	glVertex3d(selectedPt[0], selectedPt[1], selectedPt[2]);
	glEnd();
	glPointSize(1);
}
