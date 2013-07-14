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
, last_timestamp(0)
, data_ready()
{

	mode->addValidValue( "PULL" );
	mode->addValidValue( "PUSH" );
	mode->setValue("PUSH");

	isSyncSource->setValue(false, id );
}

void UbitrackMeasurement::notify_data_ready(unsigned long long int ts) {
	data_ready.lock();
	last_timestamp = ts;
	data_ready.signal();
	data_ready.unlock();
}


unsigned long long int UbitrackMeasurement::wait_for_data_ready() {
	data_ready.lock();
	data_ready.wait();
	data_ready.unlock();
	return last_timestamp;
}
