#include "ActuatorProperty.h"

// don't official function, since this is used by all three tiers, where <math.h> is not always present.
#define radians(deg) (deg*(3.1415927/180.0))

// configuration data of joints
// (gear ratio, min[rad], max[rad]
// minAngle/maxAngle/acceleration/maxSpeed is used by Visualizer only
AllActuatorsConfigType actuatorConfigType =  {
	// actuator							gear ratio					angle-offset	min angle			max angle			accel maxspeed (RPM)
	{ ActuatorConfiguration::HIP,		(90.0/10.0), 				0.0,			radians(-179.0f)	,radians(179.0f), 	161,	400},
	{ ActuatorConfiguration::UPPERARM,  (72.0/14.0)*(48.0/14.0), 	0.0,			radians(-89.0f)		,radians(89.0f), 	161,	600},
	{ ActuatorConfiguration::FOREARM,   (60.0/14.0)*(48.0/15.0), 	90.0,			radians(-135.0f)	,radians(135.0f), 	161,	600},
	{ ActuatorConfiguration::ELLBOW,	(49.0/16.0)*(34.0/14.0), 	0.0,			radians(-180.0f)	,radians(180.0f),	161,	600},
	{ ActuatorConfiguration::WRIST,		(65.0/15.0),				0.0,			radians(-100.0f)	,radians(100.0f),	161,	600}, 
	{ ActuatorConfiguration::HAND,		1.0, 						0.0,			radians(-180.0f)	,radians(180.0f),	161,	600}, 
	{ ActuatorConfiguration::GRIPPER,	1.0, 						0.0,			radians(7.0f)		,radians(80.0f),	161,	600}
};
