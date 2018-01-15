#pragma once

#include <RobotDesignerLib/RobotDesign.h>
#include <string>
#include <map>
#include <algorithm>
#include <GUILib/TranslateWidget.h>
#include <GUILib/RotateWidgetV1.h>
#include <GUILib/RotateWidgetV2.h>
#include <GUILib/GLWindow2D.h>

#include "StretchedFlatSurface.h"

/**
 * Stretched Design Window 2D
 */
class StretchedDesignWindow2D : public GLWindow2D {
private:
	GLApplication* glApp;
	bool showDesignEnvironment = true;

	StretchedFlatSurface *fabricSurface;
	GLConsole *testConsole;
	P3D lastMovedFabricSurfacePoint; // Hack to get clicks to work on the fabric surface

	P3D screenCoordsToFlatSurfacePoint(double xPos, double yPos, StretchedFlatSurface* flatSurface);


public:

	RobotDesign* robot;
	// constructor
	StretchedDesignWindow2D(int x, int y, int w, int h, GLApplication* glApp);
	// destructor
	virtual ~StretchedDesignWindow2D(void);
	virtual void draw();

	//mouse & keyboard event callbacks...

	//all these methods should returns true if the event is processed, false otherwise...
	//any time a physical key is pressed, this event will trigger. Useful for reading off special keys...
	virtual bool onKeyEvent(int key, int action, int mods);
	//this one gets triggered on UNICODE characters only...
	virtual bool onCharacterPressedEvent(int key, int mods);
	//triggered when mouse buttons are pressed
	virtual bool onMouseButtonEvent(int button, int action, int mods, double xPos, double yPos);
	//triggered when mouse moves
	virtual bool onMouseMoveEvent(double xPos, double yPos);
	//triggered when using the mouse wheel
	virtual bool onMouseWheelScrollEvent(double xOffset, double yOffset);

	void onMenuMouseButtonProcessedEvent();

	virtual void saveFile(const char* fName);
	virtual void loadFile(const char* fName);
};



