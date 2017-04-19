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

#include <H3DUbitrack/DeviceSenderHapticForceEffect.h>

#include <H3DUbitrack/H3DUbitrack.h>
#include <H3DUbitrack/MeasurementSender.h>
#include <HAPI/HAPIHapticsDevice.h>

#include <H3DUtil/ReadWriteH3DTypes.h>
#include <H3DUtil/Console.h>

using namespace H3DUbitrack;
using namespace HAPI;

DeviceSenderHapticForceEffect::DeviceSenderHapticForceEffect(
		Ubitrack::Components::ApplicationPushSourcePose* _push_source_pose,
		bool _is_active, int _freq)
: push_source_pose(_push_source_pose)
, is_active(_is_active)
{
  last_time = -1;
  time_diff = 1.0/_freq;
}

DeviceSenderHapticForceEffect::~DeviceSenderHapticForceEffect() {

}

HAPIForceEffect::EffectOutput DeviceSenderHapticForceEffect::calculateForces( const HAPIForceEffect::EffectInput &input) {
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

	m_lock.lock();
	unsigned long long tstamp = Ubitrack::Measurement::now();

	if (is_active && ready) {
		HAPI::HAPIHapticsDevice* hd = input.hd;

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
	}
	m_lock.unlock();
	// no return value
	return HAPIForceEffect::EffectOutput();
}
