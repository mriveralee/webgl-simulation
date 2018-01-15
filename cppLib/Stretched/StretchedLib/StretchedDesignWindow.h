#pragma once

#include <RobotDesignerLib/RobotDesign.h>
#include <string>
#include <map>
#include <algorithm>
#include <GUILib/TranslateWidget.h>
#include <GUILib/RotateWidgetV1.h>
#include <GUILib/RotateWidgetV2.h>
#include <GUILib/GLWindow3D.h>

#include "StretchedFlatSurface.h"
#include "StretchedExtrusionMaker.h"
#include "DelaunayTriangulator.h"
#include "DelaunayTriangulation.h"

/**
 *  StretchedDesignWindow
 */

enum StretchedDesignWindowMode {
	VIEW,
	EDIT
};

class StretchedDesignWindow : public GLWindow3D {
private:

	GLApplication* glApp;

	// Control Vars for displaying things in the design window
	bool showTranslateWidget = false, showRotateWidget = false;
	bool shouldPrintMouseLocation = false;
	bool showDesignEnvironment = true;
	bool showDelaunayTriangulaton = true;
	bool showExtrusions = true;

	// Fabric surface
	StretchedFlatSurface *fabricSurface = NULL;
	P3D lastMovedFabricSurfacePoint; // Hack to get clicks to work on the fabric surface

	// Extrusion Maker and finished extrusions
	StretchedExtrusionMaker *extrusionMaker = NULL;
	std::vector<StretchedExtrusion *> extrusions;
	
	// For Triangulation
	DelaunayTriangulator *triangulator = NULL;
	DelaunayTriangulation triangulation;
	
	StretchedDesignWindowMode windowMode = StretchedDesignWindowMode::EDIT;

	StretchedExtrusionMakerObjectType makerObjectType = StretchedExtrusionMakerObjectType::CIRCLE;

	P3D screenCoordsToFlatSurfacePoint(double xPos, double yPos, StretchedFlatSurface* flatSurface);

public:

	StretchedDesignWindow(int x, int y, int w, int h, GLApplication* glApp);
	virtual ~StretchedDesignWindow(void);

	// Draw the AppRobotDesigner scene - camera transformations, lighting, shadows, reflections, etc AppRobotDesignerly to everything drawn by this method
	virtual void drawScene();
	// This is the wild west of drawing - things that want to ignore depth buffer, camera transformations, etc. Not pretty, quite hacky, but flexible. Individual apps should be careful with implementing this method. It always gets called right at the end of the draw function
	virtual void drawAuxiliarySceneInfo();

	// Resets this window to defaults
	virtual void reset();

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

	void setupLights();

	// Returns the mesh for the simulation view that can be used to build the particle system
	virtual GLMesh* getTriangulatedFabricAreaMesh();
	// Returns the triangulation mesh
	virtual DelaunayTriangulation StretchedDesignWindow::getDelaunayTriangulation();



	// TODO: allow saving and loading of design mesh and extrusions etc
	virtual void saveFile(const char* fName);
	virtual void loadFile(const char* fName);

};



