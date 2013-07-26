/*
 * UbitrackContext.h
 *
 *  Created on: 03/08/2010
 *      Author: mvl
 */

#ifndef MATRIX3RECEIVER_H_
#define MATRIX3RECEIVER_H_


#include <H3D/X3DNode.h>

#include <H3D/SFString.h>
#include <H3D/SFMatrix3d.h>

#include <H3DUbitrack/H3DUbitrack.h>
#include <H3DUbitrack/MeasurementReceiver.h>

#include <vector>

using namespace Ubitrack::Facade;

namespace H3DUbitrack {

typedef MeasurementReceiver<Ubitrack::Measurement::Matrix3x3 , Ubitrack::Components::ApplicationPullSinkMatrix3x3> Matrix3ReceiverBase;

class H3DUBITRACK_API Matrix3Receiver : public Matrix3ReceiverBase {

public:
    Matrix3Receiver( H3D::Inst< H3D::SFNode     > _metadata = 0,
                    H3D::Inst< H3D::SFString   > _pattern = 0,
                    H3D::Inst< H3D::SFBool     > _isSyncSource = 0,
        	        H3D::Inst< H3D::SFBool     > _isDataAvailable = 0,
                    H3D::Inst< MeasurementMode > _mode = 0,
                    H3D::Inst< H3D::SFMatrix3d > _matrix   = 0
                    );

    std::auto_ptr< H3D::SFMatrix3d > matrix;

    /// Add this node to the H3DNodeDatabase system.
    static H3D::H3DNodeDatabase database;

    void updateMeasurement(const Ubitrack::Measurement::Matrix3x3& m);

};

} // namespace H3DUbitrack

#endif //MATRIX3RECEIVER_H_
