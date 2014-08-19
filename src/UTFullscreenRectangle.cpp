#include <H3DUbitrack/UTFullscreenRectangle.h>

using namespace H3D;
using namespace H3DUbitrack;


H3DNodeDatabase UTFullscreenRectangle::database
( "UTFullscreenRectangle",
  &(newInstance<UTFullscreenRectangle>),
  typeid( UTFullscreenRectangle ),
  &X3DChildNode::database );

namespace UTFullscreenRectangleInternals {
	FIELDDB_ELEMENT( UTFullscreenRectangle, texture, INPUT_OUTPUT );
	FIELDDB_ELEMENT( UTFullscreenRectangle, horizontalFlip, INPUT_OUTPUT );
	FIELDDB_ELEMENT( UTFullscreenRectangle, verticalFlip, INPUT_OUTPUT );
	FIELDDB_ELEMENT( UTFullscreenRectangle, depthMaskEnable, INPUT_OUTPUT );
	FIELDDB_ELEMENT( UTFullscreenRectangle, depthTestEnable, INPUT_OUTPUT );
}

void UTFullscreenRectangle::MFImageTextureNode::onAdd( Node *n) {
  MFImageTextureNode_t::onAdd(n);
  UTFullscreenRectangle *parent =
    static_cast<UTFullscreenRectangle*>(owner);
  UTImageTexture *t = dynamic_cast<UTImageTexture*>(n);

  t->repeatS->route(parent->displayList);

}

void UTFullscreenRectangle::MFImageTextureNode::onRemove( Node *n) {
  MFImageTextureNode_t::onRemove(n);

  if( n == NULL ){ return; }

  UTFullscreenRectangle *parent = static_cast<UTFullscreenRectangle*>(owner);
  UTImageTexture *t = dynamic_cast<UTImageTexture*>(n);

  t->repeatS->unroute(parent->displayList);
}


UTFullscreenRectangle::UTFullscreenRectangle(Inst< SFNode    > _metadata,
                                      Inst< DisplayList > _displayList,
                                      Inst< MFImageTextureNode  > _texture,
									  Inst< SFBool > _horizontalFlip,
									  Inst< SFBool > _verticalFlip,
									  Inst< SFBool > _depthMaskEnable,
									  Inst< SFBool > _depthTestEnable) 
  : X3DChildNode( _metadata )
  , H3DDisplayListObject( _displayList )
  , texture( _texture )
  , horizontalFlip(_horizontalFlip)
  , verticalFlip(_verticalFlip)
  , depthMaskEnable(_depthMaskEnable)
  , depthTestEnable(_depthTestEnable)
{

  type_name = "UTFullscreenRectangle";
  database.initFields( this );
  displayList->setOwner( this );

  displayList->touch();

  texture->route( displayList );
  horizontalFlip->route( displayList );
  verticalFlip->route( displayList );
  depthMaskEnable->route( displayList );
  depthTestEnable->route( displayList );
}



void UTFullscreenRectangle::render() {
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



	glPushAttrib( GL_ALL_ATTRIB_BITS );

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	glOrtho(viewport.x, viewport.y, viewport.z, viewport.w, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();



	glDisable(GL_BLEND);

	// depth test
	if (depthTestEnable->getValue( id ) == false) {
		glDisable(GL_DEPTH_TEST);
	} else {
		glEnable(GL_DEPTH_TEST);
	}

	// depth mask
	if (depthMaskEnable->getValue( id ) == true) {
		glDepthMask(GL_TRUE);
	} else {
		glDepthMask(GL_FALSE);
	}

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
}

