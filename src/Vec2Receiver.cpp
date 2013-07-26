#include <H3DUbitrack/Vec2Receiver.h>
#include <boost/bind.hpp>


using namespace H3D;
using namespace H3DUbitrack;


// Add the nodes to the H3DNodeDatabase system.
H3DNodeDatabase Vec2Receiver::database(
                "Vec2Receiver",
                &(newInstance<Vec2Receiver>),
                typeid( Vec2Receiver ),
                &MeasurementReceiverBase::database );


namespace Vec2ReceiverInternals {
    // Vec2Receiver
    FIELDDB_ELEMENT( Vec2Receiver, value, OUTPUT_ONLY );
}


Vec2Receiver::Vec2Receiver(H3D::Inst< H3D::SFNode > _metadata,
                           H3D::Inst< H3D::SFString   > _pattern,
                           H3D::Inst< H3D::SFBool     > _isSyncSource,
                           H3D::Inst< H3D::SFBool     > _isDataAvailable,
                           H3D::Inst< MeasurementMode > _mode,
                           H3D::Inst< H3D::SFVec2d    > _value
                           )
: Vec2ReceiverBase(_metadata, _pattern, _isSyncSource, _isDataAvailable, _mode)
, value(_value)
{
    type_name = "Vec2Receiver";
    database.initFields( this );
}

void Vec2Receiver::updateMeasurement(const Ubitrack::Measurement::Position2D& m)
{
	boost::lock_guard<boost::mutex> lock(data_lock);
	Ubitrack::Math::Vector< 2 > v = *(m.get());
	Vec2d vec(v(0), v(1));
    value->setValue( vec , id );
}

