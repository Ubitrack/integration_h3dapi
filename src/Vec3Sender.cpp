#include <H3DUbitrack/Vec3Sender.h>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

using namespace H3D;
using namespace H3DUbitrack;


// Add the nodes to the H3DNodeDatabase system.
H3DNodeDatabase Vec3Sender::database(
                "Vec3Sender",
                &(newInstance<Vec3Sender>),
                typeid( Vec3Sender ),
                &MeasurementSenderBase::database );


namespace Vec3SenderInternals {
    // Vec3Sender
    FIELDDB_ELEMENT( Vec3Sender, value, INPUT_ONLY );
}


Vec3Sender::Vec3Sender(H3D::Inst< H3D::SFNode > _metadata,
                           H3D::Inst< H3D::SFString   > _pattern,
                           H3D::Inst< H3D::SFVec3d > _value
                           )
: Vec3SenderBase(_metadata, _pattern )
, value(_value)
{
    type_name = "Vec3Sender";
    database.initFields( this );

    value->setValue(Vec3d(0., 0., 0.));
    value->routeNoEvent(hasChanges, id);
}

Ubitrack::Measurement::Position Vec3Sender::getMeasurement(TraverseInfo &ti, unsigned long long ts)
{
	boost::lock_guard<boost::mutex> lock(data_lock);
	Vec3d v = value->getValue(id);
	return Ubitrack::Measurement::Position(ts, Ubitrack::Math::Vector3d(v.x, v.y, v.z) );
}

