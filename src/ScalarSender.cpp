#include <H3DUbitrack/ScalarSender.h>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

using namespace H3D;
using namespace H3DUbitrack;


// Add the nodes to the H3DNodeDatabase system.
H3DNodeDatabase ScalarSender::database(
                "ScalarSender",
                &(newInstance<ScalarSender>),
                typeid( ScalarSender ),
                &MeasurementSenderBase::database );


namespace ScalarSenderInternals {
    // ScalarSender
    FIELDDB_ELEMENT( ScalarSender, value, INPUT_ONLY );
}


ScalarSender::ScalarSender(H3D::Inst< H3D::SFNode > _metadata,
                           H3D::Inst< H3D::SFString   > _pattern,
                           H3D::Inst< H3D::SFDouble > _value
                           )
: ScalarSenderBase(_metadata, _pattern )
, value(_value)
{
    type_name = "ScalarSender";
    database.initFields( this );

    value->setValue(0.);
    value->routeNoEvent(hasChanges, id);
}

Ubitrack::Measurement::Distance ScalarSender::getMeasurement(TraverseInfo &ti, unsigned long long ts)
{
	boost::lock_guard<boost::mutex> lock(data_lock);
	return Ubitrack::Measurement::Distance(ts, value->getValue(id));
}

