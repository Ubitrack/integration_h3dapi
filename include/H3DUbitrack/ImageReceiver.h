/*
 * UbitrackContext.h
 *
 *  Created on: 03/08/2010
 *      Author: mvl
 */

#ifndef POSERECEIVER_H_
#define POSERECEIVER_H_


#include <H3D/X3DNode.h>
#include <H3D/SFNode.h>

#include <H3D/X3DTexture2DNode.h>
#include <H3D/SFString.h>
#include <H3D/SFBool.h>
#include <H3D/SField.h>
#include <H3D/SFNode.h>

#include <H3DUbitrack/H3DUbitrack.h>
#include <H3DUbitrack/MeasurementReceiver.h>
#include <utComponents/ApplicationEndpointsVision.h>
#include <H3DUbitrack/UTTextureNode.h>

#include <utFacade/AdvancedFacade.h>
#include <utMeasurement/Measurement.h>


#include <vector>

using namespace Ubitrack::Facade;

namespace H3DUbitrack {

typedef MeasurementReceiver< Ubitrack::Measurement::ImageMeasurement , Ubitrack::Components::ApplicationPullSinkVisionImage > ImageReceiverBase;

class H3DUBITRACK_API ImageReceiver : public ImageReceiverBase  {

public:

	typedef H3D::TypedSFNode< UTTextureNode > SFUTTextureNode;

    ImageReceiver(  H3D::Inst< H3D::SFNode     > _metadata = 0,
            		H3D::Inst< H3D::SFString   > _pattern = 0,
                    H3D::Inst< H3D::SFBool     > _isActive = 0,
                    H3D::Inst< H3D::SFBool     > _isSyncSource = 0,
        	        H3D::Inst< H3D::SFBool     > _isDataAvailable = 0,
                    H3D::Inst< MeasurementMode > _mode = 0,
                    H3D::Inst< SFUTTextureNode > _texture = 0
                    );

    // the texture
    std::auto_ptr< SFUTTextureNode > texture;

    /// Add this node to the H3DNodeDatabase system.
    static H3D::H3DNodeDatabase database;

    void updateMeasurement(const Ubitrack::Measurement::ImageMeasurement& m);
};

} // namespace H3DUbitrack

#endif //POSERECEIVER_H_
