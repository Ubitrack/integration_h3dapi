#ifndef _UTSTEREOTEXTURESELECTOR_HH_
#define _UTSTEREOTEXTURESELECTOR_HH_

#include <H3DUbitrack/H3DUbitrack.h>
#include <H3DUbitrack/UTCameraViewpoint.h>
#include <H3DUbitrack/UTTextureNode.h>

#include <H3D/X3DTexture2DNode.h>
#include <H3D/X3DTextureNode.h>
#include <H3D/X3DTextureCoordinateNode.h>
#include <H3D/DependentNodeFields.h>
#include <H3D/SFFloat.h>
#include <H3D/SFBool.h>

#include <H3D/X3DChildNode.h>
#include <H3D/H3DWindowNode.h>
#include <H3D/SFInt32.h>

namespace H3DUbitrack {
	using namespace H3D;
	/**
     This node renders the StencilBuffer from one/two textures

     \beginX3D
     \exposedField zoom \n1 SFFloat \n2 1 \n3
      The stencil buffer. \eol
     \endX3D
  */
  class H3DUBITRACK_API UTStereoTextureSelector
    : public H3D::X3DChildNode {
  
  public:

    /// The SFTextureNode field is dependent on the displayList field
    /// of the containing X3DTextureNode node.
    typedef DependentMFNode< UTTextureNode,
                             FieldRef< H3DDisplayListObject,
                                       H3DDisplayListObject::DisplayList,
                                       &H3DDisplayListObject::displayList >, 
                             true >
    MFImageTextureNode_t;

	/// A SFNode encapsulating a CVImageBuffer Class
	struct MFImageTextureNode : MFImageTextureNode_t {
      inline ~MFImageTextureNode(){ clear(); }
      void onAdd(Node *n);
      void onRemove(Node *n);
    };


    /// The SFTextureNode field is dependent on the displayList field
    /// of the containing X3DTextureNode node.
    typedef DependentSFNode< X3DTextureNode, 
                             FieldRef< H3DDisplayListObject,
                                       H3DDisplayListObject::DisplayList,
                                       &H3DDisplayListObject::displayList >, 
                             true >
    SFTextureNode;



    /** Initializes local members */
    UTStereoTextureSelector(Inst< SFNode    > _metadata = 0,
		       Inst< MFImageTextureNode     > _texture = 0,
			   Inst< SFTextureNode          > _textureOut = 0,
			   Inst< SFInt32                > _textureId = 0);
    
    static H3D::H3DNodeDatabase database;
    

    /// Renders the background with OpenGL.
    virtual void render();

    /// The texture input
    ///
    /// <b>Access type:</b> inputOutput \n
    /// 
    auto_ptr< MFImageTextureNode >  texture;

    /// The texture Output
    ///
    /// <b>Access type:</b> outputOnly \n
    /// 
    auto_ptr< SFTextureNode >  textureOut;

    /// The texture ID
    ///
    /// <b>Access type:</b> outputOnly \n
    /// 
    auto_ptr< SFInt32 >  textureId;


  protected:
    inline void renderTexCoordForTexture( const Vec3f &tc, 
                                          X3DTextureNode *t ) {
      X3DTextureCoordinateNode::renderTexCoordForTexture( tc, t );
    }

  };
}

#endif
