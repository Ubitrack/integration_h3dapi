#include <H3DUbitrack/MVec2Receiver.h>
#include <boost/bind.hpp>


using namespace H3D;
using namespace H3DUbitrack;


// Add the nodes to the H3DNodeDatabase system.
H3DNodeDatabase MVec2Receiver::database(
                "MVec2Receiver",
                &(newInstance<MVec2Receiver>),
                typeid( MVec2Receiver ),
                &MeasurementReceiverBase::database );


namespace MVec2ReceiverInternals {
    // Vec2Receiver
    FIELDDB_ELEMENT( MVec2Receiver, value, OUTPUT_ONLY );
}


MVec2Receiver::MVec2Receiver(H3D::Inst< H3D::SFNode > _metadata,
                           H3D::Inst< H3D::SFString   > _pattern,
                           H3D::Inst< H3D::SFBool     > _isActive,
                           H3D::Inst< H3D::SFBool     > _isSyncSource,
                           H3D::Inst< H3D::SFBool     > _isDataAvailable,
                           H3D::Inst< MeasurementMode > _mode
                           )
: MVec2ReceiverBase(_metadata, _pattern, _isActive, _isSyncSource, _isDataAvailable, _mode)
, value(new MFVec2d)
{
    type_name = "MVec2Receiver";
    database.initFields( this );
	value->clear( id );
}

void MVec2Receiver::updateMeasurement(const Ubitrack::Measurement::PositionList2& m)
{
	boost::lock_guard<boost::mutex> lock(data_lock);
    std::vector< Ubitrack::Math::Vector< double, 2 > > v = *(m.get());
    unsigned int l = v.size();
    if (value->size() != l) {
        value->resize(l, Vec2d(0,0), id);
    }
    for (unsigned int i=0; i<l; i++) {
        Vec2d vec(v.at(i)(0), v.at(i)(1));
        value->setValue( i, vec , id );
    }
}

