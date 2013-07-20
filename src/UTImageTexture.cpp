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
	IplImage* cvimg = *m;
    PixelImage *i = dynamic_cast< PixelImage * >( image->getValue() );
    if( !i ) {
        Image::PixelType pt = Image::RGB;
        unsigned int bits = 24;
    	switch ( cvimg->nChannels ) {
    		case 1: pt = Image::LUMINANCE;
    				 bits = 8;
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

  	    //Console(4) << "DBG image: w,h: " << cvimg->width << ", " << cvimg->height << ";" << std::endl;
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
	  if (cvimg->imageSize != frame_size) {
		    Console(4) << "problem with image size: w,h: " << cvimg->width << ", " << cvimg->height << ";" <<
		    		" pixelimage: " << i->width() <<  ", " << i->height() << ";" <<
		    		" framesize: " << frame_size << ", imageSize: " << cvimg->imageSize << std::endl;
	  } else {
		  //Console(4) << "store image: w,h: " << cvimg->width << ", " << cvimg->height << ";" <<
		  //  		" pixelimage: " << i->width() <<  ", " << i->height() << ";" <<
		  //  		" framesize: " << frame_size << ", imageSize: " << cvimg->imageSize << std::endl;

		  // XXX WRITES CRAP SOMEHOW ..
		  // copy the new image data to the image.
		  memcpy( (unsigned char*)(i->getImageData()), (unsigned char*)(cvimg->imageData), cvimg->imageSize );

		  //Console(4) << "Vision::Image copied to PixelImage." << std::endl;

		  // set the edited area to be the whole texture
		  image->setEditedArea( 0, 0, 0,
				  cvimg->width - 1,
				  cvimg->height - 1,
				  0 );

		  // send an event that the image has been changed.
		  image->endEditing();
	  }


    }
}
