#include <H3DUbitrack/UTStencilBuffer.h>

using namespace H3D;
using namespace H3DUbitrack;


H3DNodeDatabase UTStencilBuffer::database
( "UTStencilBuffer",
  &(newInstance<UTStencilBuffer>),
  typeid( UTStencilBuffer ),
  &X3DChildNode::database );

namespace UTStencilBufferInternals {
	FIELDDB_ELEMENT( UTStencilBuffer, texture, INPUT_OUTPUT );
	FIELDDB_ELEMENT( UTStencilBuffer, horizontalFlip, INPUT_OUTPUT );
	FIELDDB_ELEMENT( UTStencilBuffer, verticalFlip, INPUT_OUTPUT );
}

void UTStencilBuffer::MFImageTextureNode::onAdd( Node *n) {
  MFImageTextureNode_t::onAdd(n);
  UTStencilBuffer *parent =
    static_cast<UTStencilBuffer*>(owner);
  UTImageTexture *t = dynamic_cast<UTImageTexture*>(n);

  t->repeatS->route(parent->displayList);

}

void UTStencilBuffer::MFImageTextureNode::onRemove( Node *n) {
  MFImageTextureNode_t::onRemove(n);

  if( n == NULL ){ return; }

  UTStencilBuffer *parent = static_cast<UTStencilBuffer*>(owner);
  UTImageTexture *t = dynamic_cast<UTImageTexture*>(n);

  t->repeatS->unroute(parent->displayList);
}


UTStencilBuffer::UTStencilBuffer(Inst< SFNode    > _metadata,
                                      Inst< DisplayList > _displayList,
                                      Inst< MFImageTextureNode  > _texture,
									  Inst< SFBool > _horizontalFlip,
									  Inst< SFBool > _verticalFlip)
  : X3DChildNode( _metadata )
  , H3DDisplayListObject( _displayList )
  , texture( _texture )
  , horizontalFlip(_horizontalFlip)
  , verticalFlip(_verticalFlip)
{

  type_name = "UTStencilBuffer";
  database.initFields( this );
  displayList->setOwner( this );

  displayList->touch();

  texture->route( displayList );
  horizontalFlip->route( displayList );
  verticalFlip->route( displayList );
}



void UTStencilBuffer::render() {
	X3DChildNode::render();
	Vec4f viewport = Vec4f(0, 1, 0, 1);
	Viewpoint::EyeMode eye_mode = Viewpoint::MONO;
	int texture_idx = 0;

	if (texture->size() > 1) {
		UTCameraViewpoint *cvp = dynamic_cast<UTCameraViewpoint *>(UTCameraViewpoint::getActive());
		if (cvp) {
			viewport = cvp->getViewport();
			eye_mode = cvp->getCurrentEye();
		}
		if (eye_mode == Viewpoint::RIGHT_EYE) {
			texture_idx = 1;
		}

	} else if (!(texture->size() > 0)) {
		return;
	}
	UTImageTexture *_texture = dynamic_cast<UTImageTexture *>(texture->getValueByIndex(texture_idx, id));

	glEnable(GL_STENCIL_TEST);

    glStencilFunc(GL_ALWAYS, 1, 0xFF); // Set any stencil to 1
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glStencilMask(0xFF); // Write to stencil buffer
	glDepthMask(GL_FALSE); // Don't write to depth buffer
	glClear(GL_STENCIL_BUFFER_BIT); // Clear stencil buffer (0 by default)

	glPushAttrib( GL_ALL_ATTRIB_BITS );

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	glOrtho(viewport.x, viewport.y, viewport.z, viewport.w, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	
	glDisable(GL_BLEND);

	//glCullFace( GL_BACK );
	//glEnable( GL_CULL_FACE );
	glDisable( GL_LIGHTING );


	glColor4f( 1, 1, 1, 1 );

	int b, t, l, r;
	bool hflip = horizontalFlip->getValue( id );
	bool vflip = verticalFlip->getValue( id );

	t = vflip ? 0 : 1;
	b = vflip ? 1 : 0;
	l = hflip ? 0 : 1;
	r = hflip ? 1 : 0;

	if( _texture  ) {
		_texture->preRender();
		_texture->displayList->callList();
		glBegin( GL_QUADS );
		//glNormal3f( 0, 0, 1 );
		renderTexCoordForTexture( Vec3f( b, r, 0. ), _texture );
		glVertex2i( (GLint)viewport.x, (GLint)viewport.z );
		renderTexCoordForTexture( Vec3f( t, r, 0. ), _texture );
		glVertex2i( (GLint)viewport.y, (GLint)viewport.z );
		renderTexCoordForTexture( Vec3f( t, l, 0. ), _texture );
		glVertex2i( (GLint)viewport.y, (GLint)viewport.w );
		renderTexCoordForTexture( Vec3f( b, l, 0. ), _texture );
		glVertex2i( (GLint)viewport.x, (GLint)viewport.w );
		glEnd();
		_texture->postRender();
	}


	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode( GL_MODELVIEW );
	glPopMatrix();

	glPopAttrib();

    glStencilFunc(GL_EQUAL, 1, 0x01); // Pass test if stencil value is != 1
    glStencilMask(0x00); // Don't write anything to stencil buffer
    glDepthMask(GL_TRUE); // Write to depth buffer

}

