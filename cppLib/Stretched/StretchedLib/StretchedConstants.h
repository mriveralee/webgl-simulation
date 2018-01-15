#pragma once

#include "MathLib/P3D.h"
#include "MathLib/V3D.h"
#include "StretchedColor.h"
#include "GUILib/GLUtils.h"


static const double STRETCHED_DESIGN_CAMERA_INITIAL_ZOOM_DISTANCE = -7.0;
static const V3D STRETCHED_DESIGN_CAMERA_INITIAL_VIEW_DIRECTION = V3D(0, -1, 0);
static const V3D STRETCHED_DESIGN_CAMERA_INITIAL_UP_AXIS = V3D(0, 0, 1);
static const P3D STRETCHED_DESIGN_CAMERA_INITIAL_TARGET_POSITION = P3D(0, 0, 0);

static const double STRETCHED_FLAT_FABRIC_WIDTH = 6.0;
static const double STRETCHED_FLAT_FABRIC_LENGTH = 4.0;
static const P3D STRETCHED_FABRIC_START_POSITION = P3D(0, 0, 0);

static const double STRETCHED_CONFIG_FABRIC_MESH_SCALE_FACTOR = 15.0;

static const StretchedColor CONTROL_POINTS_COLOR = StretchedColor(0, 0, 0, 0.3);
static const StretchedColor CONTROL_POINTS_HIGHLIGHTED_COLOR = StretchedColor::YELLOW;

static const GLfloat DEFAULT_GL_POINT_SIZE = 1;
static const GLfloat DEFAULT_GL_LINE_WIDTH = 1;


// Simulation Constants

//#define DEFAULT_STIFFNESS 1
//#define PIN_STIFFNESS 100
//#define DEFAULT_MASS 0.1

#define DELTA_T 0.016
#define NEWTON_TOLERANCE 0.01

#define EPSILON_CHECK 1e-10

#define SPHERE_QUALITY 20

#define DEFAULT_STIFFNESS 1
#define PIN_STIFFNESS 100
#define DEFAULT_MASS 0.1

#define VSICOSITY 0.05
#define BALL_FRICTION 0.5
#define BALL_TEXTURE_NUM 9