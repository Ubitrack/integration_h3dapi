#include <H3DUbitrack/DeviceSender.h>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

using namespace H3D;
using namespace H3DUbitrack;


// Add the nodes to the H3DNodeDatabase system.
H3DNodeDatabase DeviceSender::database(
                "DeviceSender",
                &(newInstance<DeviceSender>),
                typeid( DeviceSender ),
                &MeasurementSenderBase::database );


namespace DeviceSenderInternals {
    // DeviceSender
	FIELDDB_ELEMENT( DeviceSender, deviceIndex, INPUT_OUTPUT );
	FIELDDB_ELEMENT( DeviceSender, frequency, INPUT_OUTPUT );
    FIELDDB_ELEMENT( DeviceSender, isActive, INPUT_OUTPUT );
}


DeviceSender::DeviceSender(H3D::Inst< H3D::SFNode > _metadata,
                           H3D::Inst< H3D::SFString   > _pattern,
                           H3D::Inst< H3D::SFBool     > _isActive,
                           H3D::Inst< H3D::SFInt32    > _deviceIndex,
                           H3D::Inst< H3D::SFInt32    > _frequency
                           )
: MeasurementSenderBase(_metadata, _pattern )
, deviceIndex(_deviceIndex)
, isActive(_isActive)
, frequency(_frequency)
, push_sender_pose(NULL)
, forceeffect(NULL)
{
    type_name = "DeviceSender";
    database.initFields( this );

    deviceIndex->setValue(0, id);
    deviceIndex->routeNoEvent(hasChanges, id);

	frequency->setValue( 100 , id );
	frequency->route( hasChanges, id );
	
	// active is set directly on the effect, no need to 
	// route into hasChanges
    isActive->setValue(false);

}


void DeviceSender::update( H3D::TraverseInfo &ti, unsigned long long ts) {

	H3DInt32 device_index = deviceIndex->getValue();

	if (!hasChanges->isUpToDate()) {
		hasChanges->upToDate();
		if (device_index >= 0) {
			createHapticForceEffect(device_index);
		}
	}
	// Add the instance of DeviceSenderHapticForceEffect to be rendered on the selected
	// haptics device.
	if (!ti.getHapticsDevices().empty()) {
		if (device_index >= 0 && ti.hapticsEnabled(device_index)) {
			ti.addForceEffect(device_index, forceeffect.get());
		}
		updateValuesFromDevice( device_index );
	}

	if (!connected)
		return;

}

bool DeviceSender::connect(FacadePtr sf) {
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
	return connected;
}

bool DeviceSender::disconnect(FacadePtr sf) {
    if (!sf)
        return false;

    H3D::Console(4) << "Disconnect DeviceSender: " << pattern->getValue() << std::endl;

	// pull receiver
	push_sender_pose = NULL;
	connected = false;

	return true;
}

void DeviceSender::updateValuesFromDevice( int i ) {
	forceeffect->setActive(isActive->getValue(id));
}

void DeviceSender::createHapticForceEffect(int index) {
	if (index < 0) {
		Console(3) << "Warning: Invalid index " << index
				<< " in deviceIndex field in node " << getName()
				<< ". Index is ignored." << endl;
		return;
	}
	// Create an instance of the DeviceSenderHapticForceEffect class.
	forceeffect.reset(new DeviceSenderHapticForceEffect(
			push_sender_pose,
			isActive->getValue(id)
		)
	);
}
