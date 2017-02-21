#include <H3DUbitrack/DistanceReceiver.h>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

using namespace H3D;
using namespace H3DUbitrack;


// Add the nodes to the H3DNodeDatabase system.
H3DNodeDatabase DistanceReceiver::database(
                "DistanceReceiver",
                &(newInstance<DistanceReceiver>),
                typeid( DistanceReceiver ),
                &MeasurementReceiverBase::database );


namespace DistanceReceiverInternals {
    // DistanceReceiver
    FIELDDB_ELEMENT( DistanceReceiver, value, OUTPUT_ONLY );
}


DistanceReceiver::DistanceReceiver(H3D::Inst< H3D::SFNode > _metadata,
                           H3D::Inst< H3D::SFString   > _pattern,
                           H3D::Inst< H3D::SFBool     > _isActive,
                           H3D::Inst< H3D::SFBool     > _isSyncSource,
                           H3D::Inst< H3D::SFBool     > _isDataAvailable,
                           H3D::Inst< MeasurementMode > _mode,
                           H3D::Inst< H3D::SFDouble > _value
                           )
: DistanceReceiverBase(_metadata, _pattern, _isActive, _isSyncSource, _isDataAvailable, _mode)
, value(_value)
{
    type_name = "DistanceReceiver";
    database.initFields( this );
}

void DistanceReceiver::updateMeasurement(const Ubitrack::Measurement::Distance& m)
{
	boost::lock_guard<boost::mutex> lock(data_lock);
	double v = *(m.get());
    value->setValue( v , id );
}

