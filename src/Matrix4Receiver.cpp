#include <H3DUbitrack/Matrix4Receiver.h>
#include <boost/bind.hpp>


using namespace H3D;
using namespace H3DUbitrack;


// Add the nodes to the H3DNodeDatabase system.
H3DNodeDatabase Matrix4Receiver::database(
                "Matrix4Receiver",
                &(newInstance<Matrix4Receiver>),
                typeid( Matrix4Receiver ),
                &MeasurementReceiverBase::database );


namespace Matrix4ReceiverInternals {
    // Matrix4Receiver
    FIELDDB_ELEMENT( Matrix4Receiver, matrix, OUTPUT_ONLY );
}


Matrix4Receiver::Matrix4Receiver(H3D::Inst< H3D::SFNode > _metadata,
                           H3D::Inst< H3D::SFString   > _pattern,
                           H3D::Inst< H3D::SFBool     > _isActive,
                           H3D::Inst< H3D::SFBool     > _isSyncSource,
                           H3D::Inst< H3D::SFBool     > _isDataAvailable,
                           H3D::Inst< MeasurementMode > _mode,
                           H3D::Inst< H3D::SFMatrix4d > _matrix
                           )
: Matrix4ReceiverBase(_metadata, _pattern, _isActive, _isSyncSource, _isDataAvailable, _mode)
, matrix(_matrix)
{
    type_name = "Matrix4Receiver";
    database.initFields( this );
}

void Matrix4Receiver::updateMeasurement(const Ubitrack::Measurement::Matrix4x4& m)
{
	boost::lock_guard<boost::mutex> lock(data_lock);
	Ubitrack::Math::Matrix4x4d v = *(m.get());
	Matrix4d mat(v(0,0), v(0,1), v(0,2), v(0,3),
				 v(1,0), v(1,1), v(1,2), v(1,3),
				 v(2,0), v(2,1), v(2,2), v(2,3),
				 v(3,0), v(3,1), v(3,2), v(3,3) );
    matrix->setValue( mat , id );
}

