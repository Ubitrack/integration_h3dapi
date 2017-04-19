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
#include <H3DUtil/Threads.h>

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
			bool _is_active,
			int _freq = 100
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
	
	H3DUtil::MutexLock m_lock;

protected:
    Ubitrack::Components::ApplicationPushSourcePose* push_source_pose;
	bool is_active;

    HAPI::HAPITime last_time, start_time;
    HAPI::HAPITime time_diff;

};

}

#endif
