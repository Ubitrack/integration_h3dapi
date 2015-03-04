//////////////////////////////////////////////////////////////////////////////
//    Copyright 2013 Magic Vision Lab
//
//
/// \file DeviceSenderHapticForceEffect.h
/// \brief Header file for DeviceSenderHapticForceEffect
///
//
//////////////////////////////////////////////////////////////////////////////
#ifndef __REMOTEPHANTOMDEVICESENDERHAPTICFORCEEFFECT_H__
#define __REMOTEPHANTOMDEVICESENDERHAPTICFORCEEFFECT_H__

#include <H3DUbitrack/H3DUbitrack.h>
#include <H3DUbitrack/MeasurementSender.h>

#include <HAPI/HAPIForceEffect.h> 
#include <HAPI/HAPIHapticsDevice.h>

#include <string>
#include <math.h>
#include <vector>


namespace H3DUbitrack {
/// \ingroup ForceEffects
/// \class DeviceSenderHapticForceEffect
/// \brief A haptic force effect which sends a pos/orn to an external
/// haptic renderer and receives force/torque

class H3DUBITRACK_API RemotePhantomDeviceSenderHapticForceEffect: public HAPI::HAPIForceEffect {
public:
	RemotePhantomDeviceSenderHapticForceEffect(
			Ubitrack::Components::ApplicationPushSourcePose* _push_source_pose,
			Ubitrack::Components::ApplicationPushSourcePosition* _push_source_jointAngles,
			Ubitrack::Components::ApplicationPushSourcePosition* _push_source_gimbalAngles,
			bool _is_active,
			int _freq = 100
	);

	~RemotePhantomDeviceSenderHapticForceEffect();

	/// The force of the EffectOutput is calculated from the provided functions
	HAPIForceEffect::EffectOutput virtual calculateForces( const HAPIForceEffect::EffectInput &input );

    inline bool &getActive() {
      return is_active;
    }

    inline  void setActive(bool _active) {
      is_active = _active;
    }

protected:
    Ubitrack::Components::ApplicationPushSourcePose* push_source_pose;
	Ubitrack::Components::ApplicationPushSourcePosition* push_source_jointAngles;
	Ubitrack::Components::ApplicationPushSourcePosition* push_source_gimbalAngles;
	bool is_active;

    HAPI::HAPITime last_time, start_time;
    HAPI::HAPITime time_diff;

};

}

#endif
