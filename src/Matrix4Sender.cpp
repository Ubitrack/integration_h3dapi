#include <H3DUbitrack/Matrix4Sender.h>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

using namespace H3D;
using namespace H3DUbitrack;


// Add the nodes to the H3DNodeDatabase system.
H3DNodeDatabase Matrix4Sender::database(
        "Matrix4Sender",
        &(newInstance<Matrix4Sender>),
        typeid( Matrix4Sender ),
        &MeasurementSenderBase::database );


namespace Matrix4SenderInternals {
// Matrix4Sender
FIELDDB_ELEMENT( Matrix4Sender, value, INPUT_ONLY );
}


Matrix4Sender::Matrix4Sender(H3D::Inst< H3D::SFNode > _metadata,
        H3D::Inst< H3D::SFString   > _pattern,
        H3D::Inst< H3D::SFMatrix4d > _value
)
        : Matrix4SenderBase(_metadata, _pattern )
        , value(_value)
{
    type_name = "Matrix4Sender";
    database.initFields( this );

    value->setValue(H3D::Matrix4d());
    value->routeNoEvent(hasChanges, id);
}

Ubitrack::Measurement::Matrix4x4 Matrix4Sender::getMeasurement(TraverseInfo &ti, unsigned long long ts)
{
    boost::lock_guard<boost::mutex> lock(data_lock);
    Matrix4d v = value->getValue(id);

    double m [16] = {
            v[0][0], v[0][1], v[0][2], v[0][3],
            v[1][0], v[1][1], v[1][2], v[1][3],
            v[2][0], v[2][1], v[2][2], v[2][3],
            v[3][0], v[3][1], v[3][2], v[3][3]
    };
    return Ubitrack::Measurement::Matrix4x4(ts, Ubitrack::Math::Matrix4x4d(m) );
}


