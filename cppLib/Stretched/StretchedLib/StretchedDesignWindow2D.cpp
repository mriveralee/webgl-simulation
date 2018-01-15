#include "StretchedDesignWindow2D.h"
#include <GUILib/GLUtils.h>
#include <GUILib/GLMesh.h>
#include <GUILib/GLContentManager.h>
#include <MathLib/MathLib.h>
#include <MathLib/Matrix.h>

#include <GUILib/PlotWindow.h>

StretchedDesignWindow2D::StretchedDesignWindow2D(int x, int y, int w, int h, GLApplication* glApp) : GLWindow2D(x, y, w, h){
	this->glApp = glApp;
	testConsole = new GLConsole(getViewportXFromRelativeX(0), getViewportYFromRelativeY(60), w/2, w/2);
	
	// Background color of the window:
	bgColorR = 1;
	bgColorG = 0.2;
	bgColorB = 0.5;
	bgColorA = 0.2;
	
	fabricSurface = new StretchedFlatSurface(1.0, P3D(0, 0, 0));
	fabricSurface->setColor(P3D(1, 0, 0));

	TwAddVarRW(glApp->mainMenuBar, "Show Design Environment", TW_TYPE_BOOLCPP, &showDesignEnvironment, "");
}

StretchedDesignWindow2D::~StretchedDesignWindow2D(void) {
}


P3D StretchedDesignWindow2D::screenCoordsToFlatSurfacePoint(double xPos, double yPos, StretchedFlatSurface* flatSurface) {
	Ray clickedRay = getRayFromScreenCoords(xPos, yPos);
	P3D clickedPoint = flatSurface->pointClosestToPlane(clickedRay);
	clickedPoint = flatSurface->transformClickedPointToSurface(clickedPoint);
	Logger::consolePrint("transformedPoint: %f %f %f", clickedPoint[0], clickedPoint[1], clickedPoint[2]);
	return clickedPoint;
}


//triggered when mouse moves
bool StretchedDesignWindow2D::onMouseMoveEvent(double xPos, double yPos) {
	preDraw();
	
	bool processed = false;

	P3D clickedFabricSurfacePt = screenCoordsToFlatSurfacePoint(xPos, yPos, fabricSurface); 
	if (fabricSurface->isWithinSurface(clickedFabricSurfacePt)) {
		// Hack to get the clicked point to work - it seems the point returned from our plane check is wrong when coming
		// from a mouse move event to a click (it always returns (0, 0, 4) for clicks. We use the last moved surface point instead
		lastMovedFabricSurfacePoint = clickedFabricSurfacePt;
		// TODO: Handle case for when we are making curves with button pressed
		Logger::consolePrint("Moved in the surface");
		Logger::consolePrint("----");
		processed = true;
	}
	postDraw();

	if (processed) {
		return true;
	}
	return true;
	// Below enabled camera movement - since we want 2D drawing on the plane we restring the design view to being only zoom capabale.
	// We may have to handle the case of drawing curves above and return appropriately
	return GLWindow2D::onMouseMoveEvent(xPos, yPos);
}


void StretchedDesignWindow2D::onMenuMouseButtonProcessedEvent() {
	// Nothing to see here
}


//triggered when mouse buttons are pressed
bool StretchedDesignWindow2D::onMouseButtonEvent(int button, int action, int mods, double xPos, double yPos) {

	// Check if our click intersects with our fabric plane
	P3D clickedFabricSurfacePt = screenCoordsToFlatSurfacePoint(xPos, yPos, fabricSurface);
	// See hack above- the point returned from the Ray-Plane closest is always (0, 0, 4), so we use the lastmovedsurfacepoint
	clickedFabricSurfacePt = lastMovedFabricSurfacePoint;
	if (fabricSurface->isWithinSurface(clickedFabricSurfacePt)) {
		Logger::consolePrint("Clicked on the surface");
		return true;
	}

	// TODO fix this bit
	return true;
	//return  GLWindow2D::onMouseButtonEvent(button, action, mods, xPos, yPos);
}

//triggered when using the mouse wheel
bool StretchedDesignWindow2D::onMouseWheelScrollEvent(double xOffset, double yOffset) {
	if (GLWindow2D::onMouseWheelScrollEvent(xOffset, yOffset)) return true;
	// TODO (mrivera) handle scaling
	return true;
}

bool StretchedDesignWindow2D::onKeyEvent(int key, int actionI, int mods) {
	if (key == 't' || key == 'T' && actionI == GLFW_PRESS) {
		/*showTranslateWidget = !showTranslateWidget;
		if (showTranslateWidget)
			showRotateWidget = false;*/
	}
	return GLWindow2D::onKeyEvent(key, actionI, mods);
}

bool StretchedDesignWindow2D::onCharacterPressedEvent(int key, int mods) {
	if (GLWindow2D::onCharacterPressedEvent(key, mods)) return true;
	return false;
}

void StretchedDesignWindow2D::loadFile(const char* fName) {
	Logger::consolePrint("Loading file \'%s\'...\n", fName);
	std::string fileName;
	fileName.assign(fName);
	std::string fNameExt = fileName.substr(fileName.find_last_of('.') + 1);
}

void StretchedDesignWindow2D::saveFile(const char* fName) {
	//robot->saveRobotToFile(fName);
}

void StretchedDesignWindow2D::draw() {
	int xAxTickHeight = 30;
	int yAxTickWidth = 60;
	int titleHeight = 30;
	int legendWidth = 80;
	preDraw();
	// plotBox
	Box2d plotBox(50, xAxTickHeight, viewportWidth - yAxTickWidth - legendWidth, viewportHeight - xAxTickHeight - titleHeight, viewportWidth, viewportHeight, maxX, maxY);
	glColor4d(1, 1, 1, .9);
	plotBox.fill();
	glColor4d(0, 0, 0, .5);
	plotBox.outline();

	glColor3d(1, 0, 1);

	glPointSize(15.0);
	glBegin(GL_POINTS);
	for (int i = 0; i < 600; i++) {
		plotBox.drawVertex(i, i/2 + 1);

	}
	glEnd();
	postDraw();
}
