#include <H3DUbitrack/Matrix3Sender.h>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

using namespace H3D;
using namespace H3DUbitrack;


// Add the nodes to the H3DNodeDatabase system.
H3DNodeDatabase Matrix3Sender::database(
        "Matrix3Sender",
        &(newInstance<Matrix3Sender>),
        typeid( Matrix3Sender ),
        &MeasurementSenderBase::database );


namespace Matrix3SenderInternals {
// Matrix3Sender
FIELDDB_ELEMENT( Matrix3Sender, value, INPUT_ONLY );
}


Matrix3Sender::Matrix3Sender(H3D::Inst< H3D::SFNode > _metadata,
        H3D::Inst< H3D::SFString   > _pattern,
        H3D::Inst< H3D::SFMatrix3d > _value
)
        : Matrix3SenderBase(_metadata, _pattern )
        , value(_value)
{
    type_name = "Matrix3Sender";
    database.initFields( this );

    value->setValue(H3D::Matrix3d());
    value->routeNoEvent(hasChanges, id);
}

Ubitrack::Measurement::Matrix3x3 Matrix3Sender::getMeasurement(TraverseInfo &ti, unsigned long long ts)
{
    boost::lock_guard<boost::mutex> lock(data_lock);
    Matrix3d v = value->getValue(id);

    double m [9] = {
            v[0][0], v[0][1], v[0][2],
            v[1][0], v[1][1], v[1][2],
            v[2][0], v[2][1], v[2][2]
    };
    return Ubitrack::Measurement::Matrix3x3(ts, Ubitrack::Math::Matrix3x3d(m) );
}

