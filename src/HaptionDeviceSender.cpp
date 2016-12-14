#include <H3DUbitrack/H3DUbitrack.h>

#ifdef HAVE_VIRTUOSEAPI_CUSTOM

#include <H3DUbitrack/HaptionDeviceSender.h>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

using namespace H3D;
using namespace H3DUbitrack;

// Add the nodes to the H3DNodeDatabase system.
H3DNodeDatabase HaptionDeviceSender::database(
                "HaptionDeviceSender",
                &(newInstance<HaptionDeviceSender>),
                typeid( HaptionDeviceSender ),
                &MeasurementSenderBase::database );


namespace HaptionDeviceSenderInternals {
    // DeviceSender
	FIELDDB_ELEMENT( HaptionDeviceSender, deviceIndex, INPUT_OUTPUT );
	FIELDDB_ELEMENT( HaptionDeviceSender, frequency, INPUT_OUTPUT );
    FIELDDB_ELEMENT( HaptionDeviceSender, platformSensorsPattern, INITIALIZE_ONLY );
    FIELDDB_ELEMENT( HaptionDeviceSender, jointSensorsPattern, INITIALIZE_ONLY );
    FIELDDB_ELEMENT( HaptionDeviceSender, gimbalSensorsPattern, INITIALIZE_ONLY );
    FIELDDB_ELEMENT( HaptionDeviceSender, isActive, INPUT_OUTPUT );
}


HaptionDeviceSender::HaptionDeviceSender(H3D::Inst< H3D::SFNode > _metadata,
                           H3D::Inst< H3D::SFString   > _pattern,
                           H3D::Inst< H3D::SFString   > _platformSensorsPattern,
                           H3D::Inst< H3D::SFString   > _jointSensorsPattern,
                           H3D::Inst< H3D::SFString   > _gimbalSensorsPattern,
                           H3D::Inst< H3D::SFBool     > _isActive,
                           H3D::Inst< H3D::SFInt32    > _deviceIndex,
                           H3D::Inst< H3D::SFInt32    > _frequency
                           )
: MeasurementSenderBase(_metadata, _pattern )
, deviceIndex(_deviceIndex)
, platformSensorsPattern(_platformSensorsPattern)
, jointSensorsPattern(_jointSensorsPattern)
, gimbalSensorsPattern(_gimbalSensorsPattern)
, isActive(_isActive)
, frequency(_frequency)
, connected_platformSensors(false)
, connected_jointSensors(false)
, connected_gimbalSensors(false)
, push_sender_pose(NULL)
, push_sender_platformSensors(NULL)
, push_sender_jointSensors(NULL)
, push_sender_gimbalSensors(NULL)
, forceeffect(NULL)
{
    type_name = "HaptionDeviceSender";
    database.initFields( this );

    deviceIndex->setValue(0, id);
    deviceIndex->routeNoEvent(hasChanges, id);

	frequency->setValue( 100 , id );
	frequency->route( hasChanges, id );
	
	// active is set directly on the effect, no need to 
	// route into hasChanges
    isActive->setValue(false, id);

}


void HaptionDeviceSender::update( H3D::TraverseInfo &ti, unsigned long long ts) {

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

bool HaptionDeviceSender::connect(FacadePtr sf) {
    if (!sf)
        return false;

    // check if already connected here !
	H3D::Console(4) << "Connect HaptionDeviceSender: " << pattern->getValue() << std::endl;

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
	if (platformSensorsPattern->getValue(id) != "") {
		// push sender platformSensor (optional)
		try
		{
			push_sender_platformSensors = sf->componentByName< Ubitrack::Components::ApplicationPushSourcePosition >( platformSensorsPattern->getValue(id).c_str() ).get();
			connected_platformSensors = true;
		}
		catch ( const Ubitrack::Util::Exception& e )
		{
			H3D::Console(4) << "Caught exception in componentByName( " << platformSensorsPattern->getValue(id).c_str() <<" ): " << e  << std::endl;
			connected_platformSensors = false;
		}
	}
	if (jointSensorsPattern->getValue(id) != "") {
		// push sender jointSensors (optional)
		try
		{
			push_sender_jointSensors = sf->componentByName< Ubitrack::Components::ApplicationPushSourcePosition >( jointSensorsPattern->getValue(id).c_str() ).get();
			connected_jointSensors = true;
		}
		catch ( const Ubitrack::Util::Exception& e )
		{
			H3D::Console(4) << "Caught exception in componentByName( " << jointSensorsPattern->getValue(id).c_str() <<" ): " << e  << std::endl;
			connected_jointSensors = false;
		}
	}
	if (gimbalSensorsPattern->getValue(id) != "") {
		// push sender gimbalSensors (optional)
		try
		{
			push_sender_gimbalSensors = sf->componentByName< Ubitrack::Components::ApplicationPushSourcePosition >( gimbalSensorsPattern->getValue(id).c_str() ).get();
			connected_gimbalSensors = true;
		}
		catch ( const Ubitrack::Util::Exception& e )
		{
			H3D::Console(4) << "Caught exception in componentByName( " << gimbalSensorsPattern->getValue(id).c_str() <<" ): " << e  << std::endl;
			connected_gimbalSensors = false;
		}
	}

	return connected;
}

bool HaptionDeviceSender::disconnect(FacadePtr sf) {
    if (!sf)
        return false;

    H3D::Console(4) << "Disconnect HaptionDeviceSender: " << pattern->getValue() << std::endl;

	// pull receiver
	push_sender_pose = NULL;
	push_sender_platformSensors = NULL;
	push_sender_jointSensors = NULL;
	push_sender_gimbalSensors = NULL;
	connected = false;
	connected_platformSensors = false;
	connected_jointSensors = false;
	connected_gimbalSensors = false;

	return true;
}

void HaptionDeviceSender::updateValuesFromDevice( int i ) {
	forceeffect->setActive(isActive->getValue(id));
}

void HaptionDeviceSender::createHapticForceEffect(int index) {
	if (index < 0) {
		Console(3) << "Warning: Invalid index " << index
				<< " in deviceIndex field in node " << getName()
				<< ". Index is ignored." << endl;
		return;
	}
	// Create an instance of the DeviceSenderHapticForceEffect class.
	forceeffect.reset(new HaptionDeviceSenderHapticForceEffect(
			push_sender_pose,
			push_sender_platformSensors,
			push_sender_jointSensors,
			push_sender_gimbalSensors,
			isActive->getValue(id)
		)
	);
}

#endif // HAVE_VIRTUOSEAPI_CUSTOM