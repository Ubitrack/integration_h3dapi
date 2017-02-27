/*
 * UbitrackContext.h
 *
 *  Created on: 03/08/2010
 *      Author: mvl
 */

#ifndef IMAGESENDER_H_
#define IMAGESENDER_H_

#include <H3D/X3DNode.h>

#include <H3D/H3DImageObject.h>
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

        class H3DAPI_API SFImage : public H3D::RefCountSField <H3D::Image, H3D::ParsableField> {

        public:
            /// Constructor.
            SFImage() {
            }

        protected:
            virtual void setValueFromString(const string& s)
            {
                setValue(H3D::X3D::X3DStringTo3DImage(s));
            }
        };

    ImageSender( H3D::Inst<H3D::SFNode> _metadata = 0,
            H3D::Inst<H3D::SFString> _pattern = 0,
            H3D::Inst<SFImage> _image = 0
    );

    std::auto_ptr <SFImage> value;

    /// Add this node to the H3DNodeDatabase system.
    static H3D::H3DNodeDatabase database;

    Ubitrack::Measurement::ImageMeasurement getMeasurement(H3D::TraverseInfo& ti, unsigned long long ts);

protected:
    boost::shared_ptr<Ubitrack::Vision::Image> ut_image;
    unsigned int							   frame_bytes_allocated;

};

} // namespace H3DUbitrack

#endif //IMAGESENDER_H_
