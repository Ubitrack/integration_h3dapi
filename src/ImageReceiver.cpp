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
, push_receiver(NULL)
, pull_receiver(NULL)
{
    type_name = "PoseReceived";
    database.initFields( this );
}

bool ImageReceiver::connect(UbitrackInstance* instance)
{
    // check if already connected here !
    Console(4) << "Connect Receiver: " << pattern->getValue() << std::endl;

    AdvancedFacade* sf = instance->getFacadePtr();
    if (sf == NULL)
        return false;

    MeasurementMode::Mode _mode = mode->getMeasurementMode();
    if (_mode == MeasurementMode::PUSH) {
        // push receiver needs a registered callback
        push_receiver = new ReceiverCB<ImageReceiver, Ubitrack::Measurement::ImageMeasurement>(this);

    	try
    	{
    		sf->setCallback< Ubitrack::Measurement::ImageMeasurement >( pattern->getValue( id ).c_str(), boost::bind(&ReceiverCB<ImageReceiver, Ubitrack::Measurement::ImageMeasurement>::receiveMeasurement, boost::ref(push_receiver), _1) );
    		connected = true;
    	}
    	catch ( const Ubitrack::Util::Exception& e )
    	{
    		H3D::Console(4) << "Caught exception in ImageReceiver::setCallback( " << pattern->getValue( id ) <<" ): " << e << std::endl;
    		connected = false;
    	}

        return connected;
    } else {
        // pull receiver
    	try
    	{
    		pull_receiver = sf->componentByName< Ubitrack::Components::ApplicationPullSinkVisionImage >( pattern->getValue( id ).c_str() ).get();
    		connected = true;
    	}
    	catch ( const Ubitrack::Util::Exception& e )
    	{
    		H3D::Console(4) << "Caught exception in componentByName ApplicationPullSinkVisionImage( " << pattern->getValue( id ) <<" ): " << e << std::endl;
    		connected = false;
    	}
		return connected;
    }
}

bool ImageReceiver::disconnect(UbitrackInstance* instance)
{
    Console(4) << "Disconnect Receiver: " << pattern->getValue() << std::endl;

    if (!connected)
        return true;

    AdvancedFacade* sf = instance->getFacadePtr();
    if (sf == NULL)
        return false;

    MeasurementMode::Mode _mode = mode->getMeasurementMode();
    if (_mode == MeasurementMode::PUSH) {
    	try
    	{
    		sf->setCallback< Ubitrack::Measurement::ImageMeasurement >( pattern->getValue( id ).c_str(), NULL );
    		connected = false;
    	}
    	catch ( const Ubitrack::Util::Exception& e )
    	{
    		H3D::Console(4) << "Caught exception in ImageReceiver::setCallback( " << pattern->getValue( id ) <<" ): " << e << std::endl;
    	}

        push_receiver = NULL;
        return true;
    } else {
        // pull receiver
        pull_receiver = NULL;
        connected = false;
        return true;
    }
}

void ImageReceiver::update(unsigned long long int ts)
{
	if (!connected)
		return;
    MeasurementMode::Mode _mode = mode->getMeasurementMode();
    if ((_mode == MeasurementMode::PULL) && (pull_receiver != NULL)) {
        Ubitrack::Measurement::ImageMeasurement img = pull_receiver->get(ts);
        applyMeasurement<ImageReceiver, Ubitrack::Measurement::ImageMeasurement>(this, img);
    } else if ((_mode == MeasurementMode::PUSH) && (push_receiver != NULL)) {
    	push_receiver->transferMeasurements();
    }
}

void ImageReceiver::updateMeasurement(const Ubitrack::Measurement::ImageMeasurement& img)
{
	UTImageTexture* utit = dynamic_cast<UTImageTexture*>(texture->getValue(id));
	if (utit) {
		utit->updateTexture(img);
	}
}

