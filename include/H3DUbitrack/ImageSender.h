/*
 * UbitrackContext.h
 *
 *  Created on: 03/08/2010
 *      Author: mvl
 */

#ifndef IMAGESENDER_H_
#define IMAGESENDER_H_

#include <H3D/X3DNode.h>

#include <H3D/SFNode.h>
#include <H3D/SFBool.h>
#include <H3D/SFInt32.h>
#include "H3D/H3DSingleTextureNode.h"
#include <H3DUtil/Image.h>
#include <H3D/FieldTemplates.h>
#include <H3D/SFString.h>

#include <H3DUbitrack/H3DUbitrack.h>
#include <H3DUbitrack/MeasurementSender.h>

#include <utComponents/ApplicationEndpointsVision.h>
#include <utVision/Image.h>

#include <vector>

using namespace Ubitrack::Facade;

namespace H3DUbitrack {

typedef MeasurementSender <Ubitrack::Measurement::ImageMeasurement, Ubitrack::Components::ApplicationPushSourceVisionImage> ImageSenderBase;

class H3DUBITRACK_API ImageSender : public ImageSenderBase {

public:

    /// A field used to capture the texture
    class H3DUBITRACK_API SFCapture : public H3D::OnNewValueSField < H3D::AutoUpdate < H3D::SFBool > > {
        virtual void onNewValue( const bool &v );
    };


    typedef H3D::TypedSFNode< H3D::H3DSingleTextureNode > SFTexture;

    ImageSender( H3D::Inst<H3D::SFNode> _metadata = 0,
            H3D::Inst<H3D::SFString> _pattern = 0,
            H3D::Inst< H3D::SFInt32 > _width  = 0,
            H3D::Inst< H3D::SFInt32 > _height  = 0,
            H3D::Inst< SFTexture > _value  = 0,
            H3D::Inst< SFCapture > _capture  = 0
    );

    /// manually capture
    std::auto_ptr< SFCapture > capture;

    /// width of the captured texture
    std::auto_ptr< H3D::SFInt32 > width;

    /// height of the captured texture
    std::auto_ptr< H3D::SFInt32 > height;

    /// Contains the texture to be captured
    std::auto_ptr < SFTexture > value;

    /// Add this node to the H3DNodeDatabase system.
    static H3D::H3DNodeDatabase database;

    Ubitrack::Measurement::ImageMeasurement getMeasurement(H3D::TraverseInfo& ti, unsigned long long ts);

protected:
    boost::shared_ptr<Ubitrack::Vision::Image> ut_image;
    unsigned int							   frame_bytes_allocated;

};

} // namespace H3DUbitrack

#endif //IMAGESENDER_H_
