//
// Created by Ulrich Eck on 28/02/2017.
//

#ifndef H3DUBITRACK_MATRIX4SENDER_H
#define H3DUBITRACK_MATRIX4SENDER_H

#include <H3D/X3DNode.h>

#include <H3D/SFString.h>
#include <H3D/SFMatrix4d.h>

#include <H3DUbitrack/H3DUbitrack.h>
#include <H3DUbitrack/MeasurementSender.h>

#include <vector>

using namespace Ubitrack::Facade;

namespace H3DUbitrack {

typedef MeasurementSender<Ubitrack::Measurement::Matrix4x4 , Ubitrack::Components::ApplicationPushSourceMatrix4x4 > Matrix4SenderBase;

class H3DUBITRACK_API Matrix4Sender : public Matrix4SenderBase {

public:
    Matrix4Sender( H3D::Inst< H3D::SFNode > _metadata = 0,
            H3D::Inst< H3D::SFString      > _pattern = 0,
            H3D::Inst< H3D::SFMatrix4d    > _value = 0
    );

    std::auto_ptr< H3D::SFMatrix4d > value;

    /// Add this node to the H3DNodeDatabase system.
    static H3D::H3DNodeDatabase database;

    Ubitrack::Measurement::Matrix4x4 getMeasurement(H3D::TraverseInfo &ti, unsigned long long ts);

};

} // namespace H3DUbitrack


#endif //H3DUBITRACK_MATRIX4SENDER_H
