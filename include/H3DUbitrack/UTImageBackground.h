#ifndef _UBITRACKIMAGEBACKGROUND_HH_
#define _UBITRACKIMAGEBACKGROUND_HH_

#include <H3DUbitrack/H3DUbitrack.h>
#include <H3DUbitrack/UTCameraViewpoint.h>
#include <H3DUbitrack/UTTextureNode.h>

#include <H3D/X3DTexture2DNode.h>
#include <H3D/X3DTextureNode.h>
#include <H3D/X3DTextureCoordinateNode.h>
#include <H3D/DependentNodeFields.h>
#include <H3D/SFFloat.h>
#include <H3D/SFBool.h>

#include <H3D/X3DBackgroundNode.h>
#include <H3D/H3DWindowNode.h>
#include <H3D/SFFloat.h>

#include <utVision/TextureUpdate.h>

namespace H3DUbitrack {
	using namespace H3D;
	/**
     This is a background node that renders the ImageBuffer
     to the background thereby providing a see-through Augmented
     Reality environment.
     
     \beginX3D
     \exposedField zoom \n1 SFFloat \n2 1 \n3
      The background image camera zoom. \eol
     \endX3D
  */
  class H3DUBITRACK_API UTImageBackground
    : public H3D::X3DBackgroundNode {
  
  public:

    /// The SFTextureNode field is dependent on the displayList field
    /// of the containing X3DTextureNode node.
    typedef DependentSFNode< UTTextureNode,
                             FieldRef< H3DDisplayListObject,
                                       H3DDisplayListObject::DisplayList,
                                       &H3DDisplayListObject::displayList >, 
                             true >
    SFImageTextureNode_t;

	/// A SFNode encapsulating a CVImageBuffer Class
	struct SFImageTextureNode : SFImageTextureNode_t {
      inline ~SFImageTextureNode(){ /*clear();*/ }
      void onAdd(Node *n);
      void onRemove(Node *n);
    };




    /** Initializes local members */
    UTImageBackground( Inst< SFSetBind > _set_bind    = 0,
		       Inst< SFNode    > _metadata    = 0,
		       Inst< SFTime    > _bindTime    = 0,
		       Inst< SFBool    > _isBound     = 0,
		       Inst< DisplayList > _displayList = 0,
		       Inst< SFImageTextureNode  > _texture_left = 0,
		       Inst< SFImageTextureNode  > _texture_right = 0,
			   Inst< SFBool > _horizontalFlip = 0,
			   Inst< SFBool > _verticalFlip = 0);
    
    static H3D::H3DNodeDatabase database;
    

    inline H3D::RGBA glClearColor(){
      return H3D::RGBA(0,0,0,0); 
    }

    
    /// Renders the background with OpenGL.
    virtual void render();
    virtual void renderBackground();

    /// The texture to use
    ///
    /// <b>Access type:</b> inputOutput \n
    /// 
    /// \dotfile TextureBackground_frontTexture.dot
    auto_ptr< SFImageTextureNode >  texture_left;
    auto_ptr< SFImageTextureNode >  texture_right;

	// horizontal flip
    auto_ptr< SFBool >  horizontalFlip;

	// vertical flip
    auto_ptr< SFBool >  verticalFlip;

  protected:
    inline void renderTexCoordForTexture( const Vec3f &tc, 
                                          X3DTextureNode *t ) {
      X3DTextureCoordinateNode::renderTexCoordForTexture( tc, t );
    }

	Ubitrack::Vision::TextureUpdate m_texture_update_left;
	Ubitrack::Vision::TextureUpdate m_texture_update_right;

  };
}

#endif
