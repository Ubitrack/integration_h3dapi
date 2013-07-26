#include <H3DUbitrack/RotationReceiver.h>
#include <boost/bind.hpp>


using namespace H3D;
using namespace H3DUbitrack;


// Add the nodes to the H3DNodeDatabase system.
H3DNodeDatabase RotationReceiver::database(
                "RotationReceiver",
                &(newInstance<RotationReceiver>),
                typeid( RotationReceiver ),
                &MeasurementReceiverBase::database );


namespace RotationReceiverInternals {
    // RotationReceiver
    FIELDDB_ELEMENT( RotationReceiver, rotation, OUTPUT_ONLY );
}


RotationReceiver::RotationReceiver(H3D::Inst< H3D::SFNode > _metadata,
                           H3D::Inst< H3D::SFString   > _pattern,
                           H3D::Inst< H3D::SFBool     > _isSyncSource,
                           H3D::Inst< H3D::SFBool     > _isDataAvailable,
                           H3D::Inst< MeasurementMode > _mode,
                           H3D::Inst< H3D::SFRotation > _rotation
                           )
: RotationReceiverBase(_metadata, _pattern, _isSyncSource, _isDataAvailable, _mode)
, rotation(_rotation)
{
    type_name = "RotationReceiver";
    database.initFields( this );
}

void RotationReceiver::updateMeasurement(const Ubitrack::Measurement::Rotation& m)
{
	boost::lock_guard<boost::mutex> lock(data_lock);
	Ubitrack::Math::Quaternion v = *(m.get());
    H3D::Quaternion r((H3DFloat)(v.x()), (H3DFloat)(v.y()), (H3DFloat)(v.z()), (H3DFloat)(v.w()));
    rotation->setValue( Rotation(r) , id );
}

