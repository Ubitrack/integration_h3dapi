#include <H3DUbitrack/UbitrackMeasurement.h>

using namespace H3D;
using namespace H3DUbitrack;


H3DNodeDatabase UbitrackMeasurement::database
( "UbitrackMeasurement", NULL, typeid( UbitrackMeasurement ) );

namespace UbitrackMeasurementInternals {
    FIELDDB_ELEMENT( UbitrackMeasurement, measurementType, INITIALIZE_ONLY );
}


UbitrackMeasurement::UbitrackMeasurement(H3D::Inst< H3D::SFNode > _metadata,
                                         H3D::Inst< MeasurementType > _measurementType)
: X3DNode(_metadata)
, measurementMode(_measurementMode)
{

    type_name = "UbitrackMeasurement";
    // is abstract type .. n
    //database.initFields( this );

    measurementType->addValidValue( "PULL_SINK" );
    measurementType->addValidValue( "PUSH_SINK" );
    measurementType->addValidValue( "PULL_SOURCE" );
    measurementType->addValidValue( "PUSH_SOURCE" );
    measurementType->setValue("PULL_SINK");
}

UbitrackMeasurement::~UbitrackMeasurement() 
{

}

void UbitrackMeasurement::traverseSG(H3D::TraverseInfo &ti) 
{
    MeasurementType::Type mt = measurementType->getMeasurementType();
    if (mt == MeasurementType::Type::PULL_SINK) {
        // needs to call the receiveXXX method of the ApplicationPullSink
    } else if(mt == MeasurementType::Type::PUSH_SINK) {
        // receives a callback from the facade
    } else if(mt == MeasurementType::Type::PULL_SOURCE) {
        // receives a callbck from the facade
    } else if(mt == MeasurementType::Type::PUSH_SOURCE) {
        // needs to call the receiveXXX method of the ApplicaionPushSource
    } else {
    } 
    
}


void UbitrackMeasurement::connect(UbitrackInstance &instance) 
{
    MeasurementType::Type mt = measurementType->getMeasurementType();
    if (mt == MeasurementType::Type::PULL_SINK) {
        // retrieve ApplicationPullSink and store it
    } else if(mt == MeasurementType::Type::PUSH_SINK) {
        // create Custom SimpleXXXReceiver (Sink) and register as callback
    } else if(mt == MeasurementType::Type::PULL_SOURCE) {
        // create Custom SimpleXXXReceiver (Source) and register as callback
    } else if(mt == MeasurementType::Type::PUSH_SOURCE) {
        // retrieve ApplicationPushSource and store it
    } else {
    } 

}

void UbitrackMeasurement::disconnect(UbitrackInstance &instance) 
{
    MeasurementType::Type mt = measurementType->getMeasurementType();
    if (mt == MeasurementType::Type::PULL_SINK) {
        // free stored reference
    } else if(mt == MeasurementType::Type::PUSH_SINK) {
        // unregister callback
    } else if(mt == MeasurementType::Type::PULL_SOURCE) {
        // unregister callback
    } else if(mt == MeasurementType::Type::PUSH_SOURCE) {
        // free stored reference
    } else {
    } 
    
}

UbitrackMeasurement::MeasurementType::Type UbitrackMeasurement::MeasurementType::getMeasurementType() 
{
    upToDate();
    if( value == "PULL_SINK" )
        return PULL_SINK;
    else if( value == "PUSH_SINK" )
        return PUSH_SINK;
    else if( value == "PULL_SOURCE" )
        return PULL_SOURCE;
    else if( value == "PUSH_SOURCE" )
        return PUSH_SOURCE;
    else {
        stringstream s;
        s << "Must be one of "
        << "PULL_SINK, "
        << "PUSH_SINK, "
        << "PULL_SOURCE, "
        << "PUSH_SOURCE, ";
        throw InvalidMeasurementType( value, 
                                s.str(),
                                H3D_FULL_LOCATION );
    }
}