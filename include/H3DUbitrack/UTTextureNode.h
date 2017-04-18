//
// Created by Ulrich Eck on 13.04.17.
//

#ifndef H3D_UTTEXTURENODE_H
#define H3D_UTTEXTURENODE_H

#include "H3DUbitrack/H3DUbitrack.h"

#include <utVision/Image.h>

#include <H3D/H3DSingleTextureNode.h>
#include <H3D/TextureProperties.h>
#include <H3D/DependentNodeFields.h>
#include <H3D/SFNode.h>

namespace H3DUbitrack {
using namespace H3D;

// Abstract Interface to support ImageMeasurement Updates
class UTTextureNode : public H3D::H3DSingleTextureNode {
public:

    /// The SFTextureProperties is dependent on the propertyChanged field of
    /// the contained TextureProperties.
    typedef DependentSFNode <FieldRef<TextureProperties,
                                      Field,
                                      &TextureProperties::propertyChanged> >
            SFTextureProperties;


    UTTextureNode(
            H3D::Inst< DisplayList > _displayList = 0,
            H3D::Inst< H3D::SFNode  > _metadata  = 0,
            Inst<SFBool> _repeatS = 0,
            Inst<SFBool> _repeatT = 0,
            Inst<SFBool> _scaleToP2 = 0,
            Inst<SFTextureProperties> _textureProperties = 0) :
                H3D::H3DSingleTextureNode( _displayList, _metadata ),
                repeatS ( _repeatS  ),
                repeatT ( _repeatT  ),
                scaleToPowerOfTwo( _scaleToP2 ),
                textureProperties( _textureProperties )
    {
        database.initFields( this );
    }

    // called to update the image measurement on a subclass
    virtual void updateTexture(const Ubitrack::Measurement::ImageMeasurement& cvimg) =0;

    virtual Ubitrack::Measurement::ImageMeasurement& getImageMeasurement() = 0;


    /// If true the texture will repeat itself when the s texture coordinate
    /// is outside the range [0,1]. If false the texture will be clamped if
    /// outside the same range.
    ///
    /// <b>Access type:</b> inputOutput \n
    /// <b>Default value:</b> TRUE \n
    ///
    /// \dotfile UTTexture_repeatS.dot
    auto_ptr <SFBool> repeatS;

    /// If true the texture will repeat itself when the t texture coordinate
    /// is outside the range [0,1]. If false the texture will be clamped if
    /// outside the same range.
    ///
    /// <b>Access type:</b> inputOutput \n
    /// <b>Default value:</b> TRUE \n
    ///
    /// \dotfile UTTexture_repeatT.dot
    auto_ptr <SFBool> repeatT;

    /// If true the image used will be scaled so that the dimensions are a
    /// power of two if they are not. This will however take up more memory
    /// and might cause some unwanted strething effects on the texture. The
    /// new texture values will be linearly interpolated from the original
    /// ones.
    ///
    /// <b>Access type:</b> inputOutput \n
    /// <b>Default value:</b> TRUE \n
    ///
    /// \dotfile UTTexture_image.dot
    auto_ptr <SFBool> scaleToPowerOfTwo;

    /// The textureProperties field contains a TextureProperties node
    /// which allows fine control over a texture's application.
    ///
    /// <b>Access type:</b> inputOutput \n
    ///
    /// \dotfile UTTexture_textureProperties.dot
    auto_ptr <SFTextureProperties> textureProperties;

    /// The H3DNodeDatabase for this node.
    static H3DNodeDatabase database;

};
}

#endif //H3D_UTTEXTURENODE_H
