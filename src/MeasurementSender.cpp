#include "H3DUbitrack/MeasurementSender.h"

using namespace std;
using namespace H3D;
using namespace H3DUbitrack;

// Add the nodes to the H3DNodeDatabase system.
H3DNodeDatabase MeasurementSenderBase::database(
                "MeasurementSenderBase",
                &(newInstance<MeasurementSenderBase>),
                typeid( MeasurementSenderBase ),
                NULL );


namespace MeasurementSenderInternals {
	FIELDDB_ELEMENT( MeasurementSenderBase, pattern, INITIALIZE_ONLY );
}


MeasurementSenderBase::MeasurementSenderBase(
	    	H3D::Inst< H3D::SFNode     > _metadata,
	        H3D::Inst< H3D::SFString   > _pattern
			)
: UbitrackMeasurement(_metadata, _pattern)
, connected(false)
{
};
