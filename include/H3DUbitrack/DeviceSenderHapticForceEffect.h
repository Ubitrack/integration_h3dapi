//////////////////////////////////////////////////////////////////////////////
//    Copyright 2013 Magic Vision Lab
//
//
/// \file DeviceSenderHapticForceEffect.h
/// \brief Header file for DeviceSenderHapticForceEffect
///
//
//////////////////////////////////////////////////////////////////////////////
#ifndef __DEVICESENDERHAPTICFORCEEFFECT_H__
#define __DEVICESENDERHAPTICFORCEEFFECT_H__

#include <H3DUbitrack/H3DUbitrack.h>
#include <HAPI/HAPIForceEffect.h> 
#include <HAPI/HAPIHapticsDevice.h>

#include <string>
#include <iostream>
#include <math.h>
#include <vector>

#include <fstream>

/* Example from TUM prototype

class TestForceEffect : public HAPI::HAPIForceEffect {
public:
	/// Constructor
	TestForceEffect(SimplePoseReceiver* _poseReceiver,
		SimplePosition3DReceiver* _jointAngleReceiver,
		SimplePosition3DReceiver* _gimbalAngleReceiver,
		SimpleFacade* _facade)
		: poseReceiver(_poseReceiver),
		jointAngleReceiver(_jointAngleReceiver),
		gimbalAngleReceiver(_gimbalAngleReceiver),
		facade(_facade)
	{
		// no initialization needed for now
	}


	/// The force of the EffectOutput will be a force from the position of
	/// the haptics device to the position of the HapticSpring.
	HAPI::HAPIForceEffect::EffectOutput calculateForces( const HAPI::HAPIForceEffect::EffectInput &input ) {

		unsigned long long tstamp = facade->now();
		// retrieve information from haptic device state
		HAPI::PhantomHapticsDevice* hd = static_cast<HAPI::PhantomHapticsDevice*>(input.hd);

		if (poseReceiver != nullptr) {
			HAPI::Vec3 device_pos = hd->getPosition();
			HAPI::Quaternion device_orn = HAPI::Quaternion(hd->getOrientation());
			// convert pose HAPI -> Ubitrack
			SimplePose pose;
			pose.tx = device_pos.x;
			pose.ty = device_pos.y;
			pose.tz = device_pos.z;
			pose.rx = device_orn.v.x;
			pose.ry = device_orn.v.y;
			pose.rz = device_orn.v.z;
			pose.rw = device_orn.w;
			pose.timestamp = tstamp;

			// send pose to DFG
			poseReceiver->receivePose(pose);
		}

		if (jointAngleReceiver != nullptr) {
			HAPI::Vec3 jA = hd->getJointAngles();
			// convert pose HAPI -> Ubitrack
			SimplePosition3D jAv;
			jAv.x = jA.x;
			jAv.y = jA.y;
			jAv.z = jA.z;
			jAv.timestamp = tstamp;

			// send pose to DFG
			jointAngleReceiver->receivePosition3D(jAv);
		}

		if (gimbalAngleReceiver != nullptr) {
			HAPI::Vec3 gA = hd->getGimbalAngles();
			// convert pose HAPI -> Ubitrack
			SimplePosition3D gAv;
			gAv.x = gA.x;
			gAv.y = gA.y;
			gAv.z = gA.z;
			gAv.timestamp = tstamp;

			// send pose to DFG
			gimbalAngleReceiver->receivePosition3D(gAv);
		}

		// return no force - this is only a sensor for now
		return HAPI::HAPIForceEffect::EffectOutput( HAPI::Vec3(0., 0., 0.), HAPI::Vec3(0., 0., 0.) );
	}

protected:
	SimplePoseReceiver* poseReceiver;
	SimplePosition3DReceiver* jointAngleReceiver;
	SimplePosition3DReceiver* gimbalAngleReceiver;
	SimpleFacade* facade;
};

*/




namespace H3DUbitrack {
/// \ingroup ForceEffects
/// \class DeviceSenderHapticForceEffect
/// \brief A haptic force effect which sends a pos/orn to an external
/// haptic renderer and receives force/torque

class H3DUBITRACK_API DeviceSenderHapticForceEffect: public HAPI::HAPIForceEffect {
public:
	DeviceSenderHapticForceEffect(const HAPI::Vec3 &_holeOrigin, const HAPI::Vec3 &_holeTarget,
		HAPI::HAPIFloat _radius, HAPI::HAPIFloat _torqueSpringConstant,
		HAPI::HAPIFloat _maxAngleDiff, 
		HAPI::HAPIFloat _lockPositionSpring, HAPI::HAPIFloat _lockPositionDamping);

	~DeviceSenderHapticForceEffect();

	/// The force of the EffectOutput is calculated from the provided functions
	HAPIForceEffect::EffectOutput virtual calculateForces( const HAPIForceEffect::EffectInput &input );

    inline const HAPI::Vec3 &getLatestTorque() {
      return torque;
    }

    inline const HAPI::Vec3 &getLatestForce() {
      return force;
    }

    inline const HAPI::Vec3 &getMaxTorque() {
      return max_torque;
    }

    inline const HAPI::Vec3 &getMaxForce() {
      return max_force;
    }

    inline const HAPI::HAPIFloat &getPositionError() {
      return position_error;
    }

    inline const HAPI::HAPIFloat &getOrientationError() {
      return orientation_error;
    }

    inline const bool &getRotOK() {
      return rotOK;
    }

    inline const HAPI::Rotation &getRotationDiff() {
      return rotationDiff;
    }

    inline  void setTorqueFeedbackEnabled(bool _enabled) {
      torqueFeedbackEnabled = _enabled;
    }

    inline  void setLockingEnabled(bool _enabled) {
      lockingEnabled = _enabled;
    }


protected:
	const HAPI::Vec3 holeOrigin;
	const HAPI::Vec3 holeTarget;

	HAPI::HAPIFloat position_error;
	HAPI::HAPIFloat orientation_error;
	bool rotOK;
	HAPI::Rotation rotationDiff;

	bool is_locked;
	HAPI::Vec3 lock_position;
	H3DUtil::TimeStamp unlock_ts;

	HAPI::Vec3 force;
	HAPI::Vec3 torque;

	HAPI::Vec3 max_force;
	HAPI::Vec3 max_torque;

	HAPI::HAPIFloat radius;
	HAPI::HAPIFloat torqueSpringConstant;
	HAPI::HAPIFloat torqueLimit;
	HAPI::HAPIFloat maxAngleDiff;

	HAPI::HAPIFloat lockPositionSpring;
	HAPI::HAPIFloat lockPositionDamping;

	bool torqueFeedbackEnabled;
	bool lockingEnabled;

	bool enable_debuglog;
	ofstream log_file;
};
}

#endif
