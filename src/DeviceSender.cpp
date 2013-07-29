#include <H3DUbitrack/DeviceSender.h>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

using namespace H3D;
using namespace H3DUbitrack;


// Add the nodes to the H3DNodeDatabase system.
H3DNodeDatabase DeviceSender::database(
                "DeviceSender",
                &(newInstance<DeviceSender>),
                typeid( DeviceSender ),
                &MeasurementSenderBase::database );


namespace DeviceSenderInternals {
    // DeviceSender
    FIELDDB_ELEMENT( DeviceSender, jointAnglesPattern, INPUT_ONLY );
    FIELDDB_ELEMENT( DeviceSender, gimbalAnglesPattern, INPUT_ONLY );
    FIELDDB_ELEMENT( DeviceSender, isActive, INPUT_ONLY );
}


DeviceSender::DeviceSender(H3D::Inst< H3D::SFNode > _metadata,
                           H3D::Inst< H3D::SFString   > _pattern,
                           H3D::Inst< H3D::SFString    > _jointAnglesPattern,
                           H3D::Inst< H3D::SFString    > _gimbalAnglesPattern,
                           H3D::Inst< H3D::SFBool    > _isActive
                           )
: MeasurementSenderBase(_metadata, _pattern )
, jointAnglesPattern(_jointAnglesPattern)
, gimbalAnglesPattern(_gimbalAnglesPattern)
, isActive(_isActive)
, push_sender_pose(NULL)
, push_sender_jointAngles(NULL)
, push_sender_gimbalAngles(NULL)
, forceeffect(NULL)
{
    type_name = "DeviceSender";
    database.initFields( this );

    isActive->setValue(false);
    isActive->routeNoEvent(hasChanges, id);


}

