/*
 * UbitrackContext.h
 *
 *  Created on: 03/08/2010
 *      Author: mvl
 */

#ifndef VEC4RECEIVER_H_
#define VEC4RECEIVER_H_


#include <H3D/X3DNode.h>

#include <H3D/SFString.h>
#include <H3D/SFVec4d.h>

#include <H3DUbitrack/H3DUbitrack.h>
#include <H3DUbitrack/MeasurementReceiver.h>

#include <vector>

using namespace Ubitrack::Facade;

namespace H3DUbitrack {

typedef MeasurementReceiver<Ubitrack::Measurement::Vector4D , Ubitrack::Components::ApplicationPullSinkVector4D> Vec4ReceiverBase;

class H3DUBITRACK_API Vec4Receiver : public Vec4ReceiverBase {

public:
    Vec4Receiver( H3D::Inst< H3D::SFNode     > _metadata = 0,
                    H3D::Inst< H3D::SFString   > _pattern = 0,
                    H3D::Inst< H3D::SFBool     > _isSyncSource = 0,
        	        H3D::Inst< H3D::SFBool     > _isDataAvailable = 0,
                    H3D::Inst< MeasurementMode > _mode = 0,
                    H3D::Inst< H3D::SFVec4d    > _value = 0
                    );

    std::auto_ptr< H3D::SFVec4d    > value;

    /// Add this node to the H3DNodeDatabase system.
    static H3D::H3DNodeDatabase database;

    void updateMeasurement(const Ubitrack::Measurement::Vector4D& m);

};

} // namespace H3DUbitrack

#endif //VEC4RECEIVER_H_
