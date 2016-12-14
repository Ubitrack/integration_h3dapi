//////////////////////////////////////////////////////////////////////////////
//    Copyright 2004-2013, SenseGraphics AB
//
//    This file is part of HAPI.
//
//    HAPI is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    HAPI is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with HAPI; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//    A commercial license is also available. Please contact us at
//    www.sensegraphics.com for more information.
//
//
/// \file DeviceSenderHapticForceEffect.cpp
/// \brief cpp file for DeviceSenderHapticForceEffect
///
//
//////////////////////////////////////////////////////////////////////////////
#include <H3DUbitrack/H3DUbitrack.h>

// needs a modified version of HAPI and the latest release of ScaleAPI from Haption
#ifdef HAVE_VIRTUOSEAPI_CUSTOM

#include <H3DUbitrack/HaptionDeviceSenderHapticForceEffect.h>

#include <H3DUbitrack/H3DUbitrack.h>
#include <H3DUbitrack/MeasurementSender.h>
#include <HaptionHAPI/ScaleHapticsDevice.h>

#include <H3DUtil/ReadWriteH3DTypes.h>
#include <H3DUtil/Console.h>

using namespace H3DUbitrack;
using namespace HAPI;

HaptionDeviceSenderHapticForceEffect::HaptionDeviceSenderHapticForceEffect(
		Ubitrack::Components::ApplicationPushSourcePose* _push_source_pose,
		Ubitrack::Components::ApplicationPushSourcePosition* _push_source_platformSensors,
		Ubitrack::Components::ApplicationPushSourcePosition* _push_source_jointSensors,
		Ubitrack::Components::ApplicationPushSourcePosition* _push_source_gimbalSensors,
		bool _is_active, int _freq)
: push_source_pose(_push_source_pose)
, push_source_platformSensors(_push_source_platformSensors)
, push_source_jointSensors(_push_source_jointSensors)
, push_source_gimbalSensors(_push_source_gimbalSensors)
, is_active(_is_active)
{
  last_time = -1;
  time_diff = 1.0/_freq;
}

HaptionDeviceSenderHapticForceEffect::~HaptionDeviceSenderHapticForceEffect() {

}

HAPIForceEffect::EffectOutput HaptionDeviceSenderHapticForceEffect::calculateForces( const HAPIForceEffect::EffectInput &input) {
	bool ready = false;

    if( last_time < 0 ) {
      start_time = TimeStamp::now();
      last_time = start_time;
	  ready = true;
    } else {
      HAPITime this_time = TimeStamp::now();
      if( this_time - last_time >= time_diff ) {
        last_time = this_time;
		ready = true;
	  }
    }

	unsigned long long tstamp = Ubitrack::Measurement::now();

	if (is_active && ready) {
		// retrieve information from haptic device state
		HAPI::ScaleHapticsDevice* hd = static_cast<HAPI::ScaleHapticsDevice*>(input.hd);
		if (push_source_pose != NULL) {
			HAPI::Vec3 device_pos = hd->getRawPosition();
			HAPI::Quaternion device_orn = HAPI::Quaternion(
					hd->getRawOrientation());
			// convert HAPI -> Ubitrack
			Ubitrack::Measurement::Pose p(tstamp,
					Ubitrack::Math::Pose(
							Ubitrack::Math::Quaternion(device_orn.v.x,
									device_orn.v.y, device_orn.v.z,
									device_orn.w),
							Ubitrack::Math::Vector< double, 3 >(device_pos.x,
									device_pos.y, device_pos.z)));
			try {
				push_source_pose->send(p);
			} catch (Ubitrack::Util::Exception &e) {
				H3D::Console(4)
						<< "Error while pushing device sender measurement: "
						<< e.what() << std::endl;
			}
		}

		if (push_source_platformSensors != NULL) {
			HAPI::Vec3 pA = hd->getPlatformSensors();
			// convert HAPI -> Ubitrack
			Ubitrack::Measurement::Position p(tstamp, Ubitrack::Math::Vector< double, 3 >(pA.x, pA.y, pA.z));
			try {
				push_source_platformSensors->send(p);
			} catch (Ubitrack::Util::Exception &e) {
				H3D::Console(4) << "Error while pushing device sender measurement: " << e.what() << std::endl;
			}
		}

		if (push_source_jointSensors != NULL) {
			HAPI::Vec3 jA = hd->getJointSensors();
			// convert HAPI -> Ubitrack
			Ubitrack::Measurement::Position p(tstamp, Ubitrack::Math::Vector< double, 3 >(jA.x, jA.y, jA.z));
			try {
				push_source_jointSensors->send(p);
			} catch (Ubitrack::Util::Exception &e) {
				H3D::Console(4) << "Error while pushing device sender measurement: " << e.what() << std::endl;
			}
		}

		if (push_source_gimbalSensors != NULL) {
			HAPI::Vec3 gA = hd->getGimbalSensors();
			// convert HAPI -> Ubitrack
			Ubitrack::Measurement::Position p(tstamp, Ubitrack::Math::Vector< double, 3 >(gA.x, gA.y, gA.z));
			try {
				push_source_gimbalSensors->send(p);
			} catch (Ubitrack::Util::Exception &e) {
				H3D::Console(4) << "Error while pushing device sender measurement: " << e.what() << std::endl;
			}
		}

	}
	// no return value
	return HAPIForceEffect::EffectOutput();
}


#endif // HAVE_VIRTUOSEAPI_CUSTOM