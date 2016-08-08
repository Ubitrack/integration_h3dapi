/*
 * UbitrackContext.h
 *
 *  Created on: 03/08/2010
 *      Author: mvl
 */

#ifndef POSELISTRECEIVER_H_
#define POSELISTRECEIVER_H_


#include <H3D/X3DNode.h>

#include <H3D/SFString.h>
#include <H3D/MFVec3f.h>
#include <H3D/MFRotation.h>
#include <H3D/MFMatrix4f.h>

#include <H3DUbitrack/H3DUbitrack.h>
#include <H3DUbitrack/MeasurementReceiver.h>

#include <vector>

using namespace Ubitrack::Facade;

namespace H3DUbitrack {

typedef MeasurementReceiver<Ubitrack::Measurement::PoseList , Ubitrack::Components::ApplicationPullSinkPoseList> PoseListReceiverBase;

class H3DUBITRACK_API PoseListReceiver : public PoseListReceiverBase {

public:
    PoseListReceiver( H3D::Inst< H3D::SFNode     > _metadata = 0,
                    H3D::Inst< H3D::SFString   > _pattern = 0,
                    H3D::Inst< H3D::SFBool     > _isActive = 0,
                    H3D::Inst< H3D::SFBool     > _isSyncSource = 0,
        	        H3D::Inst< H3D::SFBool     > _isDataAvailable = 0,
                    H3D::Inst< MeasurementMode > _mode = 0,
                    H3D::Inst< H3D::MFMatrix4f > _matrix   = 0,
                    H3D::Inst< H3D::MFVec3f    > _translation = 0,
                    H3D::Inst< H3D::MFRotation > _rotation = 0
                    );

    std::auto_ptr< H3D::MFMatrix4f > matrix;
    std::auto_ptr< H3D::MFVec3f    > translation;
    std::auto_ptr< H3D::MFRotation > rotation;

    /// Add this node to the H3DNodeDatabase system.
    static H3D::H3DNodeDatabase database;

    void updateMeasurement(const Ubitrack::Measurement::PoseList& m);

};

} // namespace H3DUbitrack

#endif //POSELISTRECEIVER_H_
