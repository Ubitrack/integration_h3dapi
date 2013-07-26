#include <H3DUbitrack/PoseReceiver.h>
#include <boost/bind.hpp>


using namespace H3D;
using namespace H3DUbitrack;


// Add the nodes to the H3DNodeDatabase system.
H3DNodeDatabase PoseReceiver::database(
                "PoseReceiver",
                &(newInstance<PoseReceiver>),
                typeid( PoseReceiver ),
                &MeasurementReceiverBase::database );


namespace PoseReceiverInternals {
    // PoseReceiver
    FIELDDB_ELEMENT( PoseReceiver, matrix, OUTPUT_ONLY );
    FIELDDB_ELEMENT( PoseReceiver, translation, OUTPUT_ONLY );
    FIELDDB_ELEMENT( PoseReceiver, rotation, OUTPUT_ONLY );
}


PoseReceiver::PoseReceiver(H3D::Inst< H3D::SFNode > _metadata,
                           H3D::Inst< H3D::SFString   > _pattern,
                           H3D::Inst< H3D::SFBool     > _isSyncSource,
                           H3D::Inst< H3D::SFBool     > _isDataAvailable,
                           H3D::Inst< MeasurementMode > _mode,
                           H3D::Inst< H3D::SFMatrix4f > _matrix,
                           H3D::Inst< H3D::SFVec3f    > _translation,
                           H3D::Inst< H3D::SFRotation > _rotation
                           )
: PoseReceiverBase(_metadata, _pattern, _isSyncSource, _isDataAvailable, _mode)
, matrix(_matrix)
, translation(_translation)
, rotation(_rotation)
{
    type_name = "PoseReceiver";
    database.initFields( this );
}

void PoseReceiver::updateMeasurement(const Ubitrack::Measurement::Pose& m)
{
	boost::lock_guard<boost::mutex> lock(data_lock);
    H3D::Vec3f t((H3DFloat)(m->translation()(0)), (H3DFloat)(m->translation()(1)), (H3DFloat)(m->translation()(2)));
    translation->setValue( t , id );

    H3D::Quaternion r((H3DFloat)(m->rotation().x()), (H3DFloat)(m->rotation().y()), (H3DFloat)(m->rotation().z()), (H3DFloat)(m->rotation().w()));
    rotation->setValue( Rotation(r) , id );

    H3D::Matrix4f mat(r);
    mat.setElement(0,1, t.x);
    mat.setElement(0,2, t.y);
    mat.setElement(0,3, t.z);
    matrix->setValue( mat , id );
}

