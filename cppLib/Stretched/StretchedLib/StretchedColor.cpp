#include "StretchedColor.h"

#include "MathLib/MathLib.h"

const StretchedColor StretchedColor::RED = StretchedColor(1, 0, 0);
const StretchedColor StretchedColor::BLUE = StretchedColor(0, 0, 1);
const StretchedColor StretchedColor::GREEN = StretchedColor(0, 1, 0);
const StretchedColor StretchedColor::CYAN = StretchedColor(0, 1, 1);
const StretchedColor StretchedColor::YELLOW = StretchedColor(1, 1, 0);
const StretchedColor StretchedColor::MAGENTA = StretchedColor(1, 0, 1);
const StretchedColor StretchedColor::ORANGE = StretchedColor(1, 0.5, 0);
const StretchedColor StretchedColor::DARK_PURPLE = StretchedColor(0.5f, 0.0f, 1.0f);
const StretchedColor StretchedColor::GRAY = StretchedColor(0.6, 0.6, 0.6);

StretchedColor::StretchedColor() : StretchedColor(0, 0, 0) {
	// Nothing to see here
}

StretchedColor::StretchedColor(double r, double g, double b) : StretchedColor(r, g, b, 1) {
	// Nothing to see here
}

StretchedColor::StretchedColor(double r, double g, double b, double a) {
	red = MIN(MAX(0, r), 1);
	green = MIN(MAX(0, g), 1);
	blue = MIN(MAX(0, b), 1);
	alpha = MIN(MAX(0, a), 1);
}

StretchedColor::StretchedColor(const StretchedColor & other) {
	red = other.red;
	green = other.green;
	blue = other.blue;
	alpha = other.alpha;
}
