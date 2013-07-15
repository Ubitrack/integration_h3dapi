/*
 * UbitrackContext.h
 *
 *  Created on: 03/08/2010
 *      Author: mvl
 */

#ifndef POSERECEIVER_H_
#define POSERECEIVER_H_


#include <H3D/X3DNode.h>

#include <H3D/SFString.h>
#include <H3D/SFVec3f.h>
#include <H3D/SFRotation.h>
#include <H3D/SFMatrix4f.h>

#include <H3DUbitrack/H3DUbitrack.h>
#include <H3DUbitrack/UbitrackInstance.h>
#include <H3DUbitrack/UbitrackMeasurement.h>
#include <H3DUbitrack/MeasurementReceiver.h>

#include <utFacade/AdvancedFacade.h>
#include <utMeasurement/Measurement.h>
#include <utComponents/ApplicationPullSink.h>


#include <vector>

using namespace Ubitrack::Facade;

namespace H3DUbitrack {


class H3DUBITRACK_API PoseReceiver : public UbitrackMeasurement {

public:
    PoseReceiver( H3D::Inst< H3D::SFNode     > _metadata = 0,
                    H3D::Inst< H3D::SFString   > _pattern = 0,
                    H3D::Inst< H3D::SFBool   > _isSyncSource = 0,
                    H3D::Inst< MeasurementMode > _mode = 0,
                    H3D::Inst< H3D::SFMatrix4f > _matrix   = 0,
                    H3D::Inst< H3D::SFVec3f    > _translation = 0,
                    H3D::Inst< H3D::SFRotation > _rotation = 0
                    );

    std::auto_ptr< H3D::SFMatrix4f > matrix;
    std::auto_ptr< H3D::SFVec3f    > translation;
    std::auto_ptr< H3D::SFRotation > rotation;

    virtual string defaultXMLContainerField() { return "receiver"; }

    // basic initialization
    virtual void initializeReceiver();

    /// Add this node to the H3DNodeDatabase system.
    static H3D::H3DNodeDatabase database;

    void updateMeasurement(const Ubitrack::Measurement::Pose& m);

protected:
    typedef MeasurementReceiver< PoseReceiver, Ubitrack::Measurement::Pose , Ubitrack::Components::ApplicationPullSinkPose > PoseReceiverImpl;
    bool connected;

};




} // namespace H3DUbitrack

#endif //POSERECEIVER_H_
