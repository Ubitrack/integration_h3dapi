#include <H3DUbitrack/DistanceSender.h>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

using namespace H3D;
using namespace H3DUbitrack;


// Add the nodes to the H3DNodeDatabase system.
H3DNodeDatabase DistanceSender::database(
                "DistanceSender",
                &(newInstance<DistanceSender>),
                typeid( DistanceSender ),
                &MeasurementSenderBase::database );


namespace DistanceSenderInternals {
    // DistanceSender
    FIELDDB_ELEMENT( DistanceSender, value, INPUT_ONLY );
}


DistanceSender::DistanceSender(H3D::Inst< H3D::SFNode > _metadata,
                           H3D::Inst< H3D::SFString   > _pattern,
                           H3D::Inst< H3D::SFDouble > _value
                           )
: DistanceSenderBase(_metadata, _pattern )
, value(_value)
{
    type_name = "DistanceSender";
    database.initFields( this );

    value->setValue(0.);
    value->routeNoEvent(hasChanges, id);
}

Ubitrack::Measurement::Distance DistanceSender::getMeasurement(TraverseInfo &ti, unsigned long long ts)
{
	boost::lock_guard<boost::mutex> lock(data_lock);
	return Ubitrack::Measurement::Distance(ts, value->getValue(id));
}

