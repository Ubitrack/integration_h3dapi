/*
 * UbitrackContext.h
 *
 *  Created on: 03/08/2010
 *      Author: mvl
 */

#ifndef HAPTIONDEVICESENDER_H_
#define HAPTIONDEVICESENDER_H_


#include <H3D/X3DNode.h>

#include <H3D/SFString.h>
#include <H3D/SFBool.h>
#include <H3D/SFInt32.h>

#include <H3DUbitrack/H3DUbitrack.h>
#include <H3DUbitrack/MeasurementSender.h>
#include <H3DUbitrack/HaptionDeviceSenderHapticForceEffect.h>

#include <vector>

using namespace Ubitrack::Facade;

namespace H3DUbitrack {

class H3DUBITRACK_API HaptionDeviceSender : public MeasurementSenderBase {

public:
    HaptionDeviceSender( H3D::Inst< H3D::SFNode       > _metadata = 0,
                    H3D::Inst< H3D::SFString   > _pattern = 0,
                    H3D::Inst< H3D::SFString   > _platformSensorPattern = 0,
					H3D::Inst< H3D::SFString   > _jointSensorPattern = 0,
                    H3D::Inst< H3D::SFString   > _gimbalSensorPattern = 0,
                    H3D::Inst< H3D::SFBool     > _isActive = 0,
                    H3D::Inst< H3D::SFInt32    > _deviceIndex = 0,
					H3D::Inst< H3D::SFInt32    > _frequency = 0
                    );

    // the deviceIndex to be used
    std::auto_ptr< H3D::SFInt32 > deviceIndex;

    /// The log frequency.
    ///
    /// <b>Access type:</b> initializeOnly \n
    /// <b>Default value:</b> 100 \n
    auto_ptr< H3D::SFInt32 > frequency;

	// pattern names for sources to connect
    std::auto_ptr< H3D::SFString > platformSensorPattern;
    std::auto_ptr< H3D::SFString > jointSensorPattern;
    std::auto_ptr< H3D::SFString > gimbalSensorPattern;

    // activate transmission
    std::auto_ptr< H3D::SFBool > isActive;

    /// Add this node to the H3DNodeDatabase system.
    static H3D::H3DNodeDatabase database;

    Ubitrack::Measurement::Pose getMeasurement(H3D::TraverseInfo &ti, unsigned long long ts);

	virtual void update( H3D::TraverseInfo &ti, unsigned long long ts);

	virtual bool connect(FacadePtr sf);

	virtual bool disconnect(FacadePtr sf);

protected:

	/// Internal function used to create a haptic force effect for the
	/// haptics device of the given index.
	void createHapticForceEffect( int index );
	void updateValuesFromDevice( int index );


	Ubitrack::Components::ApplicationPushSourcePose* push_sender_pose;
    Ubitrack::Components::ApplicationPushSourcePosition* push_sender_platformSensor;
	Ubitrack::Components::ApplicationPushSourcePosition* push_sender_jointSensor;
	Ubitrack::Components::ApplicationPushSourcePosition* push_sender_gimbalSensor;

    std::auto_ptr<HaptionDeviceSenderHapticForceEffect> forceeffect;
    bool connected_platformSensors;
    bool connected_jointSensors;
    bool connected_gimbalSensors;
};

} // namespace H3DUbitrack

#endif //DEVICESENDER_H_
