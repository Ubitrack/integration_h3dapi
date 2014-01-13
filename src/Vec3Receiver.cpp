#include <H3DUbitrack/Vec3Receiver.h>
#include <boost/bind.hpp>


using namespace H3D;
using namespace H3DUbitrack;


// Add the nodes to the H3DNodeDatabase system.
H3DNodeDatabase Vec3Receiver::database(
                "Vec3Receiver",
                &(newInstance<Vec3Receiver>),
                typeid( Vec3Receiver ),
                &MeasurementReceiverBase::database );


namespace Vec3ReceiverInternals {
    // Vec3Receiver
    FIELDDB_ELEMENT( Vec3Receiver, value, OUTPUT_ONLY );
}


Vec3Receiver::Vec3Receiver(H3D::Inst< H3D::SFNode > _metadata,
                           H3D::Inst< H3D::SFString   > _pattern,
                           H3D::Inst< H3D::SFBool     > _isSyncSource,
                           H3D::Inst< H3D::SFBool     > _isDataAvailable,
                           H3D::Inst< MeasurementMode > _mode,
                           H3D::Inst< H3D::SFVec3d    > _value
                           )
: Vec3ReceiverBase(_metadata, _pattern, _isSyncSource, _isDataAvailable, _mode)
, value(_value)
{
    type_name = "Vec3Receiver";
    database.initFields( this );
}

void Vec3Receiver::updateMeasurement(const Ubitrack::Measurement::Position& m)
{
	boost::lock_guard<boost::mutex> lock(data_lock);
	Ubitrack::Math::Vector< double, 3 > v = *(m.get());
	Vec3d vec(v(0), v(1), v(2));
    value->setValue( vec , id );
}

