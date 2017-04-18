#include <H3DUbitrack/UTFullscreenRectangle.h>
#include <H3D/Appearance.h>


using namespace H3D;
using namespace H3DUbitrack;


H3DNodeDatabase UTFullscreenRectangle::database
( "UTFullscreenRectangle",
  &(newInstance<UTFullscreenRectangle>),
  typeid( UTFullscreenRectangle ),
  &X3DChildNode::database );

namespace UTFullscreenRectangleInternals {
	FIELDDB_ELEMENT( UTFullscreenRectangle, appearance, INPUT_OUTPUT );
	FIELDDB_ELEMENT( UTFullscreenRectangle, horizontalFlip, INPUT_OUTPUT );
	FIELDDB_ELEMENT( UTFullscreenRectangle, verticalFlip, INPUT_OUTPUT );
	FIELDDB_ELEMENT( UTFullscreenRectangle, depthMaskEnable, INPUT_OUTPUT );
	FIELDDB_ELEMENT( UTFullscreenRectangle, depthTestEnable, INPUT_OUTPUT );
    FIELDDB_ELEMENT( UTFullscreenRectangle, texCoord, INPUT_OUTPUT );
}


UTFullscreenRectangle::UTFullscreenRectangle(Inst< SFNode    > _metadata,
                                      Inst< DisplayList > _displayList,
                                      Inst< SFAppearanceNode  > _appearance,
									  Inst< SFBool > _horizontalFlip,
									  Inst< SFBool > _verticalFlip,
									  Inst< SFBool > _depthMaskEnable,
									  Inst< SFBool > _depthTestEnable,
									  Inst< SFVec2d > _texCoord)
  : X3DChildNode( _metadata )
  , H3DDisplayListObject( _displayList )
  , appearance( _appearance )
  , horizontalFlip(_horizontalFlip)
  , verticalFlip(_verticalFlip)
  , depthMaskEnable(_depthMaskEnable)
  , depthTestEnable(_depthTestEnable)
  , texCoord(_texCoord)
{

  type_name = "UTFullscreenRectangle";
  database.initFields( this );
  displayList->setOwner( this );

  texCoord->setValue(Vec2d(1., 1.), id);

  displayList->touch();
  appearance->route( displayList );
  horizontalFlip->route( displayList );
  verticalFlip->route( displayList );
  depthMaskEnable->route( displayList );
  depthTestEnable->route( displayList );
}



void UTFullscreenRectangle::render() {
	X3DChildNode::render();
	Vec4f viewport = Vec4f(0, 1, 0, 1);
    X3DAppearanceNode *a_ = appearance->getValue();

	Appearance *a = dynamic_cast< Appearance* >(a_);
	if (!a) {
		return;
	}

	X3DTextureNode *_texture = a->texture->getValue();

    // appearance render
    glPushAttrib( a->getAffectedGLAttribs() );
    a->preRender();
    a->displayList->callList();

	// should be more selective ...
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

	float b, t, l, r;
	bool hflip = horizontalFlip->getValue( id );
	bool vflip = verticalFlip->getValue( id );

    Vec2d tc = texCoord->getValue(id);
    float tx = (float)tc.x;
    float ty = (float)tc.y;

	t = vflip ? 0 : tx;
	b = vflip ? tx : 0;
	l = hflip ? 0 : ty;
	r = hflip ? ty : 0;

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


	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode( GL_MODELVIEW );
	glPopMatrix();
	glPopAttrib();

    a->postRender();
    glPopAttrib();
}

