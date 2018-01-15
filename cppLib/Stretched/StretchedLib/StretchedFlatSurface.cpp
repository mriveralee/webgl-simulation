#include "StretchedFlatSurface.h"

#include "GUILib/GLUtils.h"

#include "MathLib/Ray.h"
#include "Utils/Utils.h"

#include "StretchedColor.h"


static const double DEFAULT_SIZE = 10;

StretchedFlatSurface::StretchedFlatSurface(P3D centerPt) : StretchedFlatSurface(DEFAULT_SIZE, centerPt) { 
	// Nothing to see here
}

StretchedFlatSurface::StretchedFlatSurface(double size, P3D centerPt) : StretchedFlatSurface(size, size, centerPt) {
	// Nothing to see here
}

StretchedFlatSurface::StretchedFlatSurface(double length, double width, P3D centerPt) {
	this->length = length;
	this->width = width;
	center = centerPt;
	normal = Globals::worldUp;
	plane = Plane(centerPt, normal);
	color = StretchedColor::RED;
	color.alpha = 0.5;
	computeCorners();
}

StretchedFlatSurface::~StretchedFlatSurface() {
	// Nothing to see here
}

void StretchedFlatSurface::computeCorners() {
	double halfWidth = width / 2;
	double halfLength = length / 2;

	P3D c0, c1, c2, c3;

	if (normal.at(1) == 1) {
		c0 = P3D(center.at(0) - halfWidth, center.at(1), center.at(2) - halfLength);
		c1 = P3D(center.at(0) - halfWidth, center.at(1), center.at(2) + halfLength);
		c2 = P3D(center.at(0) + halfWidth, center.at(1), center.at(2) + halfLength);
		c3 = P3D(center.at(0) + halfWidth, center.at(1), center.at(2) - halfLength);
	} else if (normal.at(2) == 1) {
		c0 = P3D(center.at(0) - halfWidth, center.at(1) - halfLength, center.at(2));
		c1 = P3D(center.at(0) - halfWidth, center.at(1) + halfLength, center.at(2));
		c2 = P3D(center.at(0) + halfWidth, center.at(1) + halfLength, center.at(2));
		c3 = P3D(center.at(0) + halfWidth, center.at(1) - halfLength, center.at(2));
	} else {
		// Normal.at(0) == 1
		c0 = P3D(center.at(0), center.at(1) - halfWidth, center.at(2) - halfLength);
		c1 = P3D(center.at(0), center.at(1) - halfWidth, center.at(2) + halfLength);
		c2 = P3D(center.at(0), center.at(1) + halfWidth, center.at(2) + halfLength);
		c3 = P3D(center.at(0), center.at(1) + halfWidth, center.at(2) - halfLength);
	}

	corners.push_back(c3);
	corners.push_back(c2);
	corners.push_back(c1);
	corners.push_back(c0);
}

P3D StretchedFlatSurface::pointClosestToPlane(Ray ray) {
	V3D dir = ray.direction;
	double d = (center - P3D()).length();

	float nDotA = normal.dot(ray.origin);
	float nDotDir = normal.dot(ray.direction);
	V3D x = (((d - nDotA) / nDotDir) * dir);
	return ray.origin + x;
}

P3D StretchedFlatSurface::transformClickedPointToSurface(P3D clickedPoint) {
	// TODO (mrivera) handle other normals besides Y is up
	//clickedPoint[2] = clickedPoint[1]
	clickedPoint[1] = getCenter().at(1);
	return clickedPoint;
}

bool StretchedFlatSurface::isWithinSurface(P3D point) {
	if (corners.size() == 1) {
		return false;
	}
	if (normal.at(1) != 1 || normal.length() > 1) {
		Logger::consolePrint("Undefined normal");
		return false;
	}
	// Check that the height is correct for t
	if (!(center.at(1) - EPSILON <= point.at(1) && point.at(1) <= center.at(1) + EPSILON)) {
		return false;
	} 
	P3D c1, c2;
	double A, B, C, D;
	int cornersSize = corners.size();
	for (int i = 0; i < cornersSize && cornersSize > 1; i++) {
		c1 = corners[i];
		if (i < cornersSize - 1) {
			c2 = corners[i + 1];
		}
		else {
			c2 = corners[0];
		}
		//	A = -(y2 - y1)
		//	B = x2 - x1
		//	C = -(A * x1 + B * y1)
		//	Now all you need to do is to calculate
		//	D = A * xp + B * yp + C
		A = -1 * (c2.at(2) - c1.at(2));
		B = c2.at(0) - c1.at(0);
		C = -1 * (A * c1.at(0) + B * c1.at(2));
		D = A * point.at(0) + B * point.at(2) + C;
		if (D < 0) {
			return false;
		}
	}
	return true;		
	// TODO (mrivera): use these for calcula
	//if (normal.at(1) == 1 
	//	&& (center.at(1) - EPSILON <= point.at(1)  && point.at(1) <= center.at(1) + EPSILON)) {
	//	v1 = diff.at(0);
	//	v2 = diff.at(2);
	//	isValidNormal = true;
	//} else if (normal.at(2) == 1 
	//	&& (center.at(2) - EPSILON <= point.at(2)  && point.at(2) <= center.at(2) + EPSILON)) {
	//	v1 = diff.at(0);
	//	v2 = diff.at(1);
	//	isValidNormal = true;
	//} else if (normal.at(0) == 1
	//	&& (center.at(0) - EPSILON <= point.at(0)  && point.at(0) <= center.at(0) + EPSILON)) {
	//	v1 = diff.at(1);
	//	v2 = diff.at(2);
	//	isValidNormal = true;
	//}
	// Fail assert case
}

void StretchedFlatSurface::draw() {
	//glEnable(GL_LIGHTING);
	glColor4d(color.red , color.green, color.blue, color.alpha);
	drawBox(corners[0], corners[2]);
	//glDisable(GL_LIGHTING);
}

void StretchedFlatSurface::setColor(StretchedColor color) {
	this->color = color;
}


double StretchedFlatSurface::getWidth() { return width; };
double StretchedFlatSurface::getLength() { return length; };
double StretchedFlatSurface::getX() { return center.at(0); };
double StretchedFlatSurface::getY() { return center.at(1); };
double StretchedFlatSurface::getZ() { return center.at(2); };
P3D StretchedFlatSurface::getCenter() { return center; };
V3D StretchedFlatSurface::getNormal() { return normal; };
Plane StretchedFlatSurface::getPlane() { return plane; };
StretchedColor StretchedFlatSurface::getColor() { return color; }