/*
 * UbitrackInstance.cpp
 *
 *  Created on: 03/08/2010
 *      Author: mvl
 */

#include "H3DUbitrack/UbitrackInstance.h"
#include "H3D/ResourceResolver.h"

#include <utUtil/Exception.h>

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
    FIELDDB_ELEMENT( UbitrackInstance, sender, INPUT_OUTPUT );
}

UbitrackInstance::UbitrackInstance(
                                 Inst< SFNode     >  _metadata,
                                 Inst< MFString   > _url,
                                 Inst< SFString   > _componentDir,
                                 Inst< SFBool     >  _autoStart,
                                 Inst< MFUbitrackMeasurement >  _receiver,
                                 Inst< MFUbitrackMeasurement >  _sender ) 
: X3DChildNode( _metadata )
, X3DUrlObject( _url )
, componentDir(_componentDir)
, autoStart(_autoStart)
, receiver(_receiver)
, sender(_sender)
, running(false)
, is_loaded(false)
, facade(NULL)
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

            facade->clearDataflow();
        }

        delete facade;
        facade = NULL;
    }
}

void UbitrackInstance::initialize()
{
    try {
        facade = new SimpleFacade(componentDir->getValue( id ).c_str() );
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
                    is_loaded = facade->loadDataflow(resolved_url.c_str());
                } catch (const Ubitrack::Util::Exception& e ) {
                    // log error here
                    Console(4) << "Ubitrack Error loading DFG: " << e.what() << std::endl;
                    is_loaded = false;
                } catch (const std::exception& e) {
                    Console(4) << "Generic Exception loading DFG: " << e.what() << std::endl;
                    is_loaded = false;
                }
                
                if ((is_loaded) && (autoStart->getValue( id )))
                    Console(4) << "Autostart DFG " <<  std::endl;
                    running->setValue( true, id );
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
        for ( MFUbitrackMeasurement::const_iterator i = receiver->begin(); i != receiver->end(); ++i )
        {
            UbitrackMeasurement *um = static_cast < UbitrackMeasurement* > (*i);
            um->connect(this);
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
        for ( MFUbitrackMeasurement::const_iterator i = sender->begin(); i != sender->end(); ++i )
        {
            UbitrackMeasurement *um = static_cast < UbitrackMeasurement* > (*i);
            um->connect(this);
        }
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
        for ( MFUbitrackMeasurement::const_iterator i = sender->begin(); i != sender->end(); ++i )
        {
            UbitrackMeasurement *um = static_cast < UbitrackMeasurement* > (*i);
            um->disconnect(this);
        }

        // stop df
        try {
            facade->stopDataflow();
            stopped = true;
        } catch (const Ubitrack::Util::Exception& e ) {
            // log error here
        }

        // disconnect receivers
        for ( MFUbitrackMeasurement::const_iterator i = receiver->begin(); i != receiver->end(); ++i )
        {
            UbitrackMeasurement *um = static_cast < UbitrackMeasurement* > (*i);
            um->disconnect(this);
        }
    }
    
    return stopped;
}



void UbitrackInstance::traverseSG ( TraverseInfo& ti )
{
    if (!is_loaded)
        return;
    if (facade != NULL) {
        unsigned long long ts = facade->now();
        // first execute senders (send data to ubitrack)
        for ( MFUbitrackMeasurement::const_iterator i = sender->begin(); i != sender->end(); ++i )
        {
            UbitrackMeasurement *um = static_cast < UbitrackMeasurement* > (*i);
            um->update(ts);
        }
        // second execute receivers (get data from ubitrack)
        for ( MFUbitrackMeasurement::const_iterator i = receiver->begin(); i != receiver->end(); ++i )
        {
            UbitrackMeasurement *um = static_cast < UbitrackMeasurement* > (*i);
            um->update(ts);
        }
    }
}