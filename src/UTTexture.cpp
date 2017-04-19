//
// Created by Ulrich Eck on 13.04.17.
//


#include <H3DUbitrack/UTTexture.h>

using namespace H3DUbitrack;
using namespace H3D;

H3DNodeDatabase UTTexture::database(
        "UTTexture",
        &(newInstance<UTTexture> ),
        typeid( UTTexture ),
        &UTTextureNode::database
);

namespace UTTextureInternals {
FIELDDB_ELEMENT( UTTexture, texCoord, OUTPUT_ONLY );
}

UTTexture::UTTexture(
        Inst< DisplayList > _displayList,
        Inst< SFNode  > _metadata,
        Inst< SFBool  > _repeatS,
        Inst< SFBool  > _repeatT,
        Inst< SFBool  > _scaleToP2,
        Inst< SFVec2d > _texCoord,
        Inst< SFTextureProperties > _textureProperties) :
        UTTextureNode( _displayList, _metadata, _repeatS, _repeatT, _scaleToP2, _textureProperties),
        texCoord( _texCoord ),
        texture_id( 0 ),
        texture_unit( GL_TEXTURE0_ARB ),
        texture_target( 0 ),
        ut_last_timestamp(0),
        frame_bytes_allocated(0),
		ut_image(NULL) {

    type_name = "UTTexture";
    database.initFields( this );

    repeatS->setValue( true, id );
    repeatT->setValue( true, id );
    scaleToPowerOfTwo->setValue( true, id );

	texCoord->setValue(Vec2d(1., 1.), id);

    //repeatS->route( displayList, id );
    //repeatT->route( displayList, id );
    //scaleToPowerOfTwo->route( displayList, id );
    //textureProperties->route( displayList, id );
}

GLint UTTexture::glInternalFormat( const Ubitrack::Measurement::ImageMeasurement& cvimg ) {
    TextureProperties *texture_properties = textureProperties->getValue();
    if( GLEW_ARB_texture_compression &&
            texture_properties &&
            texture_properties->textureCompression->getValue() != "DEFAULT" ) {
        if (ut_image) {
            switch( cvimg->pixelFormat() ) {
            case Ubitrack::Vision::Image::LUMINANCE:
                return GL_COMPRESSED_LUMINANCE_ARB;
//        case Image::LUMINANCE_ALPHA:
//            return GL_COMPRESSED_LUMINANCE_ALPHA_ARB;
            case Ubitrack::Vision::Image::RGB:
            case Ubitrack::Vision::Image::BGR:
                return GL_COMPRESSED_RGB_ARB;
            case Ubitrack::Vision::Image::RGBA:
            case Ubitrack::Vision::Image::BGRA:
                return GL_COMPRESSED_RGBA_ARB;
            default:
                return GL_RGBA;
            }
        }
    }
    return GL_RGBA;
}

void UTTexture::render()     {

    if (!ut_image) {
        return;
    }

    glGetIntegerv( GL_ACTIVE_TEXTURE_ARB, &texture_unit );
    GLenum target = getTextureTarget();

    // imageNeedsUpdate signal detection does not seem to work ...
    if( (texture_target != target ) ||
         (!ut_textureUpdate.isInitialized()) ) {
        texture_target = target;

        unsigned int required_frame_bytes = ut_image->width() * ut_image->height() * ut_image->channels();

        if ((frame_bytes_allocated != required_frame_bytes) ||
            (!ut_textureUpdate.isInitialized())) {
            // @todo check here it might be already initialized ..
            ut_textureUpdate.initializeTexture(ut_image);
            texture_id = ut_textureUpdate.textureId();

            unsigned int pow2width = ut_textureUpdate.pow2width();
            unsigned int pow2height = ut_textureUpdate.pow2height();
            double tx = double( ut_image->width() ) / pow2width;
            double ty = double( ut_image->height() ) / pow2height;
            Console(3) << "Init TexCoords: " << pow2width << ", " << pow2height
                       << " / " << tx << ", " << ty << std::endl;

            // setting a field during render isn't optimal ...
            texCoord->setValue(Vec2d(tx, ty), id);

        }

    }

    glBindTexture(texture_target, texture_id);
    if ((texture_id) && (ut_textureUpdate.isInitialized()))  {
        if (ut_image.time() > ut_last_timestamp ) {
            ut_textureUpdate.updateTexture(ut_image);
        }
		enableTexturing();
		renderTextureProperties();
    }
}

void UTTexture::renderTextureProperties() {
    TextureProperties *texture_properties = textureProperties->getValue();
    if( texture_properties ) {
        texture_properties->renderTextureProperties( texture_target );
    } else {
        // set up texture parameters 
        if ( repeatS->getValue() )
            glTexParameteri( texture_target, GL_TEXTURE_WRAP_S, GL_REPEAT );
        else
            glTexParameteri( texture_target,
                    GL_TEXTURE_WRAP_S,
                    GL_CLAMP_TO_EDGE );
        if ( repeatT->getValue() )
            glTexParameteri( texture_target, GL_TEXTURE_WRAP_T, GL_REPEAT );
        else
            glTexParameteri( texture_target, GL_TEXTURE_WRAP_T,
                    GL_CLAMP_TO_EDGE );
        glTexParameteri( texture_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri( texture_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    }
}

void UTTexture::enableTexturing() {
    glEnable( texture_target );
    if( ut_image ) {
        // update blend state when image exist and image needs update
        Ubitrack::Vision::Image::PixelFormat pixel_type = ut_image->pixelFormat();
        if( // pixel_type == Image::LUMINANCE_ALPHA ||
              pixel_type == Ubitrack::Vision::Image::RGBA ||
              pixel_type == Ubitrack::Vision::Image::BGRA ) {
            glEnable( GL_BLEND );
            glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
        }
    }
}

void UTTexture::disableTexturing() {
    glDisable( texture_target );
    if( ut_image ) {
        Ubitrack::Vision::Image::PixelFormat pixel_type = ut_image->pixelFormat();
        if( //pixel_type == Image::LUMINANCE_ALPHA ||
                pixel_type == Ubitrack::Vision::Image::RGBA ||
                pixel_type == Ubitrack::Vision::Image::BGRA ) {
            glDisable( GL_BLEND );
        }
    }
}


GLenum UTTexture::getTextureTarget() {
    TextureProperties *texture_properties = textureProperties->getValue();
    if( texture_properties ) {
        const string &target_type = texture_properties->textureType->getValue();
        if( target_type == "RECTANGLE" ) return GL_TEXTURE_RECTANGLE_ARB;
        else if( target_type == "2DARRAY" ) {
            Console(3) << "Warning: Invalid textureType \"2DARRAY\" in TextureProperties for "
                       << "UTTexture. \"2DARRAY\" can only be used for 3D textures" << endl;
        } else if( target_type != "NORMAL" ) {
            Console(3) << "Warning: Invalid textureType: \"" << target_type << "\" in TextureProperties for "
                       << "UTTexture. " << endl;
        }
    }
    return GL_TEXTURE_2D;
}

std::pair<H3DInt32,H3DInt32> UTTexture::getDefaultSaveDimensions () {
    if ( ut_textureUpdate.isInitialized() ) {
        // this should probably be image dimensions here ..
        return std::pair<H3DInt32,H3DInt32> ( ut_textureUpdate.pow2width(), ut_textureUpdate.pow2height() );
    } else {
        return X3DTextureNode::getDefaultSaveDimensions();
    }
}

void UTTexture::updateTexture(const Ubitrack::Measurement::ImageMeasurement& cvimg) {
    // save measurement for rendering
    ut_image = cvimg;
}

Ubitrack::Measurement::ImageMeasurement& UTTexture::getImageMeasurement() {
    return ut_image;
}