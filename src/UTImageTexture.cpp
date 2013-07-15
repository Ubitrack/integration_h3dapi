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
		Inst<SFTextureProperties> _textureProperties) :
	X3DTexture2DNode(_displayList, _metadata, _repeatS, _repeatT, _scaleToP2,
					_image, _textureProperties) {
	type_name = "UTImageTexture";
	database.initFields(this);

}

UTImageTexture::~UTImageTexture() {
	// clean up
}


void UTImageTexture::updateTexture(const Ubitrack::Measurement::ImageMeasurement& m) {
    PixelImage *i = dynamic_cast< PixelImage * >( image->getValue() );
	Ubitrack::Vision::Image* cvimg = m.get();
    if( !i ) {
        Image::PixelType pt = Image::RGB;
        unsigned int bits = 24;
    	switch ( cvimg->nChannels ) {
    		case 1: pt = Image::LUMINANCE;
    				 break;
    		case 3:
    			if (cvimg->channelSeq[ 0 ] == 'B' && cvimg->channelSeq[ 1 ] == 'G' && cvimg->channelSeq[ 2 ] == 'R' )
    				pt = Image::BGR;
    			else
    				pt = Image::RGB;
    			break;
    		default:
    			break;
    	}

  	    Console(3) << "DBG image: w,h: " << cvimg->width << ", " << cvimg->height << ";" << std::endl;
		image->setValue( new PixelImage( cvimg->width,
										  cvimg->height,
										   1,
										   bits,
										   pt,
										   Image::UNSIGNED,
										   (unsigned char*)cvimg->imageData ) );

    } else {
	  unsigned int nbytes = i->bitsPerPixel() / 8;
	  unsigned int frame_size = cvimg->width * cvimg->height * nbytes;

	  // copy the new image data to the image.
	  memcpy( i->getImageData(), (unsigned char*)cvimg->imageData, frame_size );

	  // set the edited area to be the whole texture
	  image->setEditedArea( 0, 0, 0,
			  cvimg->width - 1,
			  cvimg->height - 1,
			  0 );

	  // send an event that the image has been changed.
	  image->endEditing();

    }
}
