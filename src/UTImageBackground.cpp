#include <H3DUbitrack/UTImageBackground.h>

using namespace H3D;
using namespace H3DUbitrack;


H3DNodeDatabase UTImageBackground::database
( "UTImageBackground",
  &(newInstance<UTImageBackground>),
  typeid( UTImageBackground ),
  &X3DBackgroundNode::database );

namespace UTImageBackgroundInternals {
	FIELDDB_ELEMENT( UTImageBackground, texture, INPUT_OUTPUT );
}

void UTImageBackground::MFImageTextureNode::onAdd( Node *n) {
  MFImageTextureNode_t::onAdd(n);
  UTImageBackground *parent =
    static_cast<UTImageBackground*>(owner);
  UTImageTexture *t = dynamic_cast<UTImageTexture*>(n);

  t->repeatS->route(parent->displayList);

}

void UTImageBackground::MFImageTextureNode::onRemove( Node *n) {
  MFImageTextureNode_t::onRemove(n);

  if( n == NULL ){ return; }

  UTImageBackground *parent = static_cast<UTImageBackground*>(owner);
  UTImageTexture *t = dynamic_cast<UTImageTexture*>(n);

  t->repeatS->unroute(parent->displayList);
}


UTImageBackground::UTImageBackground(Inst< SFSetBind > _set_bind,
                                      Inst< SFNode    > _metadata,
                                      Inst< SFTime    > _bindTime,
                                      Inst< SFBool    > _isBound,
                                      Inst< DisplayList > _displayList,
                                      Inst< MFImageTextureNode  > _texture) :
  X3DBackgroundNode( _set_bind, _metadata, _bindTime, _isBound, _displayList),
  texture( _texture ) {

  type_name = "UTImageBackground";
  database.initFields( this );

  texture->route( displayList );
}



void UTImageBackground::render() {
	if( render_enabled ) {
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
		glDepthMask(GL_FALSE);
		glDisable(GL_DEPTH_TEST);
		//glCullFace( GL_BACK );
		//glEnable( GL_CULL_FACE );
		glDisable( GL_LIGHTING );


		glColor4f( 1, 1, 1, 1 );

		if( _texture  ) {
			_texture->preRender();
			_texture->displayList->callList();
			glBegin( GL_QUADS );
			//glNormal3f( 0, 0, 1 );
			renderTexCoordForTexture( Vec3f( 0, 1, 0 ), _texture );
			glVertex2i( (GLint)viewport.x, (GLint)viewport.z );
			renderTexCoordForTexture( Vec3f( 1, 1, 0 ), _texture );
			glVertex2i( (GLint)viewport.y, (GLint)viewport.z );
			renderTexCoordForTexture( Vec3f( 1, 0, 0 ), _texture );
			glVertex2i( (GLint)viewport.y, (GLint)viewport.w );
			renderTexCoordForTexture( Vec3f( 0, 0, 0 ), _texture );
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
}

