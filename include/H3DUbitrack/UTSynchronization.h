//
// Created by Ulrich Eck on 14.04.17.
//

#ifndef H3D_UTSYNCHRONIZATION_H
#define H3D_UTSYNCHRONIZATION_H

#include "H3DUbitrack/H3DUbitrack.h"


#include <H3D/X3DBindableNode.h>
#include <H3D/MFNode.h>
#include <H3D/SFInt32.h>
#include <H3D/SFDouble.h>
#include <H3D/SFString.h>

#include <utMeasurement/Measurement.h>




namespace H3DUbitrack {

using namespace H3D;
/// A bindable node to hold the current timestamp for synchronizing all ubitrack instances
class H3DUBITRACK_API UTSynchronization : public X3DBindableNode
{
public:

    /// Constructor
    UTSynchronization ( Inst< SFSetBind > _set_bind = 0,
            Inst< SFNode > _metadata = 0,
            Inst< SFTime > _bindTime = 0,
            Inst< SFBool > _isBound = 0);

    ~UTSynchronization();

    /// Convenience function to get the top of the UTSynchronization stack.
    static inline UTSynchronization *getActive() {
        return static_cast< UTSynchronization * >( X3DBindableNode::getActive( "UTSynchronization" ) );
    }

    virtual void initialize ();

    Ubitrack::Measurement::Timestamp getTimestamp();
    void setTimestamp(Ubitrack::Measurement::Timestamp& ts);

    /// Add this node to the H3DNodeDatabase system.
    static H3DNodeDatabase database;


protected:
    Ubitrack::Measurement::Timestamp m_currentTimestamp;
//    double m_latency;
};

}


#endif //H3D_UTSYNCHRONIZATION_H
