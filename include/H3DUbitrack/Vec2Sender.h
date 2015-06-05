/*
 * UbitrackContext.h
 *
 *  Created on: 03/08/2010
 *      Author: mvl
 */

#ifndef VEC2SENDER_H_
#define VEC2SENDER_H_


#include <H3D/X3DNode.h>

#include <H3D/SFString.h>
#include <H3D/SFVec2d.h>

#include <H3DUbitrack/H3DUbitrack.h>
#include <H3DUbitrack/MeasurementSender.h>

#include <vector>

using namespace Ubitrack::Facade;

namespace H3DUbitrack {

typedef MeasurementSender<Ubitrack::Measurement::Position2D , Ubitrack::Components::ApplicationPushSourcePosition2D > Vec2SenderBase;

class H3DUBITRACK_API Vec2Sender : public Vec2SenderBase {

public:
    Vec2Sender( H3D::Inst< H3D::SFNode     > _metadata = 0,
                    H3D::Inst< H3D::SFString   > _pattern = 0,
                    H3D::Inst< H3D::SFVec2d    > _value = 0
                    );

    std::auto_ptr< H3D::SFVec2d > value;

    /// Add this node to the H3DNodeDatabase system.
    static H3D::H3DNodeDatabase database;

    Ubitrack::Measurement::Position2D getMeasurement(H3D::TraverseInfo &ti, unsigned long long ts);

};

} // namespace H3DUbitrack

#endif //VEC2SENDER_H_
