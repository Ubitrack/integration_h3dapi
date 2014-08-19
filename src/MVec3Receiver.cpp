#include <H3DUbitrack/MVec3Receiver.h>
#include <boost/bind.hpp>


using namespace H3D;
using namespace H3DUbitrack;


// Add the nodes to the H3DNodeDatabase system.
H3DNodeDatabase MVec3Receiver::database(
                "MVec3Receiver",
                &(newInstance<MVec3Receiver>),
                typeid( MVec3Receiver ),
                &MeasurementReceiverBase::database );


namespace MVec3ReceiverInternals {
    // Vec3Receiver
    FIELDDB_ELEMENT( MVec3Receiver, value, OUTPUT_ONLY );
}


MVec3Receiver::MVec3Receiver(H3D::Inst< H3D::SFNode > _metadata,
                           H3D::Inst< H3D::SFString   > _pattern,
                           H3D::Inst< H3D::SFBool     > _isActive,
                           H3D::Inst< H3D::SFBool     > _isSyncSource,
                           H3D::Inst< H3D::SFBool     > _isDataAvailable,
                           H3D::Inst< MeasurementMode > _mode
                           )
: MVec3ReceiverBase(_metadata, _pattern, _isActive, _isSyncSource, _isDataAvailable, _mode)
, value(new MFVec3d)
{
    type_name = "MVec3Receiver";
    database.initFields( this );
	value->clear( id );
}

void MVec3Receiver::updateMeasurement(const Ubitrack::Measurement::PositionList& m)
{
	boost::lock_guard<boost::mutex> lock(data_lock);
    std::vector< Ubitrack::Math::Vector< double, 3 > > v = *(m.get());
    unsigned int l = v.size();
    if (value->size() != l) {
        value->resize(l, Vec3d(0,0,0), id);
    }
    for (unsigned int i=0; i<l; i++) {
        Vec3d vec(v.at(i)(0), v.at(i)(1), v.at(i)(2));
        value->setValue( i, vec , id );
    }


}

