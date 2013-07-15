#include "H3DUbitrack/UbitrackMeasurement.h"

using namespace std;
using namespace H3D;
using namespace H3DUbitrack;

// Add the nodes to the H3DNodeDatabase system.
H3DNodeDatabase UbitrackMeasurement::database(
                "UbitrackMeasurement",
                &(newInstance<UbitrackMeasurement>),
                typeid( UbitrackMeasurement ),
                NULL );


namespace UbitrackMeasurementInternals {
    // PoseReceiver
	FIELDDB_ELEMENT( UbitrackMeasurement, mode, INITIALIZE_ONLY );
	FIELDDB_ELEMENT( UbitrackMeasurement, pattern, INITIALIZE_ONLY );
	FIELDDB_ELEMENT( UbitrackMeasurement, isSyncSource, INITIALIZE_ONLY );
}


UbitrackMeasurement::MeasurementMode::Mode UbitrackMeasurement::MeasurementMode::getMeasurementMode() {
	upToDate();
	if( value == "PULL" )
		return PULL;
	else if( value == "PUSH" )
		return PUSH;
	else {
		stringstream s;
		s << "Must be one of "
		<< "PULL, "
		<< "PUSH ";
		throw InvalidMeasurementMode( value,
								s.str(),
								H3D_FULL_LOCATION );
	}
}


UbitrackMeasurement::UbitrackMeasurement(
	H3D::Inst< H3D::SFNode     > _metadata,
	H3D::Inst< H3D::SFString   > _pattern,
	H3D::Inst< H3D::SFBool     > _isSyncSource,
	H3D::Inst< MeasurementMode > _mode
	): H3D::X3DNode(_metadata)
, pattern(_pattern)
, mode(_mode)
, isSyncSource(_isSyncSource)
, connected(false)
, receiver(NULL)
{

	mode->addValidValue( "PULL" );
	mode->addValidValue( "PUSH" );
	mode->setValue("PUSH");

	isSyncSource->setValue(false, id );
}


bool UbitrackMeasurement::connect(Ubitrack::Facade::AdvancedFacade* facade)
{
    if (receiver == NULL) {
        initializeReceiver();
	}
	// check if already connected here !
	Console(4) << "Connect Receiver: " << pattern->getValue() << std::endl;
	connected = receiver->connect(facade);
	return connected;
}

bool UbitrackMeasurement::disconnect(Ubitrack::Facade::AdvancedFacade* facade)
{
    if (!connected)
        return true;

    if (receiver != NULL) {
	    Console(4) << "Disconnect Receiver: " << pattern->getValue() << std::endl;
		receiver->disconnect(facade);
		connected = false;
		receiver = NULL;
		return true;
	}
	return false;
}

void UbitrackMeasurement::update(unsigned long long int ts)
{
	if (!connected)
		return;
    if (receiver != NULL) {
    	receiver->update(ts);
	}
}

unsigned long long int UbitrackMeasurement::wait_for_data_ready() {
	if (receiver != NULL) {
		return receiver->wait_for_data_ready();
	}
	return 0;
}
