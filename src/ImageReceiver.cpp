#include <H3DUbitrack/ImageReceiver.h>
#include <boost/bind.hpp>


using namespace H3D;
using namespace H3DUbitrack;


// Add the nodes to the H3DNodeDatabase system.
H3DNodeDatabase ImageReceiver::database(
                "ImageReceiver",
                &(newInstance<ImageReceiver>),
                typeid( ImageReceiver ),
                &MeasurementReceiverBase::database );


namespace ImageReceiverInternals {
	FIELDDB_ELEMENT( ImageReceiver, texture, INITIALIZE_ONLY );
}


ImageReceiver::ImageReceiver(
		H3D::Inst< H3D::SFNode > _metadata,
        H3D::Inst< H3D::SFString   > _pattern,
        H3D::Inst< H3D::SFBool     > _isActive,
        H3D::Inst< H3D::SFBool     > _isSyncSource,
        H3D::Inst< H3D::SFBool     > _isDataAvailable,
        H3D::Inst< MeasurementMode > _mode,
        H3D::Inst< SFUTTextureNode > _texture
		)
: ImageReceiverBase( _metadata, _pattern, _isActive, _isSyncSource, _isDataAvailable, _mode)
, texture(_texture)
{
    type_name = "ImageReceiver";
    database.initFields( this );
}
void ImageReceiver::updateMeasurement(const Ubitrack::Measurement::ImageMeasurement& img)
{
	UTTextureNode* utit = dynamic_cast<UTTextureNode*>(texture->getValue(id));
	if (utit) {
		boost::lock_guard<boost::mutex> lock(data_lock);
		utit->updateTexture(img);
	}
}

