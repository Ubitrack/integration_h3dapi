#include <H3DUbitrack/ImageReceiver.h>
#include <boost/bind.hpp>


using namespace H3D;
using namespace H3DUbitrack;


// Add the nodes to the H3DNodeDatabase system.
H3DNodeDatabase ImageReceiver::database(
                "ImageReceiver",
                &(newInstance<ImageReceiver>),
                typeid( ImageReceiver ),
                &UbitrackMeasurement::database );


namespace ImageReceiverInternals {
	FIELDDB_ELEMENT( ImageReceiver, texture, INITIALIZE_ONLY );
}


ImageReceiver::ImageReceiver(
		H3D::Inst< H3D::SFNode > _metadata,
        H3D::Inst< H3D::SFString   > _pattern,
        H3D::Inst< H3D::SFBool   > _isSyncSource,
        H3D::Inst< MeasurementMode > _mode,
        H3D::Inst< SFUTImageTexture > _texture
		)
: UbitrackMeasurement( _metadata, _pattern, _isSyncSource, _mode)
, texture(_texture)
, connected(false)
{
    type_name = "ImageReceiver";
    database.initFields( this );

}

void ImageReceiver::initializeReceiver() {
    bool is_push = false;
    MeasurementMode::Mode _m = mode->getMeasurementMode();
    if (_m == MeasurementMode::PUSH) {
    	is_push = true;
    }
    receiver = new ImageReceiverImpl(this, pattern->getValue( id ), is_push);
}

void ImageReceiver::updateMeasurement(const Ubitrack::Measurement::ImageMeasurement& img)
{
	UTImageTexture* utit = dynamic_cast<UTImageTexture*>(texture->getValue(id));
	if (utit) {
		utit->updateTexture(img);
	}
}

