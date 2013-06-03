/*
 * UbitrackInstance.cpp
 *
 *  Created on: 03/08/2010
 *      Author: mvl
 */

#include "H3DUbitrack/UbitrackInstance.h"


using namespace H3DUbitrack;
using namespace H3D;

// Add this node to the H3DNodeDatabase system.
H3DNodeDatabase UbitrackInstance::database( "UbitrackInstance",
                                          &(newInstance<UbitrackInstance>),
                                          typeid( UbitrackInstance ),
                                          &X3DBindableNode::database  );

namespace UbitrackInstanceInternals
{
    FIELDDB_ELEMENT( UbitrackInstance, dataflow, INPUT_OUTPUT );
    FIELDDB_ELEMENT( UbitrackInstance, autoStart, INPUT_OUTPUT );
    FIELDDB_ELEMENT( UbitrackInstance, ubitrackMeasurement, INPUT_OUTPUT );
}

UbitrackInstance::UbitrackInstance(
                                 Inst< SFNode     >  _metadata,
                                 Inst<  SFString  >  _dataflow,
                                 Inst<  SFBool    >  _autoStart,
                                 Inst< MFUbitrackMeasurement >  _ubitrackMeasurement ) :
X3DBindableNode( "UbitrackInstance", _set_bind, _metadata, _bindTime, _isBound ),
ubitrackMeasurement(_ubitrackMeasurement) {
    
    type_name = "UbitrackInstance";
    database.initFields( this );
}

void UbitrackInstance::traverseSG ( TraverseInfo& ti )
{
    for ( MFUbitrackMeasurement::const_iterator i= ubitrackMeasurement->begin(); i != ubitrackMeasurement->end(); ++i )
    {
        // actually do something useful here ...
        (*i)->traverseSG(ti);
    }
}