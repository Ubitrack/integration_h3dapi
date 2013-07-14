/*
 * UTImageTexture.h
 *
 *  Created on: Jul 30, 2010
 *      Author: jack
 */

#ifndef UTIMAGETEXTURE_H_
#define UTIMAGETEXTURE_H_


#include "H3DUbitrack/H3DUbitrack.h"

#include <H3D/X3DTexture2DNode.h>
#include <H3D/SFNode.h>
#include <H3DUtil/Threads.h>
#include <utMeasurement/Measurement.h>
#include <utComponents/ApplicationEndpointsVision.h>
#include <utVision/Image.h>


namespace H3DUbitrack {
  using namespace H3D;

  /// \ingroup Nodes
  /// \class UTImageTexture
  class H3DUBITRACK_API UTImageTexture : public X3DTexture2DNode {
  public:

    /// Constructor.
    UTImageTexture(
                 Inst< DisplayList > _displayList = 0,
                 Inst< SFNode      > _metadata    = 0,
                 Inst< SFBool      > _repeatS     = 0,
                 Inst< SFBool      > _repeatT     = 0,
                 Inst< SFBool      > _scaleToP2   = 0,
                 Inst< SFImage     > _image       = 0,
                 Inst< SFTextureProperties > _textureProperties = 0);

    virtual ~UTImageTexture();

    void updateTexture(const Ubitrack::Measurement::ImageMeasurement& m);

    /// The H3DNodeDatabase for this node.
    static H3DNodeDatabase database;

  };
}
#endif /* UTIMAGETEXTURE_H_ */
