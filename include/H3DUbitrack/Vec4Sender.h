/*
 * UbitrackContext.h
 *
 *  Created on: 03/08/2010
 *      Author: mvl
 */

#ifndef VEC4SENDER_H_
#define VEC4SENDER_H_


#include <H3D/X3DNode.h>

#include <H3D/SFString.h>
#include <H3D/SFVec4d.h>

#include <H3DUbitrack/H3DUbitrack.h>
#include <H3DUbitrack/MeasurementSender.h>

#include <vector>

using namespace Ubitrack::Facade;

namespace H3DUbitrack {

typedef MeasurementSender<Ubitrack::Measurement::Vector4D , Ubitrack::Components::ApplicationPushSourceVector4D > Vec4SenderBase;

class H3DUBITRACK_API Vec4Sender : public Vec4SenderBase {

public:
    Vec4Sender( H3D::Inst< H3D::SFNode     > _metadata = 0,
                    H3D::Inst< H3D::SFString   > _pattern = 0,
                    H3D::Inst< H3D::SFVec4d    > _value = 0
                    );

    std::auto_ptr< H3D::SFVec4d > value;

    /// Add this node to the H3DNodeDatabase system.
    static H3D::H3DNodeDatabase database;

    Ubitrack::Measurement::Vector4D getMeasurement(H3D::TraverseInfo &ti, unsigned long long ts);

};

} // namespace H3DUbitrack

#endif //VEC4SENDER_H_
