#include "StretchedSimWindow.h"
#include <GUILib/GLUtils.h>
#include <GUILib/GLMesh.h>
#include <GUILib/GLContentManager.h>
#include <MathLib/MathLib.h>
#include <MathLib/Matrix.h>

//void TW_CALL toggleSymBodyPair(void* clientData) {
//	//((StretchedSimWindow*)clientData)->createOrRemoveSymPair();
//}

StretchedSimWindow::StretchedSimWindow(int x, int y, int w, int h, GLApplication* glApp) : GLWindow3D(x, y, w, h){
	//setWindowTitle("Test AppRobotDesignerlication...");
	this->glApp = glApp;
	robot = new RobotDesign();

	TwAddVarRW(glApp->mainMenuBar, "Structure Features: attachment points", TW_TYPE_BOOLCPP,  &StructureFeature::showAttachmentPoints, "");
	TwAddVarRW(glApp->mainMenuBar, "Structure Features: convex hull", TW_TYPE_BOOLCPP, &StructureFeature::showConvexHull, "");
	TwAddVarRW(glApp->mainMenuBar, "Structure Features: wire frame", TW_TYPE_BOOLCPP, &StructureFeature::showWireFrameConvexHull, "");
	
	//TwAddButton(glApp->mainMenuBar, "Toggle Symmetric Body Pairs ", toggleSymBodyPair, this, " label='Symmetric Body Pairs' group='Operation' key='s' ");

	delete camera;
	camera = new GLTrackingCamera(-1.5, V3D(0, -1, 0), V3D(0, 0, 1));
	camera->setCameraTarget(P3D(0, 0, 0));
}

StretchedSimWindow::~StretchedSimWindow(void) {
}

void StretchedSimWindow::setupLights() {
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



//RobotBodyPart* StretchedSimWindow::getClosestRobotBodyPartToRay(const Ray& ray, double& dist) {
//	updateBodyPartFeaturesList();
//	RobotBodyPart* ans = NULL;
//	dist = -1;
//
//	for (uint i = 0; i < robot->robotBodyParts.size(); i++) {
//		double tmp = robot->robotBodyParts[i]->getDistanceToRayOriginIfHit(ray);
//		if (tmp >= 0 && (dist < 0 || dist > tmp)) {
//			dist = tmp;
//			ans = robot->robotBodyParts[i];
//		}
//	}
//
//	return ans;
//}

//BaseRobotBodyFeature* StretchedSimWindow::getClosestBodyFeatureToRay(const Ray& ray, double& dist) {
//	updateBodyPartFeaturesList();
//	BaseRobotBodyFeature* ans = NULL;
//	dist = -1;
//
//	for (uint i = 0; i < featureList.size(); i++){
//		double tmp = featureList[i]->getDistanceToRayOriginIfHit(ray);
//		if (tmp >= 0 && (dist < 0 || dist > tmp)) {
//			dist = tmp;
//			ans = featureList[i];
//		}
//	}
//
//	return ans;
//}

//triggered when mouse moves
bool StretchedSimWindow::onMouseMoveEvent(double xPos, double yPos) {
	preDraw();
	//Ray clickedRay = getRayFromScreenCoords(xPos, yPos);
	//bool processed = false;

	//if (!processed && rotateWidget.onMouseMoveEvent(xPos, yPos)) {
	//	//update the orientation if we've selected a joint type feature
	//	if (RobotJointFeature* j = dynamic_cast<RobotJointFeature*>(selectedBodyFeatures[0]))
	//		j->setJointAxis(rotateWidget.getOrientation() * j->getLocalCoordinatesRotationAxis(), glfwGetKey(glApp->glfwWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS);
	//	processed = true;
	//}

	//if (!processed && translateWidget.onMouseMoveEvent(xPos, yPos)) {
	//	//update the position of the selected object and continue...
	//	if (selectedBodyFeatures.size() == 1)
	//		selectedBodyFeatures[0]->setPosition(translateWidget.pos, glfwGetKey(glApp->glfwWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS);
	//	processed = true;
	//}

	////check if we're hovering over anything important here...
	//if (!processed && GlobalMouseState::lButtonPressed == false && GlobalMouseState::rButtonPressed == false && GlobalMouseState::mButtonPressed == false) {
	//	//we should only have one thing highlighted at one time - so un-highlight everything...
	//	updateBodyPartFeaturesList();
	//	double featureDist;
	//	for (uint i = 0; i < featureList.size(); i++)
	//		featureList[i]->highlighted = false;
	//	BaseRobotBodyFeature* highlightedBodyFeature = getClosestBodyFeatureToRay(clickedRay, featureDist);
	//	if (highlightedBodyFeature)
	//		highlightedBodyFeature->highlighted = true;

	//	double bodyPartDist;
	//	//check to see if any of the body parts should be highlighted...
	//	for (uint i = 0; i < robot->robotBodyParts.size(); i++)
	//		robot->robotBodyParts[i]->highlighted = false;
	//	RobotBodyPart* highlightedBodyPart = getClosestRobotBodyPartToRay(clickedRay, bodyPartDist);

	//	if (highlightedBodyPart)
	//		if (highlightedBodyFeature == NULL || (highlightedBodyFeature->parent != highlightedBodyPart && featureDist > bodyPartDist))
	//			highlightedBodyPart->highlighted = true;
	//	if (highlightedBodyPart && highlightedBodyPart->highlighted && highlightedBodyFeature)
	//		highlightedBodyFeature->highlighted = false;
	//}

	postDraw();

	//if (processed) return true;
	return true;
	// Below enabled camera movement - since we want 2D drawing on the plane we restring the design view to being only zoom capabale.
	// We may have to handle the case of drawing curves above and return appropriately
	// return GLWindow3D::onMouseMoveEvent(xPos, yPos);
}


void StretchedSimWindow::onMenuMouseButtonProcessedEvent() {
	//if (selectedBodyFeatures.size() == 1)
	//	selectedBodyFeatures[0]->parent->propagateChangesToMirroredBody();
}


//triggered when mouse buttons are pressed
bool StretchedSimWindow::onMouseButtonEvent(int button, int action, int mods, double xPos, double yPos) {
//	if (translateWidget.onMouseButtonEvent(button, action, mods, xPos, yPos))
//		return true;
//	if (rotateWidget.onMouseButtonEvent(button, action, mods, xPos, yPos))
//		return true;
//
//	//if any feature is being hovered over, and we click the mouse button, then we should invert its selection
//	if (GlobalMouseState::lButtonPressed == true) {
//		//change the selection of anything that is highlighted
//		updateBodyPartFeaturesList();
//		selectedBodyFeatures.clear();
//		selectedBodyParts.clear();
//
//
//		for (uint i = 0; i < robot->robotBodyParts.size(); i++) {
//			if (mods & GLFW_MOD_CONTROL) 
//				robot->robotBodyParts[i]->selected = false;
//			if (robot->robotBodyParts[i]->highlighted)
//				robot->robotBodyParts[i]->selected = !robot->robotBodyParts[i]->selected;
//			if (robot->robotBodyParts[i]->selected) {
//				selectedBodyParts.push_back(robot->robotBodyParts[i]);
//			}
//		}
//
//		bool bodyFeatureHighlighted = false;
//		for (uint i = 0; i < featureList.size(); i++)
//			if (featureList[i]->highlighted)
//				bodyFeatureHighlighted = true;
//
//		for (uint i = 0; i < featureList.size(); i++)
//			if (featureList[i]->selected)
//				unloadMenuParametersFor(featureList[i]);
//
//		for (uint i = 0; i < featureList.size(); i++) {
////			no need to ever have multiple body features selected at the same time
//			if ((mods & GLFW_MOD_CONTROL || bodyFeatureHighlighted) && featureList[i]->highlighted == false)
//				featureList[i]->selected = false;
//			if (featureList[i]->highlighted)
//				featureList[i]->selected = !featureList[i]->selected;
//			if (featureList[i]->selected) {
//				selectedBodyFeatures.push_back(featureList[i]);
//				loadMenuParametersFor(featureList[i]);
//			}
//		}
//		return true;
//	}
//

	return false;
	return  GLWindow3D::onMouseButtonEvent(button, action, mods, xPos, yPos);
}

//triggered when using the mouse wheel
bool StretchedSimWindow::onMouseWheelScrollEvent(double xOffset, double yOffset) {
	if (GLWindow3D::onMouseWheelScrollEvent(xOffset, yOffset)) return true;

	return true;
}

bool StretchedSimWindow::onKeyEvent(int key, int actionI, int mods) {
	if (key == 't' || key == 'T' && actionI == GLFW_PRESS) {
		showTranslateWidget = !showTranslateWidget;
		if (showTranslateWidget)
			showRotateWidget = false;
	}
	else if (key == 'r' || key == 'R' && actionI == GLFW_PRESS) {
		showRotateWidget = !showRotateWidget;
		if (showRotateWidget)
			showTranslateWidget = false;
	}

	return (GLWindow3D::onKeyEvent(key, actionI, mods));
 
	return false;
}

bool StretchedSimWindow::onCharacterPressedEvent(int key, int mods) {
	if (GLWindow3D::onCharacterPressedEvent(key, mods)) return true;

	return false;
}

void StretchedSimWindow::loadFile(const char* fName) {
	Logger::consolePrint("Loading file \'%s\'...\n", fName);
	std::string fileName;
	fileName.assign(fName);
	std::string fNameExt = fileName.substr(fileName.find_last_of('.') + 1);
	delete robot;
	robot = new RobotDesign();
	robot->readRobotFromFile(fName);
}

void StretchedSimWindow::saveFile(const char* fName) {
	//robot->saveRobotToFile(fName);
}

// Draw the AppRobotDesigner scene - camera transformations, lighting, shadows, reflections, etc AppRobotDesignerly to everything drawn by this method
void StretchedSimWindow::drawScene() {
	glColor3d(1, 1, 1);
	glDisable(GL_LIGHTING);
	glPushMatrix();
	glScaled(0.15, 0.15, 0.15);
	drawDesignEnvironmentBox();
	glPopMatrix();

	robot->draw();
}

// This is the wild west of drawing - things that want to ignore depth buffer, camera transformations, etc. Not pretty, quite hacky, but flexible. Individual apps should be careful with implementing this method. It always gets called right at the end of the draw function
void StretchedSimWindow::drawAuxiliarySceneInfo() {
	preDraw();
	glClear(GL_DEPTH_BUFFER_BIT);
	glDisable(GL_TEXTURE_2D);

	for (uint i = 0; i < robot->robotBodyParts.size(); i++)
		robot->robotBodyParts[i]->drawFeatures();

	//sync the widgets with the selected feature
	if (selectedBodyFeatures.size() == 1) {
		translateWidget.pos = selectedBodyFeatures[0]->positionWorld;
		rotateWidget.pos = selectedBodyFeatures[0]->positionWorld;
		if (RobotJointFeature* j = dynamic_cast<RobotJointFeature*>(selectedBodyFeatures[0]))
			rotateWidget.setOrientation(j->getOrientation());
	}
	translateWidget.visible = showTranslateWidget && selectedBodyFeatures.size() == 1;
	rotateWidget.visible = showRotateWidget && selectedBodyFeatures.size() == 1 && (dynamic_cast<RobotJointFeature*>(selectedBodyFeatures[0]));
	//	translateWidget.visible = showTranslateWidget && key1Down && (!robot->isCenter(selectedID[0]));
	//	rotateWidget.visible = showRotateWidget && key2Down && (!robot->isCenter(selectedID[0]));

	translateWidget.draw();
	rotateWidget.draw();
	postDraw();
}
