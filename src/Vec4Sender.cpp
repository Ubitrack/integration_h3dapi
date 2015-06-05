#include <H3DUbitrack/Vec4Sender.h>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

using namespace H3D;
using namespace H3DUbitrack;


// Add the nodes to the H3DNodeDatabase system.
H3DNodeDatabase Vec4Sender::database(
                "Vec4Sender",
                &(newInstance<Vec4Sender>),
                typeid( Vec4Sender ),
                &MeasurementSenderBase::database );


namespace Vec4SenderInternals {
    // Vec4Sender
    FIELDDB_ELEMENT( Vec4Sender, value, INPUT_ONLY );
}


Vec4Sender::Vec4Sender(H3D::Inst< H3D::SFNode > _metadata,
                           H3D::Inst< H3D::SFString   > _pattern,
                           H3D::Inst< H3D::SFVec4d > _value
                           )
: Vec4SenderBase(_metadata, _pattern )
, value(_value)
{
    type_name = "Vec4Sender";
    database.initFields( this );

    value->setValue(Vec4d(0., 0., 0., 0.));
    value->routeNoEvent(hasChanges, id);
}

Ubitrack::Measurement::Vector4D Vec4Sender::getMeasurement(TraverseInfo &ti, unsigned long long ts)
{
	boost::lock_guard<boost::mutex> lock(data_lock);
	Vec4d v = value->getValue(id);
	return Ubitrack::Measurement::Vector4D(ts, Ubitrack::Math::Vector4d(v.x, v.y, v.z, v.w) );
}

