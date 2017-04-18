//
// Created by Ulrich Eck on 14.04.17.
//

#include <H3DUbitrack/UTSynchronization.h>

using namespace H3DUbitrack;
using namespace H3D;

// Add this node to the H3DNodeDatabase system.
H3DNodeDatabase UTSynchronization::database( "UTSynchronization",
        &(newInstance<UTSynchronization>),
        typeid( UTSynchronization ),
        &X3DBindableNode::database  );

namespace UTSynchronizationInternals {
//FIELDDB_ELEMENT( UTSynchronization, ioThreads, INITIALIZE_ONLY );
//FIELDDB_ELEMENT( UTSynchronization, signalsURL, INITIALIZE_ONLY );
}

UTSynchronization::UTSynchronization(
        Inst< SFSetBind >  _set_bind,
        Inst< SFNode    >  _metadata,
        Inst< SFTime    >  _bindTime,
        Inst< SFBool    >  _isBound ) :
        X3DBindableNode( "UTSynchronization", _set_bind, _metadata, _bindTime, _isBound ),
        m_currentTimestamp(0) {

    type_name = "UTSynchronization";
    database.initFields( this );
}

UTSynchronization::~UTSynchronization() {
}

void UTSynchronization::initialize ( )
{
    m_currentTimestamp = 0;
    X3DBindableNode::initialize();
}

void UTSynchronization::setTimestamp(Ubitrack::Measurement::Timestamp& ts) {
    m_currentTimestamp = ts;
}

Ubitrack::Measurement::Timestamp UTSynchronization::getTimestamp() {
    return m_currentTimestamp;
}
