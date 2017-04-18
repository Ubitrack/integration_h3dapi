/*
 * UTImageTexture.cpp
 *
 *  Created on: Jul 30, 2010
 *      Author: jack
 */

#include "H3DUbitrack/UTImageTexture.h"

#include <H3D/Scene.h>

using namespace std;
using namespace H3DUbitrack;
using namespace H3D;

H3DNodeDatabase UTImageTexture::database("UTImageTexture",
		&(newInstance<UTImageTexture> ),
		typeid(UTImageTexture),
		&UTTextureNode::database);

namespace UTImageTextureInternals {
}

UTImageTexture::UTImageTexture(Inst<DisplayList> _displayList,
		Inst<SFNode> _metadata, Inst<SFBool> _repeatS, Inst<SFBool> _repeatT,
		Inst<SFBool> _scaleToP2, Inst<SFImage> _image,
		Inst<SFTextureProperties> _textureProperties)
	: UTTextureNode( _displayList, _metadata, _repeatS, _repeatT, _scaleToP2, _textureProperties),
    H3DImageObject( _image )
	, texture_id( 0 )
	, texture_unit( GL_TEXTURE0_ARB )
	, texture_target( 0 )
	, imageNeedsUpdate ( new Field )
	, ut_last_timestamp(0)
	, frame_bytes_allocated(0)
{
	type_name = "UTImageTexture";
	database.initFields( this );
	image->setName( "image" );
	image->setOwner( this );

	repeatS->setValue( true );
	repeatT->setValue( true );
	scaleToPowerOfTwo->setValue( true );

	image->route( displayList );
	repeatS->route( displayList );
	repeatT->route( displayList );
	scaleToPowerOfTwo->route( displayList );
	textureProperties->route( displayList );

	imageNeedsUpdate->setName( "ImageNeedsUpdate" );
	imageNeedsUpdate->setOwner( this );
	image->route( imageNeedsUpdate );
}

UTImageTexture::~UTImageTexture() {
	if( texture_id ) glDeleteTextures( 1, &texture_id );
}

void UTImageTexture::updateTexture(const Ubitrack::Measurement::ImageMeasurement& cvimg) {
	// save measurement for rendering
	ut_image = cvimg;
	repeatS->setValue(true, id);
}

Ubitrack::Measurement::ImageMeasurement& UTImageTexture::getImageMeasurement() {
	return ut_image;
}

void UTImageTexture::SFImage::setValueFromString( const string& s ) {
	setValue( X3D::X3DStringTo2DImage( s ) );
}

string UTImageTexture::SFImage::getValueAsString( const string& separator) {
	// Slow but convenient. is only used for JS engine

	stringstream ss;
	Image* img = getValue();
	if( img ) {
		ss << img->width() << separator << img->height()
		   << separator << (int)img->pixelType() + 1;
		for(unsigned int index = 0; index < img->width() * img->height(); ++index) {
			int ix = index % img->width();
			int iy = index / img->width();
			RGBA color = img->getPixel( ix, iy );
			unsigned char bytes_per_pixel = img->pixelType() + 1;
			unsigned char* data = new unsigned char[ bytes_per_pixel ];
			img->RGBAToImageValue(color, (void*) data);
			int intval = 0;
			for (int i = 0; i < bytes_per_pixel; ++i) {
				intval = intval << 8;
				intval = intval | data[i];
			}
			delete []data;
			ss<< separator << intval;
		}
	} else {
		ss << 0 << separator << 0
		   << separator << 1;
	}
	return ss.str();
}

GLint UTImageTexture::glInternalFormat( Image *i ) {
	TextureProperties *texture_properties = textureProperties->getValue();
	if( GLEW_ARB_texture_compression &&
			texture_properties &&
			texture_properties->textureCompression->getValue() != "DEFAULT" ) {
		switch( i->pixelType() ) {
		case Image::LUMINANCE:
			return GL_COMPRESSED_LUMINANCE_ARB;
		case Image::LUMINANCE_ALPHA:
			return GL_COMPRESSED_LUMINANCE_ALPHA_ARB;
		case Image::RGB:
		case Image::BGR:
			return GL_COMPRESSED_RGB_ARB;
		case Image::RGBA:
		case Image::BGRA:
			return GL_COMPRESSED_RGBA_ARB;
		default:
			return GL_RGBA;
		}
	} else {
		return X3DTextureNode::glInternalFormat( i );
	}
}

void UTImageTexture::glTexImage( Image *i, GLenum texture_target,
		bool scale_to_power_of_two ) {
	// the image data to render
	void *image_data = i->getImageData();
	// the width of the data of the image_data pointer
	unsigned int width = i->width();
	// the height of the data of the image_data pointer
	unsigned int height = i->height();
	// flag to determine if the image_data pointer should be deallocated
	// at the end of the function or not.
	bool free_image_data = false;

	GLint byte_alignment;
	glGetIntegerv( GL_UNPACK_ALIGNMENT, &byte_alignment );
	glPixelStorei( GL_UNPACK_ALIGNMENT, i->byteAlignment() );

	if( scale_to_power_of_two && !GLEW_ARB_texture_non_power_of_two ) {
		// check if any scaling is required and if so scale the image.
		bool needs_scaling = false;
		unsigned int new_width  = i->width();
		unsigned int new_height = i->height();

		if( !isPowerOfTwo( new_width ) ) {
			new_width = nextPowerOfTwo( new_width );
			needs_scaling = true;
		}

		if( !isPowerOfTwo( new_height ) ) {
			new_height = nextPowerOfTwo( new_height );
			needs_scaling = true;
		}
		if( needs_scaling ) {
			unsigned int bytes_per_pixel = i->bitsPerPixel();
			bytes_per_pixel =
					bytes_per_pixel % 8 == 0 ?
					bytes_per_pixel / 8 : bytes_per_pixel / 8 + 1;

			void * new_data = malloc( H3DMax( new_width, 4u )*
					new_height*bytes_per_pixel );
			gluScaleImage( glPixelFormat( i ),
					width,
					height,
					glPixelComponentType( i ),
					image_data,
					new_width,
					new_height,
					glPixelComponentType( i ),
					new_data );
			width = new_width;
			height = new_height;

			// remove the old image data if the image was padded before.
			if( free_image_data ) {
				free( image_data );
			} else {
				free_image_data = true;
			}
			image_data = new_data;
		}
	}

	TextureProperties *texture_properties = textureProperties->getValue();

	if( texture_properties ) {
		glPushAttrib( GL_PIXEL_MODE_BIT );
		const Vec4f &scale = texture_properties->textureTransferScale->getValue();
		glPixelTransferf( GL_RED_SCALE, scale.x );
		glPixelTransferf( GL_BLUE_SCALE, scale.y );
		glPixelTransferf( GL_GREEN_SCALE, scale.z );
		glPixelTransferf( GL_ALPHA_SCALE, scale.w );

		const Vec4f &bias = texture_properties->textureTransferBias->getValue();
		glPixelTransferf( GL_RED_BIAS, bias.x );
		glPixelTransferf( GL_BLUE_BIAS, bias.y );
		glPixelTransferf( GL_GREEN_BIAS, bias.z );
		glPixelTransferf( GL_ALPHA_BIAS, bias.w );
	}

	if( texture_properties && texture_properties->generateMipMaps->getValue() ) {
		gluBuild2DMipmaps(  texture_target,
				glInternalFormat( i ),
				width,
				height,
				glPixelFormat( i ),
				glPixelComponentType( i ),
				image_data );
	} else {
		H3DInt32 border_width =
				texture_properties ?
				texture_properties->borderWidth->getValue() : 0;

		if( border_width < 0 || border_width > 1 ) {
			Console(3) << "Warning: Invalid borderWidth \"" << border_width
					   << "\". Must be 0 or 1 (in " << getName()
					   << ")" << endl;
			border_width = 0;
		}

		// install the image as a 2d texture/
		glTexImage2D( texture_target,
				0, // mipmap level
				glInternalFormat( i ),
				width,
				height,
				border_width, // border
				glPixelFormat( i ),
				glPixelComponentType( i ),
				image_data );
	}

	glPixelStorei( GL_UNPACK_ALIGNMENT, byte_alignment );

	// restore scale and bias pixel transfer components
	if( texture_properties ) glPopAttrib();

	// free the temporary image data if necessary.
	if( free_image_data )
		free( image_data );
}

void UTImageTexture::render() {

	if (!ut_image) {
		return;
	}

	unsigned int required_frame_bytes = ut_image->width() * ut_image->height() * ut_image->channels();

	if (frame_bytes_allocated != required_frame_bytes) {
		Image::PixelType pt = Image::RGB;
		unsigned int bits = 24;

		switch (ut_image->pixelFormat()) {
		case Ubitrack::Vision::Image::LUMINANCE:
			pt = Image::LUMINANCE;
			bits = 8;
			break;
		case Ubitrack::Vision::Image::RGB:
			pt = Image::RGB;
			break;
		case Ubitrack::Vision::Image::BGR:
			pt = Image::BGR;
			break;
		case Ubitrack::Vision::Image::BGRA:
			pt = Image::BGRA;
			bits = 32;
			break;
		case Ubitrack::Vision::Image::RGBA:
			pt = Image::RGBA;
			bits = 32;
			break;
		default:
			// Log Error ?
			break;
		}

		image->setValue( new PixelImage( ut_image->width(),
				ut_image->height(),
				1,
				bits,
				pt,
				Image::UNSIGNED ) );

		frame_bytes_allocated = required_frame_bytes;

		// original render code
		glGetIntegerv( GL_ACTIVE_TEXTURE_ARB, &texture_unit );

		GLenum target = getTextureTarget();

		Image * i = static_cast< Image * >(image->getValue());
		if( displayList->hasCausedEvent( image ) || texture_target != target ) {

			if( !image->imageChanged() || texture_id == 0 || texture_target != target ) {
				// the image has changed so remove the old texture and install
				// the new
				glDeleteTextures( 1, &texture_id );
				texture_id = 0;
				texture_target = target;
				if( i ) {
					texture_id = renderImage( i,
							texture_target,
							scaleToPowerOfTwo->getValue() );

				}
			} else {
				glBindTexture(  texture_target, texture_id );
				renderSubImage( i, texture_target,
						image->xOffset(), image->yOffset(),
						image->changedWidth(),
						image->changedHeight() );
			}
			enableTexturing();
		} else {
			if ( texture_id ) {
				// same texture as last loop, so we just bind it.
				glBindTexture(  texture_target, texture_id );
				enableTexturing();
			}
		}

		if( i ) {
			renderTextureProperties();
		}
		imageNeedsUpdate->upToDate();
		// end original render code
	} else {
		PixelImage *pi = dynamic_cast<PixelImage *> (image->getValue());
		glBindTexture(GL_TEXTURE_2D, texture_id);
		enableTexturing();
		if (ut_last_timestamp < ut_image.time()) {
			// copy data because the texture could be updated less often than the
			// scene is rendered (true only for async capture devices)
			unsigned char* srcData = (unsigned char*) ut_image->Mat().data;
			unsigned char* dstData = (unsigned char*) pi->getImageData();
			memcpy(dstData, srcData, sizeof(unsigned char)*frame_bytes_allocated);
			renderSubImage(pi, texture_target, 0, 0,
					ut_image->width(), ut_image->height());
			ut_last_timestamp = ut_image.time();
		}
	}
}

void UTImageTexture::renderTextureProperties() {
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

void UTImageTexture::renderSubImage( Image *image, GLenum texture_target,
		int x_offset, int y_offset,
		int width, int height ) {
	// todo: is there a way to do this without copying data?
	unsigned char *image_data = (unsigned char *) image->getImageData();

	unsigned int bytes_per_pixel = image->bitsPerPixel() / 8;

	unsigned char *modified_data =
			new unsigned char[ width * height * bytes_per_pixel ];
	for( unsigned int i = 0; i < (unsigned int)height; ++i ) {
		memcpy( modified_data + i * width * bytes_per_pixel,
				image_data + ( (i + y_offset )* image->width() + x_offset) * bytes_per_pixel,
				width * bytes_per_pixel );
	}

	GLint byte_alignment;
	glGetIntegerv( GL_UNPACK_ALIGNMENT, &byte_alignment );
	glPixelStorei( GL_UNPACK_ALIGNMENT, image->byteAlignment() );

	TextureProperties *texture_properties = textureProperties->getValue();
	if( texture_properties ) {
		glPushAttrib( GL_PIXEL_MODE_BIT );
		const Vec4f &scale = texture_properties->textureTransferScale->getValue();
		glPixelTransferf( GL_RED_SCALE, scale.x );
		glPixelTransferf( GL_BLUE_SCALE, scale.y );
		glPixelTransferf( GL_GREEN_SCALE, scale.z );
		glPixelTransferf( GL_ALPHA_SCALE, scale.w );

		const Vec4f &bias = texture_properties->textureTransferBias->getValue();
		glPixelTransferf( GL_RED_BIAS, bias.x );
		glPixelTransferf( GL_BLUE_BIAS, bias.y );
		glPixelTransferf( GL_GREEN_BIAS, bias.z );
		glPixelTransferf( GL_ALPHA_BIAS, bias.w );
	}

	glTexSubImage2D( texture_target, 0,
			x_offset, y_offset,
			width, height,
			glPixelFormat( image ),
			glPixelComponentType( image ),
			modified_data );

	if( texture_properties ) glPopAttrib();

	delete [] modified_data;
}


void UTImageTexture::enableTexturing() {
	glEnable( texture_target );
	Image * i = static_cast< Image * >(image->getValue());
	if( i ) {
		// update blend state when image exist and image needs update
		Image::PixelType pixel_type = i->pixelType();
		if( pixel_type == Image::LUMINANCE_ALPHA ||
				pixel_type == Image::RGBA ||
				pixel_type == Image::BGRA ) {
			glEnable( GL_BLEND );
			glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
		}
	}
}

void UTImageTexture::disableTexturing() {
	glDisable( texture_target );
	Image * i = static_cast< Image * >(image->getValue());
	if( i ) {
		Image::PixelType pixel_type = i->pixelType();
		if( pixel_type == Image::LUMINANCE_ALPHA ||
				pixel_type == Image::RGBA ||
				pixel_type == Image::BGRA ) {
			glDisable( GL_BLEND );
		}
	}
}


GLenum UTImageTexture::getTextureTarget() {
	TextureProperties *texture_properties = textureProperties->getValue();
	if( texture_properties ) {
		const string &target_type = texture_properties->textureType->getValue();
		if( target_type == "RECTANGLE" ) return GL_TEXTURE_RECTANGLE_ARB;
		else if( target_type == "2DARRAY" ) {
			Console(3) << "Warning: Invalid textureType \"2DARRAY\" in TextureProperties for "
					   << "UTImageTexture. \"2DARRAY\" can only be used for 3D textures" << endl;
		} else if( target_type != "NORMAL" ) {
			Console(3) << "Warning: Invalid textureType: \"" << target_type << "\" in TextureProperties for "
					   << "UTImageTexture. " << endl;
		}
	}
	return GL_TEXTURE_2D;
}

std::pair<H3DInt32,H3DInt32> UTImageTexture::getDefaultSaveDimensions () {
	Image* im= image->getValue();
	if ( im ) {
		return std::pair<H3DInt32,H3DInt32> ( im->width(), im->height() );
	} else {
		return X3DTextureNode::getDefaultSaveDimensions();
	}
}
