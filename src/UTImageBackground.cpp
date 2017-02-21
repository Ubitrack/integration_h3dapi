#include <H3DUbitrack/UTImageBackground.h>

using namespace H3D;
using namespace H3DUbitrack;


H3DNodeDatabase UTImageBackground::database
( "UTImageBackground",
  &(newInstance<UTImageBackground>),
  typeid( UTImageBackground ),
  &X3DBackgroundNode::database );

namespace UTImageBackgroundInternals {
	FIELDDB_ELEMENT( UTImageBackground, texture_left, INPUT_OUTPUT );
	FIELDDB_ELEMENT( UTImageBackground, texture_right, INPUT_OUTPUT );
	FIELDDB_ELEMENT( UTImageBackground, horizontalFlip, INPUT_OUTPUT );
	FIELDDB_ELEMENT( UTImageBackground, verticalFlip, INPUT_OUTPUT );
}

void UTImageBackground::SFImageTextureNode::onAdd( Node *n) {
  SFImageTextureNode_t::onAdd(n);
  UTImageBackground *parent =
    static_cast<UTImageBackground*>(owner);
  UTImageTexture *t = dynamic_cast<UTImageTexture*>(n);

  t->repeatS->route(parent->displayList);

}

void UTImageBackground::SFImageTextureNode::onRemove( Node *n) {
  SFImageTextureNode_t::onRemove(n);

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
                                      Inst< SFImageTextureNode  > _texture_left,
                                      Inst< SFImageTextureNode  > _texture_right,
									  Inst< SFBool > _horizontalFlip,
									  Inst< SFBool > _verticalFlip) 
  : X3DBackgroundNode( _set_bind, _metadata, _bindTime, _isBound, _displayList)
  , texture_left( _texture_left )
  , texture_right( _texture_right )
  , horizontalFlip(_horizontalFlip)
  , verticalFlip(_verticalFlip)
  , m_texture_update_left()
  , m_texture_update_right()
{

  type_name = "UTImageBackground";
  database.initFields( this );

  // does not make sense to cache anything in this node ..
  displayList->setCacheMode(H3DDisplayListObject::DisplayList::OFF);

  texture_left->route( displayList );
  texture_right->route( displayList );
  horizontalFlip->route( displayList );
  verticalFlip->route( displayList );
}

void UTImageBackground::renderBackground() {
	render_enabled = true;
	displayList->callList(false);
	render_enabled = false;
}

void UTImageBackground::render() {
	if( render_enabled ) {
		Vec4f viewport = Vec4f(0, 1, 0, 1);
		Viewpoint::EyeMode eye_mode = Viewpoint::MONO;
		UTImageTexture *_texture = NULL;

		UTCameraViewpoint *cvp = dynamic_cast<UTCameraViewpoint *>(UTCameraViewpoint::getActive());
		if (cvp) {
			viewport = cvp->getViewport();
			eye_mode = cvp->getCurrentEye();
		}

		if (eye_mode == Viewpoint::RIGHT_EYE) {
			_texture =  dynamic_cast<UTImageTexture *>(texture_right->getValue());
		} else {
			_texture =  dynamic_cast<UTImageTexture *>(texture_left->getValue());
		}

		if (_texture == NULL) {
			Console(4) << "no texture for UTImageBackground" << std::endl;
			return;
		}

		if (!_texture->getImageMeasurement()) {
			Console(4) << "no ImageMeasurement for UTImageBackground" << std::endl;
			return;
		}

		if ((_texture->getImageMeasurement()->width() == 0) || (_texture->getImageMeasurement()->height() == 0)) {
			Console(4) << "incorrect dimensions for UTImageBackground" << std::endl;
			return;
		}

		glPushAttrib( GL_ALL_ATTRIB_BITS );

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		gluOrtho2D(viewport.x, viewport.y, viewport.z, viewport.w);

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();


		glDisable(GL_BLEND);
		glDepthMask(GL_FALSE);
		glDisable(GL_DEPTH_TEST);
		//glCullFace( GL_BACK );
		//glEnable( GL_CULL_FACE );
		glDisable( GL_LIGHTING );

		glColor4f( 1.f, 1.f, 1.f, 1.f );

		//int b, t, l, r;
		//bool hflip = horizontalFlip->getValue( id );
		//bool vflip = verticalFlip->getValue( id );

		//t = vflip ? 0 : 1;
		//b = vflip ? 1 : 0;
		//l = hflip ? 0 : 1;
		//r = hflip ? 1 : 0;

		double width = viewport.y - viewport.x;
		double height = viewport.w - viewport.z;
		unsigned int pow2width, pow2height;

		glEnable(GL_TEXTURE_2D);

		if (eye_mode == Viewpoint::RIGHT_EYE) {
			if (!m_texture_update_right.isInitialized()) {
				m_texture_update_right.initializeTexture(_texture->getImageMeasurement());
				Console(3) << "Initialize Right Camera TextureUpdate: " << m_texture_update_right.pow2width() << ", " << m_texture_update_right.pow2height() << std::endl;
			}
			m_texture_update_right.updateTexture(_texture->getImageMeasurement());
			pow2width = m_texture_update_right.pow2width();
			pow2height = m_texture_update_right.pow2height();
			glBindTexture(GL_TEXTURE_2D, m_texture_update_right.m_texture);
		} else {
			if (!m_texture_update_left.isInitialized()) {
				m_texture_update_left.initializeTexture(_texture->getImageMeasurement());
				Console(3) << "Initialize Left Camera TextureUpdate" << m_texture_update_left.pow2width() << ", " << m_texture_update_left.pow2height() << std::endl;
			}
			m_texture_update_left.updateTexture(_texture->getImageMeasurement());
			pow2width = m_texture_update_left.pow2width();
			pow2height = m_texture_update_left.pow2height();
			glBindTexture(GL_TEXTURE_2D, m_texture_update_left.m_texture);
		}

		glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );

		if ((pow2width == 0) || (pow2height == 0)) {
			Console(5) << "Error pow2 dimensions .. viewport: " << viewport <<  " p2w: " << pow2width <<  " p2h: " << pow2height <<  " w: " << _texture->getImageMeasurement()->width() <<  " h: " << _texture->getImageMeasurement()->height() << std::endl;
		}
		
		double y0 = _texture->getImageMeasurement()->origin() ? 0 : height;
		double y1 = height - y0;
		double tx = double( _texture->getImageMeasurement()->width() ) / pow2width;
		double ty = double( _texture->getImageMeasurement()->height() ) / pow2height;

		// draw two triangles
		glBegin( GL_TRIANGLE_STRIP );
		glTexCoord2d(  0, ty ); glVertex2d( 0, y1 );
		glTexCoord2d(  0,  0 ); glVertex2d( 0, y0 );
		glTexCoord2d( tx, ty ); glVertex2d( width, y1 );
		glTexCoord2d( tx,  0 ); glVertex2d( width, y0 );
		glEnd();


		//glBegin( GL_QUADS );
		//glTexCoord3f( (float)b, (float)r, 0.f );
		//glVertex2i( (GLint)viewport.x, (GLint)viewport.z );

		//glTexCoord3f( (float)t, (float)r, 0.f );
		//glVertex2i( (GLint)viewport.y, (GLint)viewport.z );

		//glTexCoord3f( (float)t, (float)l, 0.f );
		//glVertex2i( (GLint)viewport.y, (GLint)viewport.w );

		//glTexCoord3f( (float)b, (float)l, 0.f );
		//glVertex2i( (GLint)viewport.x, (GLint)viewport.w );
		//glEnd();

		glBindTexture(GL_TEXTURE_2D, 0); 

		glDisable( GL_TEXTURE_2D );

		glMatrixMode(GL_PROJECTION);
		glPopMatrix();

		glMatrixMode( GL_MODELVIEW );
		glPopMatrix();

		glPopAttrib();

	}
}

