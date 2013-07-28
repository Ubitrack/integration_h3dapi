#include <H3DUbitrack/ButtonSender.h>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

using namespace H3D;
using namespace H3DUbitrack;


// Add the nodes to the H3DNodeDatabase system.
H3DNodeDatabase ButtonSender::database(
                "ButtonSender",
                &(newInstance<ButtonSender>),
                typeid( ButtonSender ),
                &MeasurementSenderBase::database );


namespace ButtonSenderInternals {
    // ButtonSender
    FIELDDB_ELEMENT( ButtonSender, value, INPUT_ONLY );
}


ButtonSender::ButtonSender(H3D::Inst< H3D::SFNode > _metadata,
                           H3D::Inst< H3D::SFString   > _pattern,
                           H3D::Inst< H3D::SFInt32 > _value
                           )
: ButtonSenderBase(_metadata, _pattern )
, value(_value)
{
    type_name = "ButtonSender";
    database.initFields( this );

    value->setValue(0);
    value->routeNoEvent(hasChanges, id);
}

Ubitrack::Measurement::Button ButtonSender::getMeasurement(unsigned long long ts)
{
	boost::lock_guard<boost::mutex> lock(data_lock);
	return Ubitrack::Measurement::Button(ts, (int)value->getValue(id));
}

