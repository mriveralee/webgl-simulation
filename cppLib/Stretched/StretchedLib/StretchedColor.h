#pragma once

class StretchedColor {
public:
	StretchedColor();
	StretchedColor(double r, double g, double b);
	StretchedColor(double r, double g, double b, double a);
	StretchedColor(const StretchedColor &other);

	double red;
	double green;
	double blue;
	double alpha;

	static const StretchedColor RED;
	static const StretchedColor BLUE;
	static const StretchedColor GREEN;
	static const StretchedColor YELLOW;
	static const StretchedColor CYAN;
	static const StretchedColor MAGENTA;
	static const StretchedColor ORANGE;
	static const StretchedColor DARK_PURPLE;
	static const StretchedColor GRAY;
};