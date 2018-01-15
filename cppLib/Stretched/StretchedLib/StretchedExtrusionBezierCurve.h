#pragma once

#include "MathLib/P3D.h"
#include "MathLib/BezierCurve.h"

#include "StretchedColor.h"

// This is used to show a bezier curve that is being edited in 
// the design window. Upon completion the curve is added to the extrusion curve
// Note that successive curves will be connected to the previous curve
// TODO (mrivera): support independent curves (probs need a std::vector of extrusion curves in the stretchedextrusion

const int DEFAULT_EXTRUSION_BEZIER_CURVE_SEGMENTS = 40;

class StretchedExtrusionBezierCurve : public BezierCurve {

public:
	StretchedColor controlPtColor;
	StretchedColor lineColor;
	StretchedColor curveColor;

	StretchedExtrusionBezierCurve(P3D startPt);
	StretchedExtrusionBezierCurve();
	~StretchedExtrusionBezierCurve();

	void draw();

private:

	void init();
};

