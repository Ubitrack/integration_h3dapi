#include <H3DUbitrack/ButtonReceiver.h>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

using namespace H3D;
using namespace H3DUbitrack;


// Add the nodes to the H3DNodeDatabase system.
H3DNodeDatabase ButtonReceiver::database(
                "ButtonReceiver",
                &(newInstance<ButtonReceiver>),
                typeid( ButtonReceiver ),
                &MeasurementReceiverBase::database );


namespace ButtonReceiverInternals {
    // ButtonReceiver
    FIELDDB_ELEMENT( ButtonReceiver, value, OUTPUT_ONLY );
}


ButtonReceiver::ButtonReceiver(H3D::Inst< H3D::SFNode > _metadata,
                           H3D::Inst< H3D::SFString   > _pattern,
                           H3D::Inst< H3D::SFBool     > _isSyncSource,
                           H3D::Inst< H3D::SFBool     > _isDataAvailable,
                           H3D::Inst< MeasurementMode > _mode,
                           H3D::Inst< H3D::SFString > _value
                           )
: ButtonReceiverBase(_metadata, _pattern, _isSyncSource, _isDataAvailable, _mode)
, value(_value)
{
    type_name = "ButtonReceiver";
    database.initFields( this );
}

void ButtonReceiver::updateMeasurement(const Ubitrack::Measurement::Button& m)
{
	boost::lock_guard<boost::mutex> lock(data_lock);
	int v = *(m.get());
    value->setValue( boost::lexical_cast<std::string>(v) , id );
}

