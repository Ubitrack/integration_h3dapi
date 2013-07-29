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


#include <H3DUtil/ReadWriteH3DTypes.h>
#include <H3DUtil/Console.h>

using namespace H3DUbitrack;
using namespace HAPI;

DeviceSenderHapticForceEffect::DeviceSenderHapticForceEffect(const HAPI::Vec3 &_holeOrigin, const HAPI::Vec3 &_holeTarget,
	HAPIFloat _radius, HAPIFloat _torqueSpringConstant, HAPIFloat _maxAngleDiff, 
	HAPIFloat _lockPositionSpring, HAPIFloat _lockPositionDamping) :
	holeOrigin(_holeOrigin), holeTarget(_holeTarget), 
	radius(_radius), torqueSpringConstant(_torqueSpringConstant),
	maxAngleDiff(_maxAngleDiff), 
	lockPositionSpring(_lockPositionSpring), lockPositionDamping(_lockPositionDamping),
	force(Vec3(0, 0, 0)), torque(Vec3(0, 0, 0)), max_force(Vec3(0, 0, 0)), max_torque(Vec3(0, 0, 0)), 
	lock_position(Vec3(0,0,0)), is_locked(false), position_error(0), orientation_error(0), 
	rotOK(false), rotationDiff(Rotation(Vec3(1,0,0), 0)), unlock_ts(0.0), enable_debuglog(false)
{
	if(enable_debuglog) {
		ios_base::openmode mode = ios::out | ios::trunc;
		log_file.open( "debug_effect.log", mode );
		if( !log_file.is_open() )
			H3DUtil::Console(3) << "The debug_effect.log file could not be opened.";

		if( log_file.is_open() ) {
			log_file << "TIMESTAMP";
			log_file << " ";
			log_file << "FORCE";
			log_file << " ";
			log_file << "TORQUE";
			log_file << " ";
			log_file << "POSITIONERROR";
			log_file << " ";
			log_file << "ORIENTATIONERROR";
			log_file << " ";
			log_file << "SLIDEIN";
			log_file << " ";
			log_file << "ISLOCKED";
			log_file << " ";
			log_file << "LOCKPOSITION";
			log_file << " ";
			log_file << "UNLOCKTS";
			log_file << std::endl;
		}
	}

}

DeviceSenderHapticForceEffect::~DeviceSenderHapticForceEffect() {
  if( log_file.is_open() )
    log_file.close();	
}

HAPIForceEffect::EffectOutput DeviceSenderHapticForceEffect::calculateForces( const HAPIForceEffect::EffectInput &input) {

	const Vec3 pos = input.hd->getPosition();
	const Vec3 vel = input.hd->getVelocity();
	const Rotation orn = input.hd->getOrientation();
	const Matrix3 hipMat = Matrix3(Quaternion(orn));
	const Matrix3 hipMatInv = hipMat.inverse();

	const Vec3 hole_vec = holeOrigin - holeTarget;
    const Vec3 hip_vec_device = Vec3(0,0,1);
    const Vec3 hip_vec = hipMat * hip_vec_device;

	position_error = -1.0;
	orientation_error = -1.0;

	force = Vec3(0, 0, 0);
	torque = Vec3(0, 0, 0);

	if (hole_vec.length() > 0.0) {
		// check if pos is within the cylinder
		// based on http://www.cs.cf.ac.uk/Dave/CM0268/PDF/12_CM0268_Geometric_Computing2.pdf
		const HAPIFloat x0 = holeOrigin.x;
		const HAPIFloat y0 = holeOrigin.y;
		const HAPIFloat z0 = holeOrigin.z;
		const HAPIFloat f = hole_vec.x;
		const HAPIFloat g = hole_vec.y;
		const HAPIFloat h = hole_vec.z;
		const HAPIFloat t = (f*(x0 - pos.x) + g*(y0 - pos.y) + h*(z0 - pos.z))/(f*f + g*g + h*h);

		//// p = point on line projected from pos, dist is distance to pos from line
		const Vec3 p(x0-f*t, y0-g*t, z0-h*t);
		const HAPIFloat dist = (pos - p).length();
	
		Vec3 hole_vec_norm = hole_vec;
		hole_vec_norm.normalizeSafe();
		Vec3 hip_vec_norm = hip_vec;
		hip_vec_norm.normalizeSafe();

		//// check if within bounds
		bool peg_in_hole = false;
		// goal == 1.1 to leave a bit room for inprecise positioning ...
		// needs to be improved to avoid artifacts on the entry
		if ((t >= 0.0+radius) && (t <= 1.1+radius) && (dist <= radius))
			peg_in_hole = true;

		double slide_in = 1.0 - max(0.0, min(1.0, pow((1.0 - t),5)));

		if (peg_in_hole) {

			// calculate rotation difference and torque in device space
			Rotation rotDiff( hip_vec_norm, hole_vec_norm );
			
			Vec3 euler_angles = hipMat * ( Matrix3(1,0,0, 0,1,0, 0,0,0) * (hipMatInv * rotDiff.toEulerAngles()) );
			torque = slide_in*(euler_angles * torqueSpringConstant) /* - damping * input.hd->getDeviceValues().angular_velocity)*/;

			// save for retrieval from scene graph
			//rotationDiff = Rotation(euler_angles);
			rotationDiff = rotDiff;

			position_error = 1.0 - t;
			if (position_error < 0.0)
				position_error = 0.0;

			rotOK = false;

			// calculate the angle difference
			const HAPIFloat angleDiff = acos(hip_vec_norm.dotProduct(hole_vec_norm));
			orientation_error = fabs(2.0*angleDiff/H3DUtil::Constants::pi);

			if (angleDiff <= maxAngleDiff) {
					rotOK = true;
			}

			if(!rotOK) {
				if (!is_locked) {
					lock_position = pos;
					is_locked = true;
				}
				// needs 0.05 seconds of correct orientation .. therefore reset anytime 
				// the rotation is not ok ..
				unlock_ts = H3DUtil::TimeStamp(0.0);
			} else {
				// if more than 0.05 secs are ok .. and it's currently locked, 
				// then release lock
				if ( is_locked &&  ((H3DUtil::TimeStamp::now()-unlock_ts) > 0.1)) {
					is_locked = false;
					lock_position = Vec3(0,0,0);
					unlock_ts = H3DUtil::TimeStamp(0.0);
				} else if ((double)unlock_ts == 0.0) {
					// 
					unlock_ts = H3DUtil::TimeStamp::now();
				}
			}
		} else if (dist < radius) {
			is_locked = false;
			lock_position = Vec3(0,0,0);
		}

		if (is_locked && lockingEnabled) {
			// lock stylus here using a spring-damper system
			force = (lockPositionSpring * (lock_position - pos) - lockPositionDamping * vel ) * ((1.0 - position_error) + orientation_error);
		}
		if(enable_debuglog) {
			if( log_file.is_open() ) {
				log_file << H3DUtil::TimeStamp::now();
				log_file << " ";
				log_file << force;
				log_file << " ";
				log_file << torque;
				log_file << " ";
				log_file << position_error;
				log_file << " ";
				log_file << orientation_error;
				log_file << " ";
				log_file << slide_in;
				log_file << " ";
				log_file << is_locked;
				log_file << " ";
				log_file << lock_position;
				log_file << " ";
				log_file << unlock_ts;
				log_file << std::endl;
			}
		}
	}


	if (force.lengthSqr() >= max_force.lengthSqr())
		max_force = force;

	if (torque.lengthSqr() >= max_torque.lengthSqr())
		max_torque = torque;

	if (torqueFeedbackEnabled)
		return HAPIForceEffect::EffectOutput(force, torque);
	else
		return HAPIForceEffect::EffectOutput(force);
}
