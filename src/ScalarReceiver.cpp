#include <H3DUbitrack/ScalarReceiver.h>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

using namespace H3D;
using namespace H3DUbitrack;


// Add the nodes to the H3DNodeDatabase system.
H3DNodeDatabase ScalarReceiver::database(
                "ScalarReceiver",
                &(newInstance<ScalarReceiver>),
                typeid( ScalarReceiver ),
                &MeasurementReceiverBase::database );


namespace ScalarReceiverInternals {
    // ScalarReceiver
    FIELDDB_ELEMENT( ScalarReceiver, value, OUTPUT_ONLY );
}


ScalarReceiver::ScalarReceiver(H3D::Inst< H3D::SFNode > _metadata,
                           H3D::Inst< H3D::SFString   > _pattern,
                           H3D::Inst< H3D::SFBool     > _isActive,
                           H3D::Inst< H3D::SFBool     > _isSyncSource,
                           H3D::Inst< H3D::SFBool     > _isDataAvailable,
                           H3D::Inst< MeasurementMode > _mode,
                           H3D::Inst< H3D::SFDouble > _value
                           )
: ScalarReceiverBase(_metadata, _pattern, _isActive, _isSyncSource, _isDataAvailable, _mode)
, value(_value)
{
    type_name = "ScalarReceiver";
    database.initFields( this );
}

void ScalarReceiver::updateMeasurement(const Ubitrack::Measurement::Distance& m)
{
	boost::lock_guard<boost::mutex> lock(data_lock);
	double v = *(m.get());
    value->setValue( v , id );
}

