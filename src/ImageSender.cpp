#include <H3DUbitrack/ImageSender.h>

#include <H3D/X3DAppearanceChildNode.h>
#include <H3DUtil/Image.h>

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

using namespace H3D;
using namespace H3DUbitrack;


// Add the nodes to the H3DNodeDatabase system.
H3DNodeDatabase ImageSender::database(
        "ImageSender",
        &(newInstance<ImageSender>),
        typeid( ImageSender ),
        &MeasurementSenderBase::database );


namespace ImageSenderInternals {
    // ImageSender
    FIELDDB_ELEMENT( ImageSender, value, INPUT_ONLY );
	FIELDDB_ELEMENT(ImageSender, width, INPUT_OUTPUT);
	FIELDDB_ELEMENT(ImageSender, height, INPUT_OUTPUT);
	FIELDDB_ELEMENT(ImageSender, capture, INPUT_OUTPUT);
}


ImageSender::ImageSender(H3D::Inst< H3D::SFNode > _metadata,
        H3D::Inst< H3D::SFString   > _pattern,
        H3D::Inst< H3D::SFInt32 > _width,
        H3D::Inst< H3D::SFInt32 > _height,
        H3D::Inst< SFTexture > _value,
        H3D::Inst< SFCapture > _capture

)
        : ImageSenderBase(_metadata, _pattern ),
          value(_value),
          width(_width),
          height(_height),
          capture(_capture),
          frame_bytes_allocated(0)
{
    type_name = "ImageSender";
    database.initFields( this );

    width->setValue(512, id);
    height->setValue(512, id);

    value->setName( "texture" );
    value->setOwner( this );
    value->routeNoEvent(hasChanges, id);
    ut_image.reset();
}

Ubitrack::Measurement::ImageMeasurement ImageSender::getMeasurement(TraverseInfo &ti, unsigned long long ts)
{
    boost::lock_guard<boost::mutex> lock(data_lock);
    Node *n = value->getValue();
    Ubitrack::Measurement::ImageMeasurement measurement;

    if( H3DSingleTextureNode *t = dynamic_cast< H3DSingleTextureNode *>( n ) )
    {
        AutoRef<Image> pi ( t->renderToImage (
                t->saveWidth->getValue()  == -1 ? width->getValue()  : t->saveWidth->getValue(),
                t->saveHeight->getValue() == -1 ? height->getValue() : t->saveHeight->getValue())
        );

        if ( pi.get() ) {
            unsigned int depth = 3;
            Ubitrack::Vision::Image::PixelFormat fmt = Ubitrack::Vision::Image::RGB;

            switch (pi->pixelType()) {
            case Image::LUMINANCE:
                fmt = Ubitrack::Vision::Image::LUMINANCE;
                depth = 1;
                break;
            case Image::RGB:
                fmt = Ubitrack::Vision::Image::RGB;
                break;
            case Image::BGR:
                fmt = Ubitrack::Vision::Image::BGR;
                break;
            case Image::BGRA:
                fmt = Ubitrack::Vision::Image::BGRA;
                depth = 4;
                break;
            case Image::RGBA:
                fmt = Ubitrack::Vision::Image::RGBA;
                depth = 4;
                break;
            default:
                // Log Error ?
                break;
            }

            int width = pi->width();
            int height = pi->height();

            unsigned int required_frame_bytes = width * height * depth;

            if (required_frame_bytes > 0) {
                if (frame_bytes_allocated!=required_frame_bytes) {
                    ut_image.reset(new Ubitrack::Vision::Image(width, height, depth));
                    ut_image->set_pixelFormat(fmt);
                    ut_image->set_origin(0); // correct?

                    frame_bytes_allocated = required_frame_bytes;
                }
                unsigned char* srcData = (unsigned char*) pi->getImageData();
                unsigned char* dstData = (unsigned char*) ut_image->Mat().data;
                memcpy(dstData, srcData, sizeof(unsigned char)*frame_bytes_allocated);
                measurement = Ubitrack::Measurement::ImageMeasurement(ts, ut_image);
            }
        }
    }

    return measurement;
}

void ImageSender::SFCapture::onNewValue( const bool &v )
{
    ImageSender* node = static_cast< ImageSender* >( getOwner());
    if (v) {
        node->touch();
    }
}
