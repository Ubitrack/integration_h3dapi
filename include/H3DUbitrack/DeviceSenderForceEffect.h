#ifndef __DEVICESENDERFORCEEFFECT_H__
#define __DEVICESENDERFORCEEFFECT_H__

#include <H3DUbitrack/H3DUbitrack.h>
#include <H3DUbitrack/DeviceSenderHapticForceEffect.h>

#include <H3D/H3DForceEffect.h>

#include <H3D/SFInt32.h>
#include <H3D/SFMatrix4f.h>
#include <H3D/SFVec3f.h>
#include <H3D/SFFloat.h>
#include <H3D/SFBool.h>
#include <H3D/MFString.h>
#include <H3D/Scene.h>
#include <H3DUtil/Threads.h>


namespace H3DUbitrack {

using namespace H3D;

/// \ingroup H3DUbitrack Nodes
/// \class DeviceSenderForceEffect
/// \brief The DeviceSenderForceEffect manages the DeviceSenderHapticForceEffect
///
class H3DUBITRACK_API DeviceSenderForceEffect :
public H3DForceEffect {

public:

	/// Constructor.
	DeviceSenderForceEffect( Inst< SFNode > _metadata = 0,
			Inst< SFVec3f > _holeOrigin = 0,
			Inst< SFVec3f > _holeTarget = 0,
	        Inst< MFVec3f > _torque     = 0,
			Inst< MFVec3f > _force      = 0,
			Inst< MFVec3f > _maxTorque  = 0,
			Inst< MFVec3f > _maxForce   = 0,
			Inst< SFFloat > _positionError = 0,
			Inst< SFFloat > _orientationError = 0,
			Inst< SFBool > _rotOK             = 0,
			Inst< SFRotation > _rotationDiff = 0,
			Inst< SFFloat > _radius           = 0,
			Inst< SFFloat > _torqueSpringConstant = 0,
			Inst< SFFloat > _maxAngleDiff = 0,
			Inst< SFFloat > _lockPositionSpring = 0,
			Inst< SFFloat > _lockPositionDamping = 0,
			Inst< SFBool  > _enableTorqueFeedback = 0,
			Inst< SFBool  > _enableLocking = 0,
			Inst< MFInt32 > _deviceIndex = 0);

	~DeviceSenderForceEffect();

	/// Initializes the endpoint
	virtual void initialize();

	virtual void traverseSG(TraverseInfo &ti);

	auto_ptr< MFVec3f > torque;
	auto_ptr< MFVec3f > force;
	auto_ptr< MFVec3f > maxTorque;
	auto_ptr< MFVec3f > maxForce;

	auto_ptr< SFFloat > positionError;
	auto_ptr< SFFloat > orientationError;
	auto_ptr< SFBool > rotOK;
	auto_ptr< SFRotation > rotationDiff;

	auto_ptr< SFVec3f > holeOrigin;
	auto_ptr< SFVec3f > holeTarget;
	auto_ptr< SFFloat > radius;

	auto_ptr< SFFloat > torqueSpringConstant;
	auto_ptr< SFFloat > maxAngleDiff;

	auto_ptr< SFFloat > lockPositionSpring;
	auto_ptr< SFFloat > lockPositionDamping;

	auto_ptr< SFBool > enableTorqueFeedback;
	auto_ptr< SFBool > enableLocking;



	/// The H3DNodeDatabase for this node.
	static H3DNodeDatabase database;

protected:
	/// Internal haptic force effect instance
	AutoRefVector< DeviceSenderHapticForceEffect > haptic_force_effect;

	/// Internal function used to create a haptic force effect for the
	/// haptics device of the given index.
	void createHapticForceEffect( int index );

	auto_ptr< Field > updateHapticForceEffect;
	
	void updateValuesFromDevice( int index );

	bool last_enabled;
};
}

#endif
