/*
 * UbitrackInstance.cpp
 *
 *  Created on: 03/08/2010
 *      Author: mvl
 */

#include "H3DUbitrack/UbitrackInstance.h"
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

namespace UbitrackInstanceInternals
{
    FIELDDB_ELEMENT( UbitrackInstance, url, INITIALIZE_ONLY );
    FIELDDB_ELEMENT( UbitrackInstance, componentDir, INITIALIZE_ONLY );
    FIELDDB_ELEMENT( UbitrackInstance, autoStart, INITIALIZE_ONLY );
    FIELDDB_ELEMENT( UbitrackInstance, running, INPUT_OUTPUT );
    FIELDDB_ELEMENT( UbitrackInstance, receiver, INPUT_OUTPUT );
//    FIELDDB_ELEMENT( UbitrackInstance, sender, INPUT_OUTPUT );
}

UbitrackInstance::UbitrackInstance(
                                 Inst< SFNode     >  _metadata,
                                 Inst< MFString   > _url,
                                 Inst< SFString   > _componentDir,
                                 Inst< SFBool     >  _autoStart,
                                 Inst< SFRunning  >  _running,
                                 Inst< MFMeasurementReceiver >  _receiver//,
                                 //Inst< MFUbitrackMeasurement >  _sender
                                 )
: X3DChildNode( _metadata )
, X3DUrlObject( _url )
, componentDir(_componentDir)
, autoStart(_autoStart)
, receiver(_receiver)
//, sender(_sender)
, running(_running)
, is_loaded(false)
, facade(NULL)
, sync_receiver(NULL)
{
    
    type_name = "UbitrackInstance";
    database.initFields( this );

    autoStart->setValue(true, id);
    componentDir->setValue("lib/ubitrack", id);
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

        facade = NULL;
    }
}

void UbitrackInstance::initialize()
{
	Ubitrack::Util::initLogging();

    try {
        facade = new AdvancedFacade(componentDir->getValue( id ).c_str() );
    } catch (const Ubitrack::Util::Exception& e ) {
        // log error here
        is_loaded = false;
        return;
    }
    Console(4) << "Initialize UbitrackInstance" << std::endl;
    if (facade != NULL) {
        for( MFString::const_iterator i = url->begin(); i != url->end(); i++ ) {
            bool is_tmp_file= false;
            Console(4) << "Find DFG file: " << *i << std::endl;

            string resolved_url = resolveURLAsFile( *i, &is_tmp_file );
            Console(4) << "Resolved URL: " << resolved_url << std::endl;
            
            if (resolved_url != "") {
                setURLUsed( *i );
            
                Console(4) << "Load DFG: " << resolved_url << std::endl;
                
                try {
                	facade->loadDataflow(resolved_url.c_str());
                    is_loaded = true;
                } catch (const Ubitrack::Util::Exception& e ) {
                    // log error here
                    Console(4) << "Ubitrack Error loading DFG: " << e.what() << std::endl;
                    is_loaded = false;
                } catch (const std::exception& e) {
                    Console(4) << "Generic Exception loading DFG: " << e.what() << std::endl;
                    is_loaded = false;
                }
                
            }
            
            break;
        }
    }
}

bool UbitrackInstance::startDataflow()
{
    if (!is_loaded)
        return false;
    
    Console(4) << "Start DFG" << std::endl;
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
            Console(4) << "Error starting DFG: " << e.what() << std::endl;
        }

        // connect senders
        /*
        for ( MFUbitrackMeasurement::const_iterator i = sender->begin(); i != sender->end(); ++i )
        {
            UbitrackMeasurement *um = static_cast < UbitrackMeasurement* > (*i);
            um->connect(this->getFacadePtr());
        }
        */
    }    
    
    return started;
}

bool UbitrackInstance::stopDataflow()
{
    if (!is_loaded)
        return false;


    Console(4) << "Stop DFG" << std::endl;
    bool stopped = false;
    if (facade != NULL) {
        // disconnect senders
    	/*
        for ( MFUbitrackMeasurement::const_iterator i = sender->begin(); i != sender->end(); ++i )
        {
            UbitrackMeasurement *um = static_cast < UbitrackMeasurement* > (*i);
            um->disconnect(this->getFacadePtr());
        }
        */

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



void UbitrackInstance::traverseSG ( TraverseInfo& ti )
{
    if (!is_loaded)
        return;
    if (facade != NULL) {
    	bool is_running = running->getValue(id);

    	if ((!is_running) && (is_loaded) && (autoStart->getValue( id ))) {
            Console(4) << "Autostart DFG " <<  std::endl;
            running->setValue( true, id );
        	is_running = running->getValue(id);
        }


		unsigned long long ts = Ubitrack::Measurement::now();
		if ((sync_receiver != NULL) && (is_running)) {
			// This could potentially lock forever here here ...
			//Console(4) << "wait for data .. " << std::endl;
			ts = sync_receiver->wait_for_data_ready();
			//Console(4) << "data is ready.. " << std::endl;
		}

		// first execute senders (send data to ubitrack)
		/*
		for ( MFUbitrackMeasurement::const_iterator i = sender->begin(); i != sender->end(); ++i )
		{
			UbitrackMeasurement *um = static_cast < UbitrackMeasurement* > (*i);
			um->update(ts);
		}
		*/
		// second execute receivers (get data from ubitrack)
		for ( MFMeasurementReceiver::const_iterator i = receiver->begin(); i != receiver->end(); ++i )
		{
			MeasurementReceiverBase *um = static_cast < MeasurementReceiverBase* > (*i);
			//Console(4) << "update receiver: " << um->pattern->getValue() << std::endl;
			um->update(ts);
		}
    }
}
