/*
 * UbitrackContext.h
 *
 *  Created on: 03/08/2010
 *      Author: mvl
 */

#ifndef BUTTONSENDER_H_
#define BUTTONSENDER_H_


#include <H3D/X3DNode.h>

#include <H3D/SFString.h>
#include <H3D/SFInt32.h>

#include <H3DUbitrack/H3DUbitrack.h>
#include <H3DUbitrack/MeasurementSender.h>

#include <vector>

using namespace Ubitrack::Facade;

namespace H3DUbitrack {

typedef MeasurementSender<Ubitrack::Measurement::Button , Ubitrack::Components::ApplicationPushSourceButton> ButtonSenderBase;

class H3DUBITRACK_API ButtonSender : public ButtonSenderBase {

public:
    ButtonSender( H3D::Inst< H3D::SFNode     > _metadata = 0,
                    H3D::Inst< H3D::SFString   > _pattern = 0,
                    H3D::Inst< H3D::SFInt32    > _value = 0
                    );

    std::auto_ptr< H3D::SFInt32 > value;

    /// Add this node to the H3DNodeDatabase system.
    static H3D::H3DNodeDatabase database;

    Ubitrack::Measurement::Button getMeasurement(unsigned long long ts);

};

} // namespace H3DUbitrack

#endif //BUTTONSENDER_H_
