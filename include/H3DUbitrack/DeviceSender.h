/*
 * UbitrackContext.h
 *
 *  Created on: 03/08/2010
 *      Author: mvl
 */

#ifndef DEVICESENDER_H_
#define DEVICESENDER_H_


#include <H3D/X3DNode.h>

#include <H3D/SFString.h>
#include <H3D/SFBool.h>

#include <H3DUbitrack/H3DUbitrack.h>
#include <H3DUbitrack/MeasurementSender.h>
#include <H3DUbitrack/DeviceSenderForceEffect.h>

#include <vector>

using namespace Ubitrack::Facade;

namespace H3DUbitrack {

class H3DUBITRACK_API DeviceSender : public MeasurementSenderBase {

public:
    DeviceSender( H3D::Inst< H3D::SFNode     > _metadata = 0,
                    H3D::Inst< H3D::SFString   > _pattern = 0,
                    H3D::Inst< H3D::SFString    > _jointAnglesPattern = 0,
                    H3D::Inst< H3D::SFString    > _gimbalAnglesPattern = 0,
                    H3D::Inst< H3D::SFBool    > _isActive = 0
                    );

    // pattern names for sources to connect
    std::auto_ptr< H3D::SFString > jointAnglesPattern;
    std::auto_ptr< H3D::SFString > gimbalAnglesPattern;

    // activate transmission
    std::auto_ptr< H3D::SFBool > isActive;

    /// Add this node to the H3DNodeDatabase system.
    static H3D::H3DNodeDatabase database;

    Ubitrack::Measurement::Pose getMeasurement(H3D::TraverseInfo &ti, unsigned long long ts);

	virtual void update( H3D::TraverseInfo &ti, unsigned long long ts) {
		if (!connected)
			return;

/*		// call traverseSG from the ForceEffect here
		// check if field is dirty here !!
		if(dirty && (push_sender_pose != NULL)) {
			try {
				push_sender->send(getMeasurement(ti, ts));
				// clear dirty flag
				reset();
			} catch (Ubitrack::Util::Exception &e) {
				H3D::Console(4) << "Error while pushing measurement: " << pattern->getValue(id) << ": " << e.what() << std::endl;
			}
		}
*/
	}

	virtual bool connect(FacadePtr sf) {
	    if (!sf)
	        return false;

	    // check if already connected here !
		H3D::Console(4) << "Connect DeviceSender: " << pattern->getValue() << std::endl;

		// push sender pose
		try
		{
			push_sender_pose = sf->componentByName< Ubitrack::Components::ApplicationPushSourcePose >( pattern->getValue(id).c_str() ).get();
			connected = true;
		}
		catch ( const Ubitrack::Util::Exception& e )
		{
			H3D::Console(4) << "Caught exception in componentByName( " << pattern->getValue(id).c_str() <<" ): " << e  << std::endl;
			connected = false;
		}
		if (jointAnglesPattern->getValue(id) != "") {
			// push sender jointAngles (optional)
			try
			{
				push_sender_jointAngles = sf->componentByName< Ubitrack::Components::ApplicationPushSourcePosition >( jointAnglesPattern->getValue(id).c_str() ).get();
			}
			catch ( const Ubitrack::Util::Exception& e )
			{
				H3D::Console(4) << "Caught exception in componentByName( " << jointAnglesPattern->getValue(id).c_str() <<" ): " << e  << std::endl;
			}
		}
		if (gimbalAnglesPattern->getValue(id) != "") {
			// push sender gimbalAngles (optional)
			try
			{
				push_sender_gimbalAngles = sf->componentByName< Ubitrack::Components::ApplicationPushSourcePosition >( gimbalAnglesPattern->getValue(id).c_str() ).get();
			}
			catch ( const Ubitrack::Util::Exception& e )
			{
				H3D::Console(4) << "Caught exception in componentByName( " << gimbalAnglesPattern->getValue(id).c_str() <<" ): " << e  << std::endl;
			}
		}


		return connected;
	}

	virtual bool disconnect(FacadePtr sf) {
	    if (!sf)
	        return false;

	    H3D::Console(4) << "Disconnect DeviceSender: " << pattern->getValue() << std::endl;

		// pull receiver
		push_sender_pose = NULL;
		push_sender_jointAngles = NULL;
		push_sender_gimbalAngles = NULL;
		return true;
	}

protected:

	Ubitrack::Components::ApplicationPushSourcePose* push_sender_pose;
	Ubitrack::Components::ApplicationPushSourcePosition* push_sender_jointAngles;
	Ubitrack::Components::ApplicationPushSourcePosition* push_sender_gimbalAngles;

    std::auto_ptr<DeviceSenderForceEffect> forceeffect;
};

} // namespace H3DUbitrack

#endif //DEVICESENDER_H_
