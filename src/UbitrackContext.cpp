/*
 * UbitrackContext.cpp
 *
 *  Created on: 03/08/2010
 *      Author: mvl
 */

#include "H3DUbitrack/UbitrackContext.h"


using namespace H3DUbitrack;
using namespace H3D;

// Add this node to the H3DNodeDatabase system.
H3DNodeDatabase UbitrackContext::database( "UbitrackContext",
                                            &(newInstance<UbitrackContext>),
                                            typeid( UbitrackContext ),
                                            &X3DBindableNode::database  );

namespace UbitrackContextInternals
{
    FIELDDB_ELEMENT( UbitrackContext, ubitrackInstance, INITIALIZE_ONLY );
}

UbitrackContext::UbitrackContext(
                                     Inst<   SFSetBind >  _set_bind,
                                     Inst< SFNode    >  _metadata,
                                     Inst<  SFTime    >  _bindTime,
                                     Inst<  SFBool    >  _isBound,
                                     Inst< MFUbitrackInstance >  _ubitrackInstance ) :
X3DBindableNode( "UbitrackContext", _set_bind, _metadata, _bindTime, _isBound ),
ubitrackInstance(_ubitrackInstance) {
    
    type_name = "UbitrackContext";
    database.initFields( this );
}

void UbitrackContext::traverseSG ( TraverseInfo& ti )
{
    for ( MFUbitrackInstance::const_iterator i= ubitrackInstance->begin(); i != ubitrackInstance->end(); ++i )
    {
        // actually do something useful here ...
        (*i)->traverseSG(ti);
    }
}
