//
// Created by Ulrich Eck on 13.04.17.
//

#ifndef H3D_UTTEXTURE_H
#define H3D_UTTEXTURE_H

#include "H3DUbitrack/H3DUbitrack.h"
#include "H3DUbitrack/UTTextureNode.h"

#include <H3D/SFNode.h>
#include <H3D/SFVec2d.h>
#include <H3DUtil/Threads.h>
#include <utMeasurement/Measurement.h>
#include <utComponents/ApplicationEndpointsVision.h>
#include <utVision/Image.h>
#include <utVision/TextureUpdate.h>

#include <H3D/H3DSingleTextureNode.h>
#include <H3D/TextureProperties.h>
#include <H3D/DependentNodeFields.h>


namespace H3DUbitrack {
using namespace H3D;

class H3DUBITRACK_API UTTexture : public UTTextureNode {
public:

    /// Constructor.
    UTTexture( Inst<DisplayList> _displayList = 0,
            Inst<SFNode> _metadata = 0,
            Inst<SFBool> _repeatS = 0,
            Inst<SFBool> _repeatT = 0,
            Inst<SFBool> _scaleToP2 = 0,
            Inst<SFVec2d> _texCoord = 0,
            Inst<SFTextureProperties> _textureProperties = 0
    );

    /// Destructor.
    ~UTTexture()
    {
        if (texture_id) glDeleteTextures(1, &texture_id);
    }

    /// Performs the OpenGL rendering required to install the image
    /// as a texture.
    virtual void render();

    /// Render all OpenGL texture properties.
    virtual void renderTextureProperties();

    /// Get the OpenGL texture id that is used for this texture.
    virtual GLuint getTextureId()
    {
        return texture_id;
    }

    /// Get the OpenGL texture unit that is used for this texture.
    virtual GLuint getTextureUnit()
    {
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
    virtual GLint glInternalFormat( const Ubitrack::Measurement::ImageMeasurement& cvimg );

    bool isInitialized() const {
        return (frame_bytes_allocated > 0);
    }

    virtual void updateTexture(const Ubitrack::Measurement::ImageMeasurement& cvimg);

    virtual Ubitrack::Measurement::ImageMeasurement& getImageMeasurement();

    auto_ptr <SFVec2d> texCoord;

    /// The H3DNodeDatabase for this node.
    static H3DNodeDatabase database;

protected:

    /// Returns the default dimensions to use when this texture is saved to file.
    ///
    /// Returns the dimensions of the Image object
    ///
    virtual std::pair <H3DInt32, H3DInt32> getDefaultSaveDimensions();

    /// The OpenGL texture id of the installed texture, 0 if not installed.
    GLuint texture_id;
    /// The OpenGL texture unit that is used to render this texture.
    GLint texture_unit;
    /// The OpenGL texture target that is used to render this texture.
    GLenum texture_target;

    Ubitrack::Vision::TextureUpdate         ut_textureUpdate;
    Ubitrack::Measurement::ImageMeasurement ut_image;
    unsigned long long 					    ut_last_timestamp;
    unsigned int							frame_bytes_allocated;



};

}

#endif //H3D_UTTEXTURE_H
