#pragma once

#include <RobotDesignerLib/RobotDesign.h>
#include <string>
#include <map>
#include <algorithm>
#include <GUILib/TranslateWidget.h>
#include <GUILib/RotateWidgetV1.h>
#include <GUILib/RotateWidgetV2.h>
#include <GUILib/GLWindow3D.h>

/**
 * StretchedSimWindow
 */
class StretchedSimWindow : public GLWindow3D {
private:
	GLApplication* glApp;
	TranslateWidget translateWidget;
	RotateWidgetV2 rotateWidget;
	bool showTranslateWidget = false, showRotateWidget = false;

	DynamicArray<BaseRobotBodyFeature*> featureList;

public:

	RobotDesign* robot;
	// constructor
	StretchedSimWindow(int x, int y, int w, int h, GLApplication* glApp);
	// destructor
	virtual ~StretchedSimWindow(void);

	// Draw the AppRobotDesigner scene - camera transformations, lighting, shadows, reflections, etc AppRobotDesignerly to everything drawn by this method
	virtual void drawScene();
	// This is the wild west of drawing - things that want to ignore depth buffer, camera transformations, etc. Not pretty, quite hacky, but flexible. Individual apps should be careful with implementing this method. It always gets called right at the end of the draw function
	virtual void drawAuxiliarySceneInfo();

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

	//BaseRobotBodyFeature* getClosestBodyFeatureToRay(const Ray& ray, double& dist);
	//RobotBodyPart* getClosestRobotBodyPartToRay(const Ray& ray, double& dist);
	//void updateBodyPartFeaturesList();

	void onMenuMouseButtonProcessedEvent();

	DynamicArray<BaseRobotBodyFeature*> selectedBodyFeatures;
	DynamicArray<RobotBodyPart*> selectedBodyParts;

	void setupLights();

	virtual void saveFile(const char* fName);
	virtual void loadFile(const char* fName);

	//void createOrRemoveSymPair();

	//void loadMenuParametersFor(BaseRobotBodyFeature* brbFeature);
	//void unloadMenuParametersFor(BaseRobotBodyFeature* brbFeature);

	
	
	
/*
	void updateParams(int ID);
	void clearSeletion();
	void split();
	void createOrRemoveSymPair();
	void stickToYZPlane();
	void newRobot();
	void save();

	void addJoint();
	void addFeaturePoint();

	//parameters used to display properties of selected body parts
	double posX = 0, posY = 0, posZ = 0, angleX = 0, angleY = 0, angleZ = 0;
	double parentEmbeddingAngle = 0, childEmbeddingAngle = 0;


	Ray lastClickedRay = Ray(P3D(0, 0, 0), V3D(0, 0, 1)), curClickedRay = Ray(P3D(0, 0, 0), V3D(0, 0, 1));
	bool shiftMode = false, jointMode = false, featurePointMode = false;

	int lastSelectedID = -1, curSelectedID = -1;
	void findAndAddOrRemove(int ID);
	void findAndRemove(int ID);
	std::vector<int> selectedID;

	bool mouseDownLeft = false, key1Down = false, key2Down = false, mouseDraging = false;


	double winDesWidth, winSimWidth;
	char saveFileName[256] = "sav.rbs";



	bool showGroundPlane = false;
*/
};



