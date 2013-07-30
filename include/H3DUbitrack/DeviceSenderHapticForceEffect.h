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

class H3DUBITRACK_API DeviceSenderHapticForceEffect: public HAPI::HAPIForceEffect {
public:
	DeviceSenderHapticForceEffect(
			Ubitrack::Components::ApplicationPushSourcePose* _push_source_pose,
			Ubitrack::Components::ApplicationPushSourcePosition* _push_source_jointAngles,
			Ubitrack::Components::ApplicationPushSourcePosition* _push_source_gimbalAngles,
			bool _is_active
	);

	~DeviceSenderHapticForceEffect();

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
};

}

#endif
