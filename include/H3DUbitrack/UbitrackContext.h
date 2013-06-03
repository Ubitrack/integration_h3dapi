/*
 * UbitrackContext.h
 *
 *  Created on: 03/08/2010
 *      Author: mvl
 */

#ifndef UBITRACKCONTEXT_H_
#define UBITRACKCONTEXT_H_

#include <H3D/X3DBindableNode.h>
#include <H3D/MFNode.h>

#include <H3DUbitrack/UbitrackInstance.h>

namespace H3DUbitrack {
    
    using namespace H3D;
    /// A bindable node containing all AR cameras, providing an explicit and predictable way of indexing them
    class H3DUBITRACK_API UbitrackContext : public X3DBindableNode
    {
    public:
        typedef TypedMFNode< UbitrackInstance > MFUbitrackInstance;
        
        /// Constructor
        UbitrackContext ( Inst< SFSetBind > _set_bind = 0,
                           Inst< SFNode > _metadata = 0,
                           Inst< SFTime > _bindTime = 0,
                           Inst< SFBool > _isBound = 0,
                           Inst< MFUbitrackInstance > _ubitrackInstance = 0);
        
        /// Convenience function to get the top of the UbitrackContext stack.
        static inline UbitrackContext *getActive() {
            return static_cast< UbitrackContext * >( X3DBindableNode::getActive( "UbitrackContext" ) );
        }
        
        virtual void traverseSG ( TraverseInfo& ti );
        
        /// Contains the ubitrack instances in this node
        auto_ptr< MFUbitrackInstance > ubitrackInstance;
        
        /// Add this node to the H3DNodeDatabase system.
        static H3DNodeDatabase database;
        
    };
    
}

#endif /* UBITRACKCONTEXT_H_ */
