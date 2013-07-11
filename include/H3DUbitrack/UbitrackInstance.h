/*
 * UbitrackInstance.h
 *
 *  Created on: 02/06/2013
 *      Author: Ulrich Eck
 */

#ifndef UBITRACKINSTANCE_H_
#define UBITRACKINSTANCE_H_

#include <H3D/X3DChildNode.h>
#include <H3D/SFString.h>
#include <H3D/MFString.h>
#include <H3D/MFNode.h>
#include <H3D/X3DUrlObject.h>

#include <H3DUbitrack/UbitrackMeasurement.h>

#include <utFacade/SimpleFacade.h>
#include <boost/shared_ptr.hpp>


namespace H3DUbitrack {
    
using namespace H3D;

class UbitrackMeasurement;
 
typedef boost::shared_ptr<Ubitrack::Facade::SimpleFacade> SimpleFacadePtr;  
    
class H3DUBITRACK_API UbitrackInstance : public X3DChildNode, public X3DUrlObject
{
public:
    
    class SFRunning: public SFBool {
    public:
        virtual void setValue( const bool &v, int id = 0 ) {
            UbitrackInstance* ui_node = static_cast< UbitrackInstance* >( getOwner() );
            if (ui_node->isLoaded()) {
                if (v) {
                    bool r = ui_node->startDataflow();                        
                    if (r)
                        SFBool::setValue( true, id );
                    else
                        SFBool::setValue( false, id );
                    
                } else {
                    ui_node->stopDataflow();
                    SFBool::setValue( false, id );
                }
            
            } else {
                SFBool::setValue( false, id );
            }
        }
    };
    
    typedef TypedMFNode< UbitrackMeasurement > MFUbitrackMeasurement;
    
    /// Constructor
    UbitrackInstance(Inst< SFNode                > _metadata     = 0,
                     Inst< MFString              > _url          = 0,
                     Inst< SFString              > _componentDir = 0,
                     Inst< SFBool                > _autoStart    = 0,
                     Inst< SFRunning             >  _running     = 0,
                     Inst< MFUbitrackMeasurement > receiver      = 0,
                     Inst< MFUbitrackMeasurement > sender        = 0);
    
    ~UbitrackInstance();
    
    virtual void initialize(); 
    
    virtual void traverseSG ( TraverseInfo& ti );
    
    /// Contains the ubitrack measurement receivers (Sinks) connected to the dataflow
    auto_ptr< MFUbitrackMeasurement > receiver;
    
    /// Contains the ubitrack measurement senders (Sources) connected to the dataflow
    auto_ptr< MFUbitrackMeasurement > sender;
    
    /// the component library directory for ubitrack
    auto_ptr< SFString > componentDir;
    
    /// should the dataflow be started automatically ?
    auto_ptr< SFBool > autoStart;
    
    /// is the dataflow currently running, setting this field to true actually starts the dataflow
    auto_ptr< SFRunning > running;
    
    inline SimpleFacadePtr getFacadePtr() { return facade; }
    
    bool startDataflow();
    bool stopDataflow();
    inline bool isLoaded() {
        return is_loaded;
    }

    /// Add this node to the H3DNodeDatabase system.
    static H3DNodeDatabase database;
    
protected:
    SimpleFacadePtr facade;
    bool is_loaded;
        
    
    
};
    
}

#endif /* UBITRACKINSTANCE_H_ */
