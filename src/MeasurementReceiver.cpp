#include "H3DUbitrack/MeasurementReceiver.h"

using namespace std;
using namespace H3D;
using namespace H3DUbitrack;

// Add the nodes to the H3DNodeDatabase system.
H3DNodeDatabase MeasurementReceiverBase::database(
                "MeasurementReceiverBase",
                &(newInstance<MeasurementReceiverBase>),
                typeid( MeasurementReceiverBase ),
                NULL );


namespace MeasurementReceiverInternals {
    // PoseReceiver
	FIELDDB_ELEMENT( MeasurementReceiverBase, mode, INITIALIZE_ONLY );
	FIELDDB_ELEMENT( MeasurementReceiverBase, pattern, INITIALIZE_ONLY );
	FIELDDB_ELEMENT( MeasurementReceiverBase, isActive, INPUT_OUTPUT );
	FIELDDB_ELEMENT( MeasurementReceiverBase, isSyncSource, INITIALIZE_ONLY );
	FIELDDB_ELEMENT( MeasurementReceiverBase, isDataAvailable, OUTPUT_ONLY );
}


MeasurementReceiverBase::MeasurementReceiverBase(
	    	H3D::Inst< H3D::SFNode     > _metadata,
	        H3D::Inst< H3D::SFString   > _pattern,
	        H3D::Inst< H3D::SFBool     > _isActive,
	        H3D::Inst< H3D::SFBool     > _isSyncSource,
	        H3D::Inst< H3D::SFBool     > _isDataAvailable,
			H3D::Inst< MeasurementMode > _mode
			)
: UbitrackMeasurement(_metadata, _pattern)
, isSyncSource(_isSyncSource)
, isActive(_isActive)
, isDataAvailable(_isDataAvailable)
, data_ready(false)
, connected(false)
, last_timestamp(0)
, mode(_mode)
{
	isSyncSource->setValue(false, id );
	isActive->setValue(true, id );
	mode->addValidValue( "PULL" );
	mode->addValidValue( "PUSH" );
	mode->setValue("PUSH");
};
