#include <H3DUbitrack/DeviceSenderForceEffect.h>

#include <H3D/Scene.h>

#include <sstream>
#include <iostream>
#include <istream>

using namespace H3DUbitrack;
using namespace H3D;

// Add this node to the H3DNodeDatabase system.
H3DNodeDatabase DeviceSenderForceEffect::database("DeviceSenderForceEffect",
		&(newInstance<DeviceSenderForceEffect> ), typeid(DeviceSenderForceEffect),
		&H3DForceEffect::database);

namespace DeviceSenderForceEffectInternals {
FIELDDB_ELEMENT( DeviceSenderForceEffect, holeOrigin, INPUT_OUTPUT);
FIELDDB_ELEMENT( DeviceSenderForceEffect, holeTarget, INPUT_OUTPUT);
FIELDDB_ELEMENT( DeviceSenderForceEffect, torque, OUTPUT_ONLY);
FIELDDB_ELEMENT( DeviceSenderForceEffect, force, OUTPUT_ONLY);
FIELDDB_ELEMENT( DeviceSenderForceEffect, maxTorque, OUTPUT_ONLY);
FIELDDB_ELEMENT( DeviceSenderForceEffect, maxForce, OUTPUT_ONLY);
FIELDDB_ELEMENT( DeviceSenderForceEffect, positionError, OUTPUT_ONLY);
FIELDDB_ELEMENT( DeviceSenderForceEffect, orientationError, OUTPUT_ONLY);
FIELDDB_ELEMENT( DeviceSenderForceEffect, rotOK, OUTPUT_ONLY);
FIELDDB_ELEMENT( DeviceSenderForceEffect, rotationDiff, OUTPUT_ONLY);
FIELDDB_ELEMENT( DeviceSenderForceEffect, radius, INPUT_OUTPUT);
FIELDDB_ELEMENT( DeviceSenderForceEffect, torqueSpringConstant, INPUT_OUTPUT);
FIELDDB_ELEMENT( DeviceSenderForceEffect, maxAngleDiff, INPUT_OUTPUT);
FIELDDB_ELEMENT( DeviceSenderForceEffect, lockPositionSpring, INPUT_OUTPUT);
FIELDDB_ELEMENT( DeviceSenderForceEffect, lockPositionDamping, INPUT_OUTPUT);
FIELDDB_ELEMENT( DeviceSenderForceEffect, enableTorqueFeedback, INPUT_OUTPUT);
FIELDDB_ELEMENT( DeviceSenderForceEffect, enableLocking, INPUT_OUTPUT);
}

DeviceSenderForceEffect::DeviceSenderForceEffect(Inst<SFNode> _metadata,
		Inst<SFVec3f> _holeOrigin,
		Inst<SFVec3f> _holeTarget, 
        Inst< MFVec3f > _torque,
        Inst< MFVec3f > _force,
        Inst< MFVec3f > _maxTorque,
        Inst< MFVec3f > _maxForce,
		Inst< SFFloat > _positionError,
		Inst< SFFloat > _orientationError,
		Inst< SFBool  > _rotOK,
		Inst< SFRotation > _rotationDiff,
		Inst< SFFloat > _radius,
		Inst< SFFloat > _torqueSpringConstant,
		Inst< SFFloat > _maxAngleDiff,
		Inst< SFFloat > _lockPositionSpring,
		Inst< SFFloat > _lockPositionDamping,
		Inst< SFBool  > _enableTorqueFeedback,
		Inst< SFBool  > _enableLocking,
		Inst<MFInt32> _deviceIndex) :
		H3DForceEffect(_metadata, _deviceIndex), holeOrigin(_holeOrigin),
				holeTarget(_holeTarget), torque(_torque), force(_force), maxTorque(_maxTorque), maxForce(_maxForce), 
				positionError(_positionError), orientationError(_orientationError), rotOK(_rotOK),
				rotationDiff(_rotationDiff), radius(_radius), torqueSpringConstant(_torqueSpringConstant), 
				maxAngleDiff(_maxAngleDiff), lockPositionSpring(_lockPositionSpring),
				lockPositionDamping(_lockPositionDamping), 
				enableTorqueFeedback(_enableTorqueFeedback), enableLocking(_enableLocking), 
				last_enabled(false), updateHapticForceEffect(new Field) {

	type_name = "DeviceSenderForceEffect";
	database.initFields(this);

	holeOrigin->route( updateHapticForceEffect );
	holeTarget->route( updateHapticForceEffect );
	radius->route( updateHapticForceEffect );
	torqueSpringConstant->route(updateHapticForceEffect );
	maxAngleDiff->route( updateHapticForceEffect );
	lockPositionSpring->route( updateHapticForceEffect );
	lockPositionDamping->route( updateHapticForceEffect );
	deviceIndex->route( updateHapticForceEffect );


	holeOrigin->setValue(Vec3f(0,0,0), id);
	holeTarget->setValue(Vec3f(0,0,0), id);
	radius->setValue(0, id);

	torqueSpringConstant->setValue( 0.05, id );
	maxAngleDiff->setValue( 0.18, id );
	lockPositionSpring->setValue( 50.0, id );
	lockPositionDamping->setValue( 1.0, id );

	enableTorqueFeedback->setValue( true, id );
	enableLocking->setValue( true, id );


}

DeviceSenderForceEffect::~DeviceSenderForceEffect() {
}

void DeviceSenderForceEffect::initialize() {
	// initialize
}

void DeviceSenderForceEffect::traverseSG(TraverseInfo &ti) {

	const vector<H3DInt32> &device_index = deviceIndex->getValue();
	if (!updateHapticForceEffect->isUpToDate()) {
		updateHapticForceEffect->upToDate();
		if (device_index.empty()) {
			for (unsigned int i = 0; i < ti.getHapticsDevices().size(); i++) {
				createHapticForceEffect(i);
			}
		} else {
			for (unsigned int i = 0; i < device_index.size(); i++) {
				createHapticForceEffect(device_index[i]);
			}
		}
	}
	// Add the instance of DeviceSenderHapticForceEffect to be rendered on the selected
	// haptics device.
	if (!ti.getHapticsDevices().empty()) {
		if (device_index.empty()) {
			for (unsigned int i = 0; i < ti.getHapticsDevices().size(); i++) {
				if (ti.hapticsEnabled(i)) {
					ti.addForceEffect(i, haptic_force_effect[i]);
				}
				updateValuesFromDevice( i );

			}
		} else {
			for (unsigned int i = 0; i < device_index.size(); i++) {
				if (device_index[i] >= 0 && ti.hapticsEnabled(device_index[i])
						&& haptic_force_effect[device_index[i]]) {
					ti.addForceEffect(device_index[i],
							haptic_force_effect[device_index[i]]);

				}

				updateValuesFromDevice( device_index[i] );
			}
		}
	}
}

void DeviceSenderForceEffect::updateValuesFromDevice( int i ) {
	if( i >= (int)torque->size() )
		torque->resize( i + 1, Vec3f( 0, 0, 0 ), id );
	Vec3f f = (Vec3f) haptic_force_effect[i]->getLatestTorque();
	torque->setValue( i, f, id );

	if( i >= (int)force->size() )
		force->resize( i + 1, Vec3f( 0, 0, 0 ), id );
	f = (Vec3f) haptic_force_effect[i]->getLatestForce();
	force->setValue( i, f, id );

	if( i >= (int)maxTorque->size() )
		maxTorque->resize( i + 1, Vec3f( 0, 0, 0 ), id );
	f = (Vec3f) haptic_force_effect[i]->getMaxTorque();
	maxTorque->setValue( i, f, id );

	if( i >= (int)maxForce->size() )
		maxForce->resize( i + 1, Vec3f( 0, 0, 0 ), id );
	f = (Vec3f) haptic_force_effect[i]->getMaxForce();
	maxForce->setValue( i, f, id );

	positionError->setValue( haptic_force_effect[i]->getPositionError(), id );
	orientationError->setValue( haptic_force_effect[i]->getOrientationError(), id );
	rotOK->setValue( haptic_force_effect[i]->getRotOK(), id );
	rotationDiff->setValue( Rotation(haptic_force_effect[i]->getRotationDiff()), id );

	haptic_force_effect[i]->setTorqueFeedbackEnabled( enableTorqueFeedback->getValue( id ) );
	haptic_force_effect[i]->setLockingEnabled( enableLocking->getValue( id ) );

}

void DeviceSenderForceEffect::createHapticForceEffect(int index) {
	if (index < 0) {
		Console(3) << "Warning: Invalid index " << index
				<< " in deviceIndex field in node " << getName()
				<< ". Index is ignored." << endl;
		return;
	}
	if (index >= (int) haptic_force_effect.size()) {
		haptic_force_effect.resize(index + 1, NULL);
	}

	// Create an instance of the TORQUEVIZDeviceLogEffect class.
	haptic_force_effect.set(index, 
		new DeviceSenderHapticForceEffect(
			holeOrigin->getValue(id), holeTarget->getValue(id), radius->getValue(id), torqueSpringConstant->getValue(id),
			maxAngleDiff->getValue(id), lockPositionSpring->getValue(id), lockPositionDamping->getValue(id)
		));
}
