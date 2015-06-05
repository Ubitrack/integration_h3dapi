#include <H3DUbitrack/Vec2Sender.h>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

using namespace H3D;
using namespace H3DUbitrack;


// Add the nodes to the H3DNodeDatabase system.
H3DNodeDatabase Vec2Sender::database(
                "Vec2Sender",
                &(newInstance<Vec2Sender>),
                typeid( Vec2Sender ),
                &MeasurementSenderBase::database );


namespace Vec2SenderInternals {
    // Vec2Sender
    FIELDDB_ELEMENT( Vec2Sender, value, INPUT_ONLY );
}


Vec2Sender::Vec2Sender(H3D::Inst< H3D::SFNode > _metadata,
                           H3D::Inst< H3D::SFString   > _pattern,
                           H3D::Inst< H3D::SFVec2d > _value
                           )
: Vec2SenderBase(_metadata, _pattern )
, value(_value)
{
    type_name = "Vec2Sender";
    database.initFields( this );

    value->setValue(Vec2d(0., 0.));
    value->routeNoEvent(hasChanges, id);
}

Ubitrack::Measurement::Position2D Vec2Sender::getMeasurement(TraverseInfo &ti, unsigned long long ts)
{
	boost::lock_guard<boost::mutex> lock(data_lock);
	Vec2d v = value->getValue(id);
	return Ubitrack::Measurement::Position2D(ts, Ubitrack::Math::Vector2d(v.x, v.y) );
}

