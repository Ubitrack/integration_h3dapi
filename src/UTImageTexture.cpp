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
		&X3DTexture2DNode::database);

namespace UTImageTextureInternals {
 FIELDDB_ELEMENT(UTImageTexture, repeatS, INPUT_OUTPUT );
}

UTImageTexture::UTImageTexture(Inst<DisplayList> _displayList,
		Inst<SFNode> _metadata, Inst<SFBool> _repeatS, Inst<SFBool> _repeatT,
		Inst<SFBool> _scaleToP2, Inst<SFImage> _image,
		Inst<SFTextureProperties> _textureProperties)
	: X3DTexture2DNode(_displayList, _metadata, _repeatS, _repeatT, _scaleToP2,
					_image, _textureProperties)
        , ut_last_timestamp(0)
	, frame_bytes_allocated(0)
	{
	type_name = "UTImageTexture";
	database.initFields(this);

}

UTImageTexture::~UTImageTexture() {
	// clean up
}

void UTImageTexture::render() {

	if (!ut_image) {
		return;
	}

	unsigned int required_frame_bytes = ut_image->width * ut_image->height * ut_image->nChannels;

	if (frame_bytes_allocated != required_frame_bytes) {
        Image::PixelType pt = Image::RGB;
        unsigned int bits = 24;

    	switch ( ut_image->nChannels ) {
    		case 1: pt = Image::LUMINANCE;
    				 bits = 8;
					 //H3D::Console << "Grayscale Texture" << std::endl;
    				 break;
    		case 3:
    				if (ut_image->channelSeq[ 0 ] == 'B' && ut_image->channelSeq[ 1 ] == 'G' && ut_image->channelSeq[ 2 ] == 'R' ) {
    					pt = Image::BGR;
						//H3D::Console << "BGR Texture" << std::endl;
					} else {
						//H3D::Console << "RGB Texture" << std::endl;
					}
    				break;
    		default:
    				break;
    	}

		image->setValue( new PixelImage( ut_image->width,
										  ut_image->height,
										  1,
										  bits,
										  pt,
										  Image::UNSIGNED ) );

		frame_bytes_allocated = required_frame_bytes;

		X3DTexture2DNode::render();
	} else {
		PixelImage *pi = dynamic_cast<PixelImage *> (image->getValue());
		glBindTexture(GL_TEXTURE_2D, texture_id);
		enableTexturing();
		if (ut_last_timestamp < ut_image.time()) {
			// copy data because the texture could be updated less often than the
			// scene is rendered (true only for async capture devices)
			unsigned char* srcData = (unsigned char*) ut_image->imageData;
			unsigned char* dstData = (unsigned char*) pi->getImageData();
			memcpy(dstData, srcData, sizeof(unsigned char)*frame_bytes_allocated);
			renderSubImage(pi, texture_target, 0, 0,
							 ut_image->width, ut_image->height);
			enableTexturing();
			ut_last_timestamp = ut_image.time();
		}
	}
}


void UTImageTexture::updateTexture(const Ubitrack::Measurement::ImageMeasurement& cvimg) {
	// save measurement for rendering
	ut_image = cvimg;
	repeatS->setValue(true, id);
}

void UTImageTexture::drawAsStencilBuffer() {

	// Inserted this line to reset the position of the raster
	// so that the entire stencil_mask will be copied into the stencil
	// buffer by glDrawPixels.  This ensures that even if user calls such
	// as glDrawPixels(), glCopyPixels(), or glBindFramebufferEXT()
	// leave glRasterPos in the middle of the screen, the entire stencil
	// stencil_mack still gets copied to the stencil buffery by
	// glDrawPixels()
	glWindowPos2f(0.000000f, 0.000000f);

	// what is the correct width/height .. screen or texture ?
	glDrawPixels( ut_image->width, ut_image->height,
				  GL_STENCIL_INDEX, GL_UNSIGNED_BYTE, ut_image->imageData );
}