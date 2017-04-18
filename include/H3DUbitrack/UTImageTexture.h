/*
 * UTImageTexture.h
 *
 *  Created on: Jul 30, 2010
 *      Author: jack
 */

#ifndef UTIMAGETEXTURE_H_
#define UTIMAGETEXTURE_H_


#include "H3DUbitrack/H3DUbitrack.h"
#include "H3DUbitrack/UTTextureNode.h"

#include <H3D/H3DSingleTextureNode.h>
#include <H3D/H3DImageObject.h>
#include <H3D/TextureProperties.h>
#include <H3D/DependentNodeFields.h>

#include <H3D/SFNode.h>
#include <H3DUtil/Threads.h>

#include <utMeasurement/Measurement.h>
#include <utComponents/ApplicationEndpointsVision.h>
#include <utVision/Image.h>


namespace H3DUbitrack {
  using namespace H3D;

  /// \ingroup Nodes
  /// \class UTImageTexture
  class H3DUBITRACK_API UTImageTexture : public UTTextureNode, public H3DImageObject {
  public:

    /// A SFNode encapsulating an Image class
    class H3DAPI_API SFImage: public H3DImageObject::SFImage {
    public:
        virtual void setValueFromString( const string &s );

        virtual string getValueAsString(const string& separator = " ");

        virtual X3DTypes::X3DType getX3DType() { return X3DTypes::SFIMAGE; }
    };


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

    virtual void updateTexture(const Ubitrack::Measurement::ImageMeasurement& cvimg);

	virtual Ubitrack::Measurement::ImageMeasurement& getImageMeasurement();

    /// Performs the OpenGL rendering required to install the image
    /// as a texture.
    virtual void render();

    /// Render all OpenGL texture properties.
    virtual void renderTextureProperties();

    /// Get the OpenGL texture id that is used for this texture.
    virtual GLuint getTextureId() {
        return texture_id;
    }

    /// Get the OpenGL texture unit that is used for this texture.
    virtual GLuint getTextureUnit() {
        return texture_unit;
    }

    /// Get the OpenGL texture target that is used for this texture.
    virtual GLenum getTextureTarget();

    /// Virtual function for making all OpenGL calls that are needed to
    /// enable texturing for the texture.
    ///
    virtual void enableTexturing();

    /// Virtual function for making all OpenGL calls that are needed to
    /// disable texturing for the texture.
    ///
    virtual void disableTexturing();

    /// Returns the internal OpenGL format to use given an Image
    virtual GLint glInternalFormat( Image *image );

    /// Installs the given image as a OpenGL texture with a call to
    /// the glTexImage2D function. This function is used by renderImage ()
    /// and uses the glInternalFormat (), glPixelFormat () and
    /// glPixelComponentType () functions to get the arguments to the
    /// glTexImage2D call.
    ///
    virtual void glTexImage( Image *image, GLenum texture_target,
            bool scale_to_power_of_two );

    /// Replaces part of the current texture from an image.
    virtual void renderSubImage( Image *image, GLenum texture_target,
            int xoffset, int yoffset,
            int width, int height );

    /// The H3DNodeDatabase for this node.
    static H3DNodeDatabase database;

  protected:

    Ubitrack::Measurement::ImageMeasurement ut_image;
    unsigned long long 					    ut_last_timestamp;
    unsigned int							frame_bytes_allocated;

    /// Returns the default dimensions to use when this texture is saved to file.
    ///
    /// Returns the dimensions of the Image object
    ///
    virtual std::pair<H3DInt32,H3DInt32> getDefaultSaveDimensions ();

    /// The OpenGL texture id of the installed texture, 0 if not installed.
    GLuint texture_id;
    /// The OpenGL texture unit that is used to render this texture.
    GLint texture_unit;
    /// The OpenGL texture target that is used to render this texture.
    GLenum texture_target;

    /// Field to indicate image will change
    auto_ptr< Field > imageNeedsUpdate;


  };
}
#endif /* UTIMAGETEXTURE_H_ */
