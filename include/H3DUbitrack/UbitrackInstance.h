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
#include <H3D/SFInt32.h>
#include <H3D/SFString.h>
#include <H3D/MFString.h>
#include <H3D/MFNode.h>
#include <H3D/X3DUrlObject.h>

#include <H3DUbitrack/MeasurementReceiver.h>
#include <H3DUbitrack/MeasurementSender.h>

#include <utFacade/AdvancedFacade.h>
#include <utMeasurement/Measurement.h>


namespace H3DUbitrack {
    
using namespace H3D;

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

    class SFAddDataflow: public SFString {
    public:
        virtual void setValue( const std::string &v, int id = 0 )
        {
            if (v.empty()) {
                SFString::setValue("", id);
                return;
            }
            UbitrackInstance*ui_node = static_cast< UbitrackInstance* >( getOwner());

            bool is_running = ui_node->running->getValue();
            if (is_running) {
                ui_node->stopDataflow();
            }

            bool loaded = ui_node->loadDataflow(v, false);

            if (is_running) {
                ui_node->startDataflow();
            }

            SFString::setValue(v, id);
        }
    };
    
    typedef TypedMFNode< MeasurementReceiverBase > MFMeasurementReceiver;
    typedef TypedMFNode< MeasurementSenderBase   > MFMeasurementSender;

    
    /// Constructor
    UbitrackInstance(Inst< SFNode                > _metadata      = 0,
                     Inst< MFString              > _url           = 0,
                     Inst< SFString              > _componentDir  = 0,
                     Inst< SFString              > _log4cppConfig = 0,
                     Inst< SFBool                > _autoStart     = 0,
                     Inst< SFBool                > _dropEvents    = 0,
                     Inst< SFRunning             > _running       = 0,
                     Inst< SFInt32               > _pollEvery     = 0,
                     Inst< SFAddDataflow         > _addDataflow   = 0,
                     Inst< MFMeasurementReceiver > receiver       = 0,
                     Inst< MFMeasurementSender   > sender         = 0
                     );
    
    ~UbitrackInstance();
    
    virtual void initialize(); 
    
    virtual void traverseSG ( TraverseInfo& ti );

    virtual void render();
    
    /// Contains the ubitrack measurement receivers (Sinks) connected to the dataflow
    auto_ptr< MFMeasurementReceiver > receiver;
    
    /// Contains the ubitrack measurement senders (Sources) connected to the dataflow
    auto_ptr< MFMeasurementSender > sender;
    
    /// the component library directory for ubitrack
    auto_ptr< SFString > componentDir;
    
    /// the log file for ubitrack
    auto_ptr< SFString > log4cppConfig;
    
    /// should the dataflow be started automatically ?
    auto_ptr< SFBool > autoStart;

    /// should the eventqueue drop events ?
    auto_ptr< SFBool > dropEvents;

    // poll every nth traversal
	auto_ptr< SFInt32 > pollEvery;
    
    /// is the dataflow currently running, setting this field to true actually starts the dataflow
    auto_ptr< SFRunning > running;

    auto_ptr< SFAddDataflow > addDataflow;

    inline FacadePtr getFacadePtr() { return facade; }
    
    bool startDataflow();
    bool stopDataflow();

    inline bool isLoaded() {
        return is_loaded;
    }

    bool loadDataflow(const std::string& fname, bool replace=true);

    /// Add this node to the H3DNodeDatabase system.
    static H3DNodeDatabase database;

	static bool loggingIsInitialized;

protected:
    FacadePtr facade;
    MeasurementReceiverBase* sync_receiver;
    bool is_loaded;
        
    unsigned int traversal_counter;
    bool m_openglInitialized;
    
};
    
}

#endif /* UBITRACKINSTANCE_H_ */
