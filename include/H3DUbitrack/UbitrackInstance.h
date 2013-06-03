/*
 * UbitrackInstance.h
 *
 *  Created on: 02/06/2013
 *      Author: Ulrich Eck
 */

#ifndef UBITRACKINSTANCE_H_
#define UBITRACKCONTEXT_H_

#include <H3D/X3DNode.h>
#include <H3D/SFString.h>
#include <H3D/MFNode.h>

#include <H3DUbitrack/UbitrackMeasurement.h>

namespace H3DUbitrack {
    
    using namespace H3D;
    class UbitrackMeasurement;

    class H3DUBITRACK_API UbitrackInstance : public X3DNode
    {
    public:
        typedef TypedMFNode< UbitrackMeasurement > MFUbitrackMeasurement;
        
        /// Constructor
        UbitrackInstance ( Inst< SFNode > _metadata = 0,
                         Inst< SFString > _dataflow = 0,
                         Inst< SFBool > _autoStart = 0,
                         Inst< MFUbitrackMeasurement> _ubitrackMeasurement = 0);
                
        virtual void traverseSG ( TraverseInfo& ti );
        
        /// Contains the ubitrack measurements connected to this dataflow
        auto_ptr< MFUbitrackMeasurement > ubitrackMeasurement;

        
        /// the dataflow filename
        auto_ptr< SFString > dataflow;

        /// should the dataflow be started automatically ?
        auto_ptr< SFBool > autoStart;
        
        /// Add this node to the H3DNodeDatabase system.
        static H3DNodeDatabase database;
        
    };
    
}

#endif /* UBITRACKINSTANCE_H_ */