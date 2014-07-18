#include <H3DUbitrack/Vec4Receiver.h>
#include <boost/bind.hpp>


using namespace H3D;
using namespace H3DUbitrack;


// Add the nodes to the H3DNodeDatabase system.
H3DNodeDatabase Vec4Receiver::database(
                "Vec4Receiver",
                &(newInstance<Vec4Receiver>),
                typeid( Vec4Receiver ),
                &MeasurementReceiverBase::database );


namespace Vec4ReceiverInternals {
    // Vec4Receiver
    FIELDDB_ELEMENT( Vec4Receiver, value, OUTPUT_ONLY );
}


Vec4Receiver::Vec4Receiver(H3D::Inst< H3D::SFNode > _metadata,
                           H3D::Inst< H3D::SFString   > _pattern,
                           H3D::Inst< H3D::SFBool     > _isActive,
                           H3D::Inst< H3D::SFBool     > _isSyncSource,
                           H3D::Inst< H3D::SFBool     > _isDataAvailable,
                           H3D::Inst< MeasurementMode > _mode,
                           H3D::Inst< H3D::SFVec4d    > _value
                           )
: Vec4ReceiverBase(_metadata, _pattern, _isActive, _isSyncSource, _isDataAvailable, _mode)
, value(_value)
{
    type_name = "Vec4Receiver";
    database.initFields( this );
}

void Vec4Receiver::updateMeasurement(const Ubitrack::Measurement::Vector4D& m)
{
	boost::lock_guard<boost::mutex> lock(data_lock);
	Ubitrack::Math::Vector< double, 4 > v = *(m.get());
	Vec4d vec(v(0), v(1), v(2), v(3));
    value->setValue( vec , id );
}

