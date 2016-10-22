#include <H3DUbitrack/PoseListReceiver.h>
#include <boost/bind.hpp>


using namespace H3D;
using namespace H3DUbitrack;


// Add the nodes to the H3DNodeDatabase system.
H3DNodeDatabase PoseListReceiver::database(
                "PoseListReceiver",
                &(newInstance<PoseListReceiver>),
                typeid( PoseListReceiver ),
                &MeasurementReceiverBase::database );


namespace PoseListReceiverInternals {
    // PoseReceiver
    FIELDDB_ELEMENT( PoseListReceiver, matrix, OUTPUT_ONLY );
    FIELDDB_ELEMENT( PoseListReceiver, translation, OUTPUT_ONLY );
    FIELDDB_ELEMENT( PoseListReceiver, rotation, OUTPUT_ONLY );
}


PoseListReceiver::PoseListReceiver(H3D::Inst< H3D::SFNode > _metadata,
                           H3D::Inst< H3D::SFString   > _pattern,
                           H3D::Inst< H3D::SFBool     > _isActive,
                           H3D::Inst< H3D::SFBool     > _isSyncSource,
                           H3D::Inst< H3D::SFBool     > _isDataAvailable,
                           H3D::Inst< MeasurementMode > _mode,
                           H3D::Inst< H3D::MFMatrix4f > _matrix,
                           H3D::Inst< H3D::MFVec3f    > _translation,
                           H3D::Inst< H3D::MFRotation > _rotation
                           )
: PoseListReceiverBase(_metadata, _pattern, _isActive, _isSyncSource, _isDataAvailable, _mode)
, matrix(_matrix)
, translation(_translation)
, rotation(_rotation)
{
    type_name = "PoseListReceiver";
    database.initFields( this );
}

void PoseListReceiver::updateMeasurement(const Ubitrack::Measurement::PoseList& m)
{
	boost::lock_guard<boost::mutex> lock(data_lock);

    std::vector< Ubitrack::Math::Pose > v = *(m.get());

	unsigned int l = v.size();
    if (matrix->size() != l) {
        matrix->resize(l, Matrix4f(), id);
        translation->resize(l, Vec3f(0, 0, 0), id);
        rotation->resize(l, Rotation(), id);
    }

    for (unsigned int i=0; i<l; i++) {
		H3D::Vec3f t((H3DFloat)(v[i].translation()(0)), (H3DFloat)(v[i].translation()(1)), (H3DFloat)(v[i].translation()(2)));
		translation->setValue(i, t , id );

		H3D::Quaternion r((H3DFloat)(v[i].rotation().x()), (H3DFloat)(v[i].rotation().y()), (H3DFloat)(v[i].rotation().z()), (H3DFloat)(v[i].rotation().w()));
		rotation->setValue( i, Rotation(r) , id );

		H3D::Matrix4f mat(r);
		mat.setElement(0,3, t.x);
		mat.setElement(1,3, t.y);
		mat.setElement(2,3, t.z);
		matrix->setValue( i, mat , id );
    }

}

