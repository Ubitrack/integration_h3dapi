/*
 * UbitrackContext.h
 *
 *  Created on: 03/08/2010
 *      Author: mvl
 */

#ifndef ROTATIONRECEIVER_H_
#define ROTATIONRECEIVER_H_


#include <H3D/X3DNode.h>

#include <H3D/SFString.h>
#include <H3D/SFRotation.h>

#include <H3DUbitrack/H3DUbitrack.h>
#include <H3DUbitrack/MeasurementReceiver.h>

#include <vector>

using namespace Ubitrack::Facade;

namespace H3DUbitrack {

typedef MeasurementReceiver<Ubitrack::Measurement::Rotation , Ubitrack::Components::ApplicationPullSinkRotation> RotationReceiverBase;

class H3DUBITRACK_API RotationReceiver : public RotationReceiverBase {

public:
    RotationReceiver( H3D::Inst< H3D::SFNode     > _metadata = 0,
                    H3D::Inst< H3D::SFString   > _pattern = 0,
                    H3D::Inst< H3D::SFBool     > _isSyncSource = 0,
        	        H3D::Inst< H3D::SFBool     > _isDataAvailable = 0,
                    H3D::Inst< MeasurementMode > _mode = 0,
                    H3D::Inst< H3D::SFRotation > _rotation = 0
                    );

    std::auto_ptr< H3D::SFRotation > rotation;

    /// Add this node to the H3DNodeDatabase system.
    static H3D::H3DNodeDatabase database;

    void updateMeasurement(const Ubitrack::Measurement::Rotation& m);

};

} // namespace H3DUbitrack

#endif //ROTATIONRECEIVER_H_
