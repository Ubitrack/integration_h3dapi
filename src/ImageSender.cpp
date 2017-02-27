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
}


ImageSender::ImageSender(H3D::Inst< H3D::SFNode > _metadata,
        H3D::Inst< H3D::SFString   > _pattern,
        H3D::Inst< SFImage > _value
)
        : ImageSenderBase(_metadata, _pattern ),
          value(_value),
          frame_bytes_allocated(0)
{
    type_name = "ImageSender";
    database.initFields( this );

    value->setName( "image" );
    value->setOwner( this );
    value->routeNoEvent(hasChanges, id);
}

Ubitrack::Measurement::ImageMeasurement ImageSender::getMeasurement(TraverseInfo &ti, unsigned long long ts)
{
    boost::lock_guard<boost::mutex> lock(data_lock);
    PixelImage *pi = dynamic_cast<PixelImage *> (value->getValue());
    if (pi) {

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
        if (frame_bytes_allocated != required_frame_bytes) {
            ut_image.reset(new Ubitrack::Vision::Image(width, height, depth));
            ut_image->set_pixelFormat(fmt);
            ut_image->set_origin(0); // correct?
        }
        unsigned char* srcData = (unsigned char*) pi->getImageData();
        unsigned char* dstData = (unsigned char*) ut_image->Mat().data;
        memcpy(dstData, srcData, sizeof(unsigned char)*frame_bytes_allocated);

    }
    return Ubitrack::Measurement::ImageMeasurement(ts, ut_image);
}

