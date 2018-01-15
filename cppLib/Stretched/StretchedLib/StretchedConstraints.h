#pragma once

// Hold indices of pts for a bend constraint
struct StretchedBendConstraint {
	int a;
	int b;
	int c;
	double theta;
	double k;
};


// Holds indices of pts for a triangle area constraint
struct StretchedTriangleAreaConstraint {
	int a;
	int b;
	int c;
	double k;
	double area; // Compute using StretchedTriangle::computeArea()
};