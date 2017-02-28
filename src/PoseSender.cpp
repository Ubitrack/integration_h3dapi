#include <H3DUbitrack/PoseSender.h>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

using namespace H3D;
using namespace H3DUbitrack;


// Add the nodes to the H3DNodeDatabase system.
H3DNodeDatabase PoseSender::database(
        "PoseSender",
        &(newInstance<PoseSender>),
        typeid( PoseSender ),
        &MeasurementSenderBase::database );


namespace PoseSenderInternals {
// PoseSender
FIELDDB_ELEMENT( PoseSender, value, INPUT_ONLY );
}


PoseSender::PoseSender(H3D::Inst< H3D::SFNode > _metadata,
        H3D::Inst< H3D::SFString   > _pattern,
        H3D::Inst< H3D::SFMatrix4d > _value
)
        : PoseSenderBase(_metadata, _pattern )
        , value(_value)
{
    type_name = "PoseSender";
    database.initFields( this );

    value->setValue(Matrix4d());
    value->routeNoEvent(hasChanges, id);
}

Ubitrack::Measurement::Pose PoseSender::getMeasurement(TraverseInfo &ti, unsigned long long ts)
{
    boost::lock_guard<boost::mutex> lock(data_lock);
    H3D::Matrix4d v = value->getValue(id);

    H3D::Vec3d t(v.getTranslationPart());
    Ubitrack::Math::Vector<double, 3> t_(t.x, t.y, t.z);

    H3D::Quaterniond r(v.getRotationPart());
    Ubitrack::Math::Quaternion r_(r.v.x, r.v.y, r.v.z, r.w);

    return Ubitrack::Measurement::Pose(ts, Ubitrack::Math::Pose(r_, t_) );
}

