#include <H3DUbitrack/Matrix3Receiver.h>
#include <boost/bind.hpp>


using namespace H3D;
using namespace H3DUbitrack;


// Add the nodes to the H3DNodeDatabase system.
H3DNodeDatabase Matrix3Receiver::database(
                "Matrix3Receiver",
                &(newInstance<Matrix3Receiver>),
                typeid( Matrix3Receiver ),
                &MeasurementReceiverBase::database );


namespace Matrix3ReceiverInternals {
    // Matrix3Receiver
    FIELDDB_ELEMENT( Matrix3Receiver, matrix, OUTPUT_ONLY );
}


Matrix3Receiver::Matrix3Receiver(H3D::Inst< H3D::SFNode > _metadata,
                           H3D::Inst< H3D::SFString   > _pattern,
                           H3D::Inst< H3D::SFBool     > _isActive,
                           H3D::Inst< H3D::SFBool     > _isSyncSource,
                           H3D::Inst< H3D::SFBool     > _isDataAvailable,
                           H3D::Inst< MeasurementMode > _mode,
                           H3D::Inst< H3D::SFMatrix3d > _matrix
                           )
: Matrix3ReceiverBase(_metadata, _pattern, _isActive, _isSyncSource, _isDataAvailable, _mode)
, matrix(_matrix)
{
    type_name = "Matrix3Receiver";
    database.initFields( this );
}

void Matrix3Receiver::updateMeasurement(const Ubitrack::Measurement::Matrix3x3& m)
{
	boost::lock_guard<boost::mutex> lock(data_lock);
	Ubitrack::Math::Matrix3x3d v = *(m.get());
	Matrix3d mat(v(0,0), v(0,1), v(0,2),
				 v(1,0), v(1,1), v(1,2),
				 v(2,0), v(2,1), v(2,2) );
    matrix->setValue( mat , id );
}

