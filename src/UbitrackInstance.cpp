/*
 * UbitrackInstance.cpp
 *
 *  Created on: 03/08/2010
 *      Author: mvl
 */

#include "H3DUbitrack/UbitrackInstance.h"
#include "H3DUbitrack/UTSynchronization.h"
#include "H3DUbitrack/UbitrackMeasurement.h"
#include "H3D/ResourceResolver.h"

#include <utUtil/Exception.h>
#include <utUtil/Logging.h>


using namespace H3DUbitrack;
using namespace H3D;
using namespace Ubitrack::Facade;

// Add this node to the H3DNodeDatabase system.
H3DNodeDatabase UbitrackInstance::database( "UbitrackInstance",
                                          &(newInstance<UbitrackInstance>),
                                          typeid( UbitrackInstance ),
                                          &X3DChildNode::database  );

bool UbitrackInstance::loggingIsInitialized = false;

namespace UbitrackInstanceInternals
{
    FIELDDB_ELEMENT( UbitrackInstance, url, INITIALIZE_ONLY );
    FIELDDB_ELEMENT( UbitrackInstance, componentDir, INITIALIZE_ONLY );
    FIELDDB_ELEMENT( UbitrackInstance, log4cppConfig, INITIALIZE_ONLY );
    FIELDDB_ELEMENT( UbitrackInstance, autoStart, INITIALIZE_ONLY );
    FIELDDB_ELEMENT( UbitrackInstance, dropEvents, INITIALIZE_ONLY );
    FIELDDB_ELEMENT( UbitrackInstance, running, INPUT_OUTPUT );
    FIELDDB_ELEMENT( UbitrackInstance, pollEvery, INPUT_OUTPUT );
    FIELDDB_ELEMENT( UbitrackInstance, addDataflow, INPUT_OUTPUT );
    FIELDDB_ELEMENT( UbitrackInstance, receiver, INPUT_OUTPUT );
    FIELDDB_ELEMENT( UbitrackInstance, sender, INPUT_OUTPUT );
}

UbitrackInstance::UbitrackInstance(
                                 Inst< SFNode     >  _metadata,
                                 Inst< MFString   > _url,
                                 Inst< SFString   > _componentDir,
                                 Inst< SFString   > _log4cppConfig,
                                 Inst< SFBool     >  _autoStart,
                                 Inst< SFBool     >  _dropEvents,
                                 Inst< SFRunning  >  _running,
                                 Inst< SFInt32  >  _pollEvery,
                                 Inst< SFAddDataflow > _addDataflow,
                                 Inst< MFMeasurementReceiver >  _receiver,
                                 Inst< MFMeasurementSender >  _sender
                                 )
: X3DChildNode( _metadata )
, X3DUrlObject( _url )
, componentDir(_componentDir)
, log4cppConfig(_log4cppConfig)
, autoStart(_autoStart)
, dropEvents(_dropEvents)
, receiver(_receiver)
, sender(_sender)
, running(_running)
, addDataflow(_addDataflow)
, pollEvery(_pollEvery)
, is_loaded(false)
, traversal_counter(0)
, m_openglInitialized(false)
//, facade(NULL)
, sync_receiver(NULL)
{
    
    type_name = "UbitrackInstance";
    database.initFields( this );

    autoStart->setValue(true, id);
    dropEvents->setValue(false, id);
	pollEvery->setValue(0, id);
    addDataflow->setValue("", id);
    componentDir->setValue("lib/ubitrack", id);
    log4cppConfig->setValue("log4cpp.conf", id);
}

UbitrackInstance::~UbitrackInstance() 
{
    if (facade != NULL) {
        if (is_loaded) {
            if (running->getValue( id ))
                running->setValue( false, id );

            if (sync_receiver != NULL) {
            	// cleanup sync_receiver related stuff  here ..
            	sync_receiver = NULL;
            }
            facade->clearDataflow();
        }

        //facade = NULL;
        facade.reset();
    }
}

void UbitrackInstance::initialize()
{
    bool is_tmp_file= false;

	if (!loggingIsInitialized) {
		std::string logging_filename = resolveURLAsFile( log4cppConfig->getValue( id ), &is_tmp_file );
		H3D::Console << "Initializing Ubitrack Logging: " << logging_filename << std::endl;
		Ubitrack::Util::initLogging(logging_filename.c_str());
		loggingIsInitialized = true;
	}

	traversal_counter = pollEvery->getValue(id);

    try {
		std::string components_path = componentDir->getValue( id );
        bool drop_ev = dropEvents->getValue( id );
		H3D::Console << "Components path: " << components_path << " drop events: " << drop_ev << std::endl;
        facade.reset(new AdvancedFacade(drop_ev, components_path.c_str() ));
    } catch (const Ubitrack::Util::Exception& e ) {
		H3D::Console << "Error creating facade instance: " << e.what() << std::endl; 
        is_loaded = false;
        return;
    }
    Console(4) << "Initialize UbitrackInstance" << std::endl;
    if (facade != NULL) {
        for( MFString::const_iterator i = url->begin(); i != url->end(); i++ ) {
            is_loaded = loadDataflow(*i);
            if (is_loaded) {
                break;
            }
        }
    }
}

bool UbitrackInstance::loadDataflow(const std::string& fname, bool replace) {
    bool is_tmp_file = false;
    string resolved_url = resolveURLAsFile( fname.c_str(), &is_tmp_file );
    H3D::Console << "Resolved URL: " << resolved_url << std::endl;

    if (resolved_url != "") {
        H3D::Console << "Load DFG: " << resolved_url << std::endl;

        try {
            facade->loadDataflow(resolved_url.c_str(), replace);
            return true;
        } catch (const Ubitrack::Util::Exception& e ) {
            // log error here
            H3D::Console << "Ubitrack Error loading DFG: " << e.what() << std::endl;
        } catch (const std::exception& e) {
            H3D::Console << "Generic Exception loading DFG: " << e.what() << std::endl;
        }
    }
    return false;
}


bool UbitrackInstance::startDataflow()
{
    if (!is_loaded)
        return false;
    
    H3D::Console << "Start DFG" << std::endl;
    bool started = false;
    if (facade != NULL) {

        // connect receivers
        for ( MFMeasurementReceiver::const_iterator i = receiver->begin(); i != receiver->end(); ++i )
        {
        	MeasurementReceiverBase *um = static_cast < MeasurementReceiverBase* > (*i);
            um->connect(this->getFacadePtr());
            if ((um->isSyncSource->getValue(id)) &&
            		(um->mode->is_push()) &&
            		(sync_receiver == NULL)) {
            	//Console(4) << "set sync receiver " << um->pattern->getValue() << std::endl;
            	sync_receiver = um;
            }
        }
        
        // start df
        try {
            facade->startDataflow();
            started = true;
        } catch (const Ubitrack::Util::Exception& e ) {
            // log error here
            H3D::Console << "Error starting DFG: " << e.what() << std::endl;
        }

        // connect senders
        for ( MFMeasurementSender::const_iterator i = sender->begin(); i != sender->end(); ++i )
        {
        	MeasurementSenderBase *um = static_cast < MeasurementSenderBase* > (*i);
            um->connect(this->getFacadePtr());
        }
    }    
    
    return started;
}

bool UbitrackInstance::stopDataflow()
{
    if (!is_loaded)
        return false;


    H3D::Console << "Stop DFG" << std::endl;
    bool stopped = false;
    if (facade != NULL) {
        // disconnect senders
        for ( MFMeasurementSender::const_iterator i = sender->begin(); i != sender->end(); ++i )
        {
        	MeasurementSenderBase *um = static_cast < MeasurementSenderBase* > (*i);
            um->disconnect(this->getFacadePtr());
        }

        // stop df

        // delete ref to sync_receiver ..
        if (sync_receiver != NULL) {
            sync_receiver = NULL;
        }

        try {
            facade->stopDataflow();
            stopped = true;
        } catch (const Ubitrack::Util::Exception& e ) {
            // log error here
			H3D::Console << "Error while stopping dataflow: " << e.what() << std::endl;
        }

        // disconnect receivers
        for ( MFMeasurementReceiver::const_iterator i = receiver->begin(); i != receiver->end(); ++i )
        {
        	MeasurementReceiverBase *um = static_cast < MeasurementReceiverBase* > (*i);
            um->disconnect(this->getFacadePtr());
        }

    }
    
    return stopped;
}

void UbitrackInstance::render() {
    if ((is_loaded) && (!m_openglInitialized)) {
        Ubitrack::Facade::initGPU();
        m_openglInitialized = true;
    }
}


void UbitrackInstance::traverseSG ( TraverseInfo& ti )
{
    if (!is_loaded)
        return;
    if (facade != NULL) {
    	bool is_running = running->getValue(id);

    	if ((!is_running) && (is_loaded) && (autoStart->getValue( id ))) {
            H3D::Console << "Autostart DFG " <<  std::endl;
            running->setValue( true, id );
        	is_running = running->getValue(id);
        }

		// only poll every n-th traversal .. e.g. 
		int pe = pollEvery->getValue(id);
		if (pe > 0) {
			if (traversal_counter == 0) {
				traversal_counter = pe;
			}
			if (traversal_counter < pe) {
				traversal_counter--;
				return;
			}
			traversal_counter--;
		}

        // timestamp synchronization happens automatic and uncoordinated.
        // the each instance that has a push source configured as SyncSource
        // will set a timestamp for all pulling instances to follow
        // all others will retrieve the timestamps - therefore,
        // the sync source must be the first instance to be traversed.
        UTSynchronization *sync = UTSynchronization::getActive();
        unsigned long long ts = 0;
        if ((sync_receiver != NULL) && (is_running)) {
            // This could potentially lock forever here here ...
            //Console(4) << "wait for data .. " << std::endl;
            ts = sync_receiver->wait_for_data_ready();
            //Console(4) << "data is ready.. " << std::endl;
            if (sync) {
                sync->setTimestamp(ts);
            }
        } else if (sync) {
            ts = sync->getTimestamp();
        } else {
            ts = Ubitrack::Measurement::now();
        }

		// first execute senders (send data to ubitrack)
		for ( MFMeasurementSender::const_iterator i = sender->begin(); i != sender->end(); ++i )
		{
			MeasurementSenderBase *um = static_cast < MeasurementSenderBase* > (*i);
			um->update(ti, ts);
		}
		// second execute receivers (get data from ubitrack)
		for ( MFMeasurementReceiver::const_iterator i = receiver->begin(); i != receiver->end(); ++i )
		{
			MeasurementReceiverBase *um = static_cast < MeasurementReceiverBase* > (*i);
			//Console(4) << "update receiver: " << um->pattern->getValue() << std::endl;
			um->update(ti, ts);
		}

    }
}
