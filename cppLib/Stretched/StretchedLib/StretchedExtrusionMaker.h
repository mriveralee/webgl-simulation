#pragma once

#include "StretchedExtrusionBezierCurve.h"
#include "StretchedExtrusionCircle.h"
#include "StretchedExtrusion.h"
#include "StretchedColor.h"

static const int STRETCHED_EXTRUSION_NONE_ID = -9999;
static const int STRETCHED_EXTRUSION_CENTER_PT_ID = -3;
static const int STRETCHED_EXTRUSION_START_PT_ID = -2;
static const int STRETCHED_EXTRUSION_END_PT_ID = -1;

enum StretchedExtrusionMakerState {
	NONE,
	EDITING,
	COMPLETED,
};

enum StretchedExtrusionMakerObjectType {
	BEZIER_CURVE,
	CIRCLE
};

class StretchedExtrusionMaker {

public:
	StretchedExtrusionBezierCurve extrusionCurve;
	StretchedExtrusionCircle extrusionCircle;

	StretchedExtrusionMaker();
	~StretchedExtrusionMaker();

	StretchedExtrusion* createExtrusion();
	void makeCurve(P3D startPt, StretchedExtrusionMakerObjectType objectType);
	void clearCurve();
	void draw();

	// Controls the output of the segmented extrusion must be greater than 0
	void setExtrusionSegmentResolution(int resolution);
	void setExtrusionCircleRadius(double circleRadius);
	double getExtrusionCircleRadius();

	StretchedExtrusionMakerState getState();
	void setState(StretchedExtrusionMakerState newState);
	
	// Returns the id of the currently selected point (returns STRETCHED_EXTRUSION_NONE_ID) if none is selected
	int getSelectedPointId();
	
	// Updates the selected point id by finding the idea of the point closest (or none if no point is in range)
	void updateSelectedPointIdFromPoint(P3D point);

	// We use this to update the position of the currently selected point
	void updatedSelectedPointPosition(P3D point);

	virtual bool onMouseButtonEvent(int button, int action, int mods, double xPos, double yPos);
	virtual bool onMouseMoveEvent(double xPos, double yPos);
	
private:
	StretchedExtrusionMakerState state;
	StretchedExtrusionMakerObjectType makerObjectType;

	StretchedColor selectedPtColor;
	int extrusionSegmentResolution = DEFAULT_EXTRUSION_BEZIER_CURVE_SEGMENTS;
	int selectedPtId = STRETCHED_EXTRUSION_NONE_ID;

	//void setState(StretchedExtrusionMakerState newState);
	void setSelectedPointId(int selectedId);
	void drawSelectedPoint();

};

