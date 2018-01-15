#include "StretchedDesignWindow.h"
#include <GUILib/GLUtils.h>
#include <GUILib/GLMesh.h>
#include <GUILib/GLContentManager.h>
#include <MathLib/MathLib.h>
#include <MathLib/Matrix.h>

#include "StretchedConstants.h"
#include "StretchedColor.h"
#include "StretchedExtrusionCircle.h"
#include "StretchedKeyPressUtil.h"

#include "DelaunayTriangulator.h"

void TW_CALL updateExtrusionMakerCircleRadius(const void *value, void *clientData) {
	double radius = *static_cast<const double *>(value);
	StretchedExtrusionMaker *maker = (StretchedExtrusionMaker *)clientData;
	maker->setExtrusionCircleRadius(radius);
}

void TW_CALL getExtrusionMakerCircleRadius(void *value, void *clientData) {
	StretchedExtrusionMaker *maker = (StretchedExtrusionMaker *)clientData;
	*static_cast<double *>(value) = maker->getExtrusionCircleRadius();
}

StretchedDesignWindow::StretchedDesignWindow(int x, int y, int w, int h, GLApplication* glApp) : GLWindow3D(x, y, w, h){
	this->glApp = glApp;
	
	// Design window params
	char* designVarGroup = " group = 'Design Options' ";
	TwAddVarRW(glApp->mainMenuBar, "Show Design Environment", TW_TYPE_BOOLCPP, &showDesignEnvironment, designVarGroup);
	TwAddVarRW(glApp->mainMenuBar, "Show Extrusions", TW_TYPE_BOOLCPP, &showExtrusions, designVarGroup);
	TwAddVarRW(glApp->mainMenuBar, "Show Delaunay Triangulation", TW_TYPE_BOOLCPP, &showDelaunayTriangulaton, designVarGroup);
	
	// Debugging window params
	char* debugVarGroup = "group = 'Debug Options'";
	TwAddVarRW(glApp->mainMenuBar, "Print Mouse Location", TW_TYPE_BOOLCPP, &shouldPrintMouseLocation, debugVarGroup);

	// Add enum modes to the window bar
	TwEnumVal designWindowModeEV[] = { { StretchedDesignWindowMode::VIEW, "VIEW" },{ StretchedDesignWindowMode::EDIT, "EDIT" } };
	TwType designWindowModeType = TwDefineEnum("StretchedDesignWindowMode", designWindowModeEV, 2);
	TwAddVarRW(glApp->mainMenuBar, "Design Mode", designWindowModeType, &windowMode, designVarGroup);

	// Add enum modes to the window bar
	TwEnumVal extrusionModeEV[] = { { StretchedExtrusionMakerObjectType::CIRCLE, "CIRCLE" },{ StretchedExtrusionMakerObjectType::BEZIER_CURVE, "BEZIER CURVE" } };
	TwType extrusionObjectType = TwDefineEnum("StretchedExtrusionMakerObjectType", extrusionModeEV, 2);
	TwAddVarRW(glApp->mainMenuBar, "Extrusion Object Type", extrusionObjectType, &makerObjectType, designVarGroup);

	// Size of Extrusion Circle Radius
	std::string params = string(designVarGroup) + string(" step=0.1 min=1 max=10 ");
	TwAddVarCB(glApp->mainMenuBar, "Extrusion Circle Radius", TW_TYPE_DOUBLE, updateExtrusionMakerCircleRadius, getExtrusionMakerCircleRadius, &this->extrusionMaker, params.c_str());


	// Set up the camera
	delete camera;
	camera = new GLTrackingCamera(
		STRETCHED_DESIGN_CAMERA_INITIAL_ZOOM_DISTANCE, 
		STRETCHED_DESIGN_CAMERA_INITIAL_VIEW_DIRECTION, 
		STRETCHED_DESIGN_CAMERA_INITIAL_UP_AXIS);
	camera->setCameraTarget(STRETCHED_DESIGN_CAMERA_INITIAL_TARGET_POSITION);

	reset();
}

StretchedDesignWindow::~StretchedDesignWindow(void) {
	delete extrusionMaker;
	delete fabricSurface;
}

void StretchedDesignWindow::setupLights() {
	GLfloat bright[] = { 0.8f, 0.8f, 0.8f, 1.0f };
	GLfloat mediumbright[] = { 0.3f, 0.3f, 0.3f, 1.0f };

	glLightfv(GL_LIGHT1, GL_DIFFUSE, bright);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, mediumbright);
	glLightfv(GL_LIGHT3, GL_DIFFUSE, mediumbright);
	glLightfv(GL_LIGHT4, GL_DIFFUSE, mediumbright);


	GLfloat light0_position[] = { 0.0f, 10000.0f, 10000.0f, 0.0f };
	GLfloat light0_direction[] = { 0.0f, -10000.0f, -10000.0f, 0.0f };

	GLfloat light1_position[] = { 0.0f, 10000.0f, -10000.0f, 0.0f };
	GLfloat light1_direction[] = { 0.0f, -10000.0f, 10000.0f, 0.0f };

	GLfloat light2_position[] = { 0.0f, -10000.0f, 0.0f, 0.0f };
	GLfloat light2_direction[] = { 0.0f, 10000.0f, -0.0f, 0.0f };

	GLfloat light3_position[] = { 10000.0f, -10000.0f, 0.0f, 0.0f };
	GLfloat light3_direction[] = { -10000.0f, 10000.0f, -0.0f, 0.0f };

	GLfloat light4_position[] = { -10000.0f, -10000.0f, 0.0f, 0.0f };
	GLfloat light4_direction[] = { 10000.0f, 10000.0f, -0.0f, 0.0f };


	glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
	glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
	glLightfv(GL_LIGHT2, GL_POSITION, light2_position);
	glLightfv(GL_LIGHT3, GL_POSITION, light3_position);
	glLightfv(GL_LIGHT4, GL_POSITION, light4_position);


	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, light0_direction);
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, light1_direction);
	glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, light2_direction);
	glLightfv(GL_LIGHT3, GL_SPOT_DIRECTION, light3_direction);
	glLightfv(GL_LIGHT4, GL_SPOT_DIRECTION, light4_direction);


	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHT2);
	glEnable(GL_LIGHT3);
	glEnable(GL_LIGHT4);
}

GLMesh* StretchedDesignWindow::getTriangulatedFabricAreaMesh()
{
	GLMesh *mesh = new GLMesh();
	std::vector<P3D> trianglePts = triangulation.getOrderedTriangulationPoints();
	for (unsigned int i = 0; i < (int)trianglePts.size(); i++) {
		mesh->addVertex(trianglePts[i]);
	}
	return mesh;
}

DelaunayTriangulation StretchedDesignWindow::getDelaunayTriangulation() {
	return triangulation;
}

P3D StretchedDesignWindow::screenCoordsToFlatSurfacePoint(double xPos, double yPos, StretchedFlatSurface* flatSurface) {
	Ray clickedRay = getRayFromScreenCoords(xPos, yPos);
	P3D clickedPoint = flatSurface->pointClosestToPlane(clickedRay);
	clickedPoint = flatSurface->transformClickedPointToSurface(clickedPoint);
	if (shouldPrintMouseLocation) {
		Logger::consolePrint("Mouse Location: %f %f %f", clickedPoint[0], clickedPoint[1], clickedPoint[2]);
	}
	return clickedPoint;
}

//triggered when mouse moves
bool StretchedDesignWindow::onMouseMoveEvent(double xPos, double yPos) {
	preDraw();
	bool processed = false;

	P3D clickedFabricSurfacePt = screenCoordsToFlatSurfacePoint(xPos, yPos, fabricSurface);
	lastMovedFabricSurfacePoint = clickedFabricSurfacePt;
	if (windowMode == StretchedDesignWindowMode::EDIT) {
		if (extrusionMaker->getState() == StretchedExtrusionMakerState::EDITING 
			&& GlobalMouseState::lButtonPressed) {
			// Update our position if necessary
			extrusionMaker->updatedSelectedPointPosition(clickedFabricSurfacePt);
			processed = true;
		}
		else {
			extrusionMaker->updateSelectedPointIdFromPoint(clickedFabricSurfacePt);
		}
	}

	if (fabricSurface->isWithinSurface(clickedFabricSurfacePt)) {
		// Hack to get the clicked point to work - it seems the point returned from our plane check is wrong when coming
		// from a mouse move event to a click (it always returns (0, 0, 4) for clicks. We use the last moved surface point instead
		lastMovedFabricSurfacePoint = clickedFabricSurfacePt;
		// TODO: Handle case for when we are making curves with button pressed
		// Logger::consolePrint("Moved in the surface");
		// Logger::consolePrint("----");
		processed = true;
	}
	postDraw();
	if (processed) {
		return true;
	}
	return true;
	// Below enabled camera movement - since we want 2D drawing on the plane we restring the design view to being only zoom capabale.
	// We may have to handle the case of drawing curves above and return appropriately
	//return GLWindow3D::onMouseMoveEvent(xPos, yPos);
}


void StretchedDesignWindow::onMenuMouseButtonProcessedEvent() {
	//if (selectedBodyFeatures.size() == 1)
	//	selectedBodyFeatures[0]->parent->propagateChangesToMirroredBody();
}

//triggered when mouse buttons are pressed
bool StretchedDesignWindow::onMouseButtonEvent(int button, int action, int mods, double xPos, double yPos) {
	// Check if our click intersects with our fabric plane
	P3D clickedFabricSurfacePt = screenCoordsToFlatSurfacePoint(xPos, yPos, fabricSurface);
	// See hack above- the point returned from the Ray-Plane closest is always (0, 0, 4), so we use the lastmovedsurfacepoint
	clickedFabricSurfacePt = lastMovedFabricSurfacePoint;
	if (windowMode == StretchedDesignWindowMode::EDIT) {
		if (extrusionMaker->getState() == StretchedExtrusionMakerState::EDITING && action == GLFW_PRESS) {
			extrusionMaker->updateSelectedPointIdFromPoint(clickedFabricSurfacePt);
		} else if (action == GLFW_RELEASE) {
			extrusionMaker->updateSelectedPointIdFromPoint(clickedFabricSurfacePt);
		} else {
			extrusionMaker->makeCurve(clickedFabricSurfacePt, makerObjectType);
			extrusionMaker->setState(StretchedExtrusionMakerState::EDITING);
		}
	}

	if (fabricSurface->isWithinSurface(clickedFabricSurfacePt)) {
		// Logger::consolePrint("Clicked on the surface");
		return true;
	}

	return true;
	//return  GLWindow3D::onMouseButtonEvent(button, action, mods, xPos, yPos);
}

//triggered when using the mouse wheel
bool StretchedDesignWindow::onMouseWheelScrollEvent(double xOffset, double yOffset) {
	if (GLWindow3D::onMouseWheelScrollEvent(xOffset, yOffset)) return true;
	return true;
}

bool StretchedDesignWindow::onKeyEvent(int key, int actionI, int mods) {
	
	StretchedExtrusionMakerState currentState = extrusionMaker->getState();
	
	if (compareKeyPressIgnoreCase(key, 'O') && actionI == GLFW_PRESS) {
		// If we are editing and press this, then we finish the current extrusion curve
		switch (currentState) {
			case EDITING:
				Logger::consolePrint("Finished creating extrusion");
				extrusions.push_back(extrusionMaker->createExtrusion());
				extrusionMaker->setState(StretchedExtrusionMakerState::NONE);
				break;
		}
	} else if (compareKeyPressIgnoreCase(key, 'P') && actionI == GLFW_PRESS) {
		switch (currentState) {
			case EDITING:
				Logger::consolePrint("Deleted bezier curve");
				extrusionMaker->clearCurve();
				extrusionMaker->setState(StretchedExtrusionMakerState::NONE);
				break;
		}
	} else if (compareKeyPressIgnoreCase(key, 'w') && actionI == GLFW_PRESS) {
		// Concatenate all the extrusions
		int totalExtrusionPtSize = 0;
		int extrusionsSize = extrusions.size();
		for (int i = 0; i < extrusionsSize; i++) {
			totalExtrusionPtSize += extrusions[i]->points.size();
		}
		std::vector<P3D> allPoints = std::vector<P3D>();
		allPoints.reserve(totalExtrusionPtSize);
		for (int i = 0; i < extrusionsSize; i++) {
			std::vector<P3D> exPts = extrusions[i]->points;
			allPoints.insert(allPoints.end(), exPts.begin(), exPts.end());
		}
		triangulation = triangulator->triangulatePoints(allPoints, DelaunayTriangulatorPlane2DType::XZ_PLANE);
		triangulation.setColor(StretchedColor::GREEN);
		Logger::consolePrint("Finished  Delaunay Triangulation");
	}

	// We don't propagate the events to the window because we don't allow 3D camera interaction (apart from zoom).
	// return (GLWindow3D::onKeyEvent(key, actionI, mods));
	return true;
}

bool StretchedDesignWindow::onCharacterPressedEvent(int key, int mods) {
	if (GLWindow3D::onCharacterPressedEvent(key, mods)) return true;
	return false;
}

void StretchedDesignWindow::loadFile(const char* fName) {
	Logger::consolePrint("Loading file \'%s\'...\n", fName);
	std::string fileName;
	fileName.assign(fName);
	std::string fNameExt = fileName.substr(fileName.find_last_of('.') + 1);
	// robot->readRobotFromFile(fName);
}

void StretchedDesignWindow::saveFile(const char* fName) {
	//robot->saveRobotToFile(fName);
}

// Draw the AppRobotDesigner scene - camera transformations, lighting, shadows, reflections, etc AppRobotDesignerly to everything drawn by this method
void StretchedDesignWindow::drawScene() {
	glColor3d(1, 1, 1);
	glDisable(GL_LIGHTING);
	glPushMatrix();
	glScaled(0.15, 0.15, 0.15);
	if (showDesignEnvironment) {
		drawGround();
	}
	glPopMatrix();

	fabricSurface->draw();
	extrusionMaker->draw();

	// Draw extrusions
	if (showExtrusions) {
		for (int i = 0; i < (int) extrusions.size(); i++) {
			extrusions.at(i)->draw();
		}
	}

	// Draw triangulation pts
	if (showDelaunayTriangulaton) {
		triangulation.draw();
	}

}

// This is the wild west of drawing - things that want to ignore depth buffer, camera transformations, etc. Not pretty, quite hacky, but flexible. Individual apps should be careful with implementing this method. It always gets called right at the end of the draw function
void StretchedDesignWindow::drawAuxiliarySceneInfo() {
	preDraw();
	glClear(GL_DEPTH_BUFFER_BIT);
	glDisable(GL_TEXTURE_2D);

	// TODO(mrivera) draw selected fabric outline
	postDraw();
}

void StretchedDesignWindow::reset() {
	if (fabricSurface != NULL) {
		delete fabricSurface;
	}
	// Fabric Surface
	fabricSurface = new StretchedFlatSurface(
		STRETCHED_FLAT_FABRIC_LENGTH,
		STRETCHED_FLAT_FABRIC_WIDTH,
		P3D(0, 0, 0));
	StretchedColor r = StretchedColor::RED;
	r.alpha = 0.5;
	fabricSurface->setColor(r);

	// Triangulator
	if (triangulator != NULL) {
		delete triangulator;
	}
	triangulator = new DelaunayTriangulator();

	// Extrusion Maker 
	if (extrusionMaker != NULL) {
		delete extrusionMaker;
	}
	extrusionMaker = new StretchedExtrusionMaker();

	// Clear any completed extrusions
	extrusions.clear();

	// Clear any triangulation points
	triangulation = DelaunayTriangulation();
}
