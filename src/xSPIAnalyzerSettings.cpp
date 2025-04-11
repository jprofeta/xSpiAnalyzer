#include "xSPIAnalyzerSettings.h"

#include <sstream>
#include <cstring>
#include <AnalyzerHelpers.h>

xSPIAnalyzerSettings::xSPIAnalyzerSettings()
:   mEnableChannel( UNDEFINED_CHANNEL ),
    mClockChannel( UNDEFINED_CHANNEL ),
    mD0Channel( UNDEFINED_CHANNEL ),
    mD1Channel( UNDEFINED_CHANNEL ),
    mD2Channel( UNDEFINED_CHANNEL ),
    mD3Channel( UNDEFINED_CHANNEL ),
    mD4Channel( UNDEFINED_CHANNEL ),
    mD5Channel( UNDEFINED_CHANNEL ),
    mD6Channel( UNDEFINED_CHANNEL ),
    mD7Channel( UNDEFINED_CHANNEL ),
    mDataStrobeChannel( UNDEFINED_CHANNEL ),
    mClockInactiveState( BIT_LOW ),
    mEnableActiveState( BIT_LOW ),
    mBusWidth( xSPIAnalyzerEnums::OneLane ),
    mDoubleRate( false )
{
    mEnableChannelInterface.SetTitleAndTooltip( "Enable", "Enable (CS, Chip Select)" );
    mEnableChannelInterface.SetChannel( mEnableChannel );
    mEnableChannelInterface.SetSelectionOfNoneIsAllowed( false );

    mClockChannelInterface.SetTitleAndTooltip( "Clock", "Clock (CLK/SCK)" );
    mClockChannelInterface.SetChannel( mClockChannel );
    mClockChannelInterface.SetSelectionOfNoneIsAllowed( false );
    
    mD0ChannelInterface.SetTitleAndTooltip( "D0", "Data Signal 0 (D0/D0/SI/MOSI)" );
    mD0ChannelInterface.SetChannel( mD0Channel );
    mD0ChannelInterface.SetSelectionOfNoneIsAllowed( false );

    mD1ChannelInterface.SetTitleAndTooltip( "D1", "Data Signal 1 (D1/D1/SO/MISO)" );
    mD1ChannelInterface.SetChannel( mD1Channel );
    mD1ChannelInterface.SetSelectionOfNoneIsAllowed( false );

    mD2ChannelInterface.SetTitleAndTooltip( "D2", "Data Signal 2 (D2/D2)" );
    mD2ChannelInterface.SetChannel( mD2Channel );
    mD2ChannelInterface.SetSelectionOfNoneIsAllowed( true );

    mD3ChannelInterface.SetTitleAndTooltip( "D3", "Data Signal 3 (D3/D3)" );
    mD3ChannelInterface.SetChannel( mD3Channel );
    mD3ChannelInterface.SetSelectionOfNoneIsAllowed( true );

    mD4ChannelInterface.SetTitleAndTooltip( "D4", "Data Signal 4 (D4/D4)" );
    mD4ChannelInterface.SetChannel( mD4Channel );
    mD4ChannelInterface.SetSelectionOfNoneIsAllowed( true );

    mD5ChannelInterface.SetTitleAndTooltip( "D5", "Data Signal 5 (D5/D5)" );
    mD5ChannelInterface.SetChannel( mD5Channel );
    mD5ChannelInterface.SetSelectionOfNoneIsAllowed( true );

    mD6ChannelInterface.SetTitleAndTooltip( "D6", "Data Signal 6 (D6/D6)" );
    mD6ChannelInterface.SetChannel( mD6Channel );
    mD6ChannelInterface.SetSelectionOfNoneIsAllowed( true );

    mD7ChannelInterface.SetTitleAndTooltip( "D7", "Data Signal 7 (D7/D7)" );
    mD7ChannelInterface.SetChannel( mD7Channel );
    mD7ChannelInterface.SetSelectionOfNoneIsAllowed( true );

    mDataStrobeChannelInterface.SetTitleAndTooltip( "DS", "Data Strobe" );
    mDataStrobeChannelInterface.SetChannel( mDataStrobeChannel );
    mDataStrobeChannelInterface.SetSelectionOfNoneIsAllowed( true );

    mClockInactiveStateInterface.SetTitleAndTooltip( "Clock Polarity", "Active polarity of the clock signal." );
    mClockInactiveStateInterface.AddNumber( BIT_LOW, "Clock is Low when inactive (CPOL = 0)", "CPOL = 0 (Clock Polarity)" );
    mClockInactiveStateInterface.AddNumber( BIT_HIGH, "Clock is High when inactive (CPOL = 1)", "CPOL = 1 (Clock Polarity)" );
    mClockInactiveStateInterface.SetNumber( mClockInactiveState );

    mEnableActiveStateInterface.SetTitleAndTooltip( "Enable Polarity", "Active polarity of the enable signal." );
    mEnableActiveStateInterface.AddNumber( BIT_LOW, "Enable line is Active Low (Standard)", "" );
    mEnableActiveStateInterface.AddNumber( BIT_HIGH, "Enable line is Active High", "" );
    mEnableActiveStateInterface.SetNumber( mEnableActiveState );

    mBusWidthInterface.SetTitleAndTooltip( "Bus Width", "Number of data lines used for transfers." );
    mBusWidthInterface.AddNumber( xSPIAnalyzerEnums::OneLane, "1 Data Line", "Standard SPI (MOSI and MISO)" );
    mBusWidthInterface.AddNumber( xSPIAnalyzerEnums::TwoLanes, "2 Data Lines", "Dual-SPI" );
    mBusWidthInterface.AddNumber( xSPIAnalyzerEnums::FourLanes, "4 Data Lines", "Quad-SPI" );
    mBusWidthInterface.AddNumber( xSPIAnalyzerEnums::EightLanes, "8 Data Lines", "Octa-SPI" );
    mBusWidthInterface.SetNumber( mBusWidth );

    mDoubleRateInterface.SetTitleAndTooltip( "Double Data Rate Clock", "Data is latched on both edges of the clock signal." );
    mDoubleRateInterface.SetCheckBoxText( "Use DDR" );
    mDoubleRateInterface.SetValue( mDoubleRate );

    AddInterface( &mEnableChannelInterface );
    AddInterface( &mClockChannelInterface );
    AddInterface( &mD0ChannelInterface );
    AddInterface( &mD1ChannelInterface );
    AddInterface( &mD2ChannelInterface );
    AddInterface( &mD3ChannelInterface );
    AddInterface( &mD4ChannelInterface );
    AddInterface( &mD5ChannelInterface );
    AddInterface( &mD6ChannelInterface );
    AddInterface( &mD7ChannelInterface );
    AddInterface( &mDataStrobeChannelInterface );
    AddInterface( &mClockInactiveStateInterface );
    AddInterface( &mEnableActiveStateInterface );
    AddInterface( &mBusWidthInterface );
    AddInterface( &mDoubleRateInterface );

    AddExportOption( 0, "Export as text/csv file" );
    AddExportExtension( 0, "text", "txt" );
    AddExportExtension( 0, "csv", "csv" );

    ClearChannels();
    AddChannel( mEnableChannel, "ENABLE", false );
    AddChannel( mClockChannel, "CLOCK", false );
    AddChannel( mD0Channel, "D0", false );
    AddChannel( mD1Channel, "D1", false );
    AddChannel( mD2Channel, "D2", false );
    AddChannel( mD3Channel, "D3", false );
    AddChannel( mD4Channel, "D4", false );
    AddChannel( mD5Channel, "D5", false );
    AddChannel( mD6Channel, "D6", false );
    AddChannel( mD7Channel, "D7", false );
    AddChannel( mDataStrobeChannel, "DS", false );
}

xSPIAnalyzerSettings::~xSPIAnalyzerSettings()
{
}

bool xSPIAnalyzerSettings::SetSettingsFromInterfaces()
{
    Channel enable = mEnableChannelInterface.GetChannel();
    Channel clock = mClockChannelInterface.GetChannel();
    Channel d0 = mD0ChannelInterface.GetChannel();
    Channel d1 = mD1ChannelInterface.GetChannel();
    Channel d2 = mD2ChannelInterface.GetChannel();
    Channel d3 = mD3ChannelInterface.GetChannel();
    Channel d4 = mD4ChannelInterface.GetChannel();
    Channel d5 = mD5ChannelInterface.GetChannel();
    Channel d6 = mD6ChannelInterface.GetChannel();
    Channel d7 = mD7ChannelInterface.GetChannel();
    Channel dataStrobe = mDataStrobeChannelInterface.GetChannel();

    xSPIAnalyzerEnums::BusWidth busWidth = ( xSPIAnalyzerEnums::BusWidth )U32( mBusWidthInterface.GetNumber() );
    bool doubleRate = mDoubleRateInterface.GetValue();

    std::vector<Channel> channels;
    channels.push_back( d0 );
    channels.push_back( d1 );
    channels.push_back( d2 );
    channels.push_back( d3 );
    channels.push_back( d4 );
    channels.push_back( d5 );
    channels.push_back( d6 );
    channels.push_back( d7 );
    channels.push_back( clock );
    channels.push_back( enable );
    channels.push_back( dataStrobe );

    if ( AnalyzerHelpers::DoChannelsOverlap( &channels[ 0 ], channels.size() ) == true )
    {
        SetErrorText( "Please select different channels for each input." );
        return false;
    }

    if( ( ( busWidth == xSPIAnalyzerEnums::OneLane ) || ( busWidth == xSPIAnalyzerEnums::TwoLanes ) )
        && ( d0 == UNDEFINED_CHANNEL ) && ( d1 == UNDEFINED_CHANNEL ) )
    {
        SetErrorText( "Please select D0 and D1 when using 1 or 2 data lanes." );
        return false;
    }

    if( ( busWidth == xSPIAnalyzerEnums::FourLanes ) && ( d0 == UNDEFINED_CHANNEL ) && ( d1 == UNDEFINED_CHANNEL )
        && ( d2 == UNDEFINED_CHANNEL ) && ( d3 == UNDEFINED_CHANNEL ) )
    {
        SetErrorText( "Please select D0-D3 when using 4 data lanes." );
        return false;
    }

    if( ( busWidth == xSPIAnalyzerEnums::EightLanes ) && ( d0 == UNDEFINED_CHANNEL ) && ( d1 == UNDEFINED_CHANNEL )
        && ( d2 == UNDEFINED_CHANNEL ) && ( d3 == UNDEFINED_CHANNEL ) && ( d4 == UNDEFINED_CHANNEL ) && ( d5 == UNDEFINED_CHANNEL )
        && ( d6 == UNDEFINED_CHANNEL ) && ( d7 == UNDEFINED_CHANNEL ) )
    {
        SetErrorText( "Please select D0-D7 when using 8 data lanes." );
        return false;
    }

    if ((doubleRate) && (dataStrobe == UNDEFINED_CHANNEL))
    {
        SetErrorText( "Please select DS when using double data rate mode." );
        return false;
    }

    mEnableChannel = mEnableChannelInterface.GetChannel();
    mClockChannel = mClockChannelInterface.GetChannel();
    mD0Channel = mD0ChannelInterface.GetChannel();
    mD1Channel = mD1ChannelInterface.GetChannel();
    mD2Channel = mD2ChannelInterface.GetChannel();
    mD3Channel = mD3ChannelInterface.GetChannel();
    mD4Channel = mD4ChannelInterface.GetChannel();
    mD5Channel = mD5ChannelInterface.GetChannel();
    mD6Channel = mD6ChannelInterface.GetChannel();
    mD7Channel = mD7ChannelInterface.GetChannel();
    mDataStrobeChannel = mDataStrobeChannelInterface.GetChannel();
    mClockInactiveState = (BitState)U32(mClockInactiveStateInterface.GetNumber());
    mEnableActiveState = (BitState)U32(mEnableActiveStateInterface.GetNumber());
    mBusWidth = (xSPIAnalyzerEnums::BusWidth)U32( mBusWidthInterface.GetNumber() );
    mDoubleRate = mDoubleRateInterface.GetValue();

    ClearChannels();
    AddChannel(mEnableChannel, "ENABLE", mEnableChannel != UNDEFINED_CHANNEL);
    AddChannel(mClockChannel, "CLOCK", mClockChannel != UNDEFINED_CHANNEL);
    AddChannel(mD0Channel, "D0", mD0Channel != UNDEFINED_CHANNEL);
    AddChannel(mD1Channel, "D1", mD1Channel != UNDEFINED_CHANNEL);
    AddChannel(mD2Channel, "D2", mD2Channel != UNDEFINED_CHANNEL);
    AddChannel(mD3Channel, "D3", mD3Channel != UNDEFINED_CHANNEL);
    AddChannel(mD4Channel, "D4", mD4Channel != UNDEFINED_CHANNEL);
    AddChannel(mD5Channel, "D5", mD5Channel != UNDEFINED_CHANNEL);
    AddChannel(mD6Channel, "D6", mD6Channel != UNDEFINED_CHANNEL);
    AddChannel(mD7Channel, "D7", mD7Channel != UNDEFINED_CHANNEL);
    AddChannel(mDataStrobeChannel, "DS", mDataStrobeChannel != UNDEFINED_CHANNEL);

    return true;
}

void xSPIAnalyzerSettings::UpdateInterfacesFromSettings()
{
    mEnableChannelInterface.SetChannel( mEnableChannel );
    mClockChannelInterface.SetChannel( mClockChannel );
    mD0ChannelInterface.SetChannel( mD0Channel );
    mD1ChannelInterface.SetChannel( mD1Channel );
    mD2ChannelInterface.SetChannel( mD2Channel );
    mD3ChannelInterface.SetChannel( mD3Channel );
    mD4ChannelInterface.SetChannel( mD4Channel );
    mD5ChannelInterface.SetChannel( mD5Channel );
    mD6ChannelInterface.SetChannel( mD6Channel );
    mD7ChannelInterface.SetChannel( mD7Channel );
    mDataStrobeChannelInterface.SetChannel( mDataStrobeChannel );
    mClockInactiveStateInterface.SetNumber( mClockInactiveState );
    mEnableActiveStateInterface.SetNumber( mEnableActiveState );
    mBusWidthInterface.SetNumber( mBusWidth );
    mDoubleRateInterface.SetValue( mDoubleRate );
}

void xSPIAnalyzerSettings::LoadSettings( const char* settings )
{
    SimpleArchive text_archive;
    text_archive.SetString( settings );

    const char* name_string; // the first thing in the archive is the name of the protocol analyzer that the data belongs to.
    text_archive >> &name_string;
    if( strcmp( name_string, "xSpiAnalyzer" ) != 0 )
        AnalyzerHelpers::Assert( "xSpiAnalyzer: Provided with a settings string that doesn't belong to us;" );

    text_archive >> mEnableChannel;
    text_archive >> mClockChannel;
    text_archive >> mD0Channel;
    text_archive >> mD1Channel;
    text_archive >> mD2Channel;
    text_archive >> mD3Channel;
    text_archive >> mD4Channel;
    text_archive >> mD5Channel;
    text_archive >> mD6Channel;
    text_archive >> mD7Channel;
    text_archive >> mDataStrobeChannel;
    text_archive >> *( U32* )&mClockInactiveState;
    text_archive >> *( U32* )&mEnableActiveState;
    text_archive >> *( U32* )&mBusWidth;
    text_archive >> mDoubleRate;

    ClearChannels();
    AddChannel(mD0Channel, "D0", mD0Channel != UNDEFINED_CHANNEL);
    AddChannel(mD1Channel, "D1", mD1Channel != UNDEFINED_CHANNEL);
    AddChannel(mD2Channel, "D2", mD2Channel != UNDEFINED_CHANNEL);
    AddChannel(mD3Channel, "D3", mD3Channel != UNDEFINED_CHANNEL);
    AddChannel(mD4Channel, "D4", mD4Channel != UNDEFINED_CHANNEL);
    AddChannel(mD5Channel, "D5", mD5Channel != UNDEFINED_CHANNEL);
    AddChannel(mD6Channel, "D6", mD6Channel != UNDEFINED_CHANNEL);
    AddChannel(mD7Channel, "D7", mD7Channel != UNDEFINED_CHANNEL);
    AddChannel(mClockChannel, "CLOCK", mClockChannel != UNDEFINED_CHANNEL);
    AddChannel(mEnableChannel, "ENABLE", mEnableChannel != UNDEFINED_CHANNEL);
    AddChannel(mDataStrobeChannel, "DS", mDataStrobeChannel != UNDEFINED_CHANNEL);

    UpdateInterfacesFromSettings();
}

const char* xSPIAnalyzerSettings::SaveSettings()
{
    SimpleArchive text_archive;

    text_archive << "xSpiAnalyzer";
    text_archive << mEnableChannel;
    text_archive << mClockChannel;
    text_archive << mD0Channel;
    text_archive << mD1Channel;
    text_archive << mD2Channel;
    text_archive << mD3Channel;
    text_archive << mD4Channel;
    text_archive << mD5Channel;
    text_archive << mD6Channel;
    text_archive << mD7Channel;
    text_archive << mDataStrobeChannel;
    text_archive << mClockInactiveState;
    text_archive << mEnableActiveState;
    text_archive << mBusWidth;
    text_archive << mDoubleRate;

    return SetReturnString( text_archive.GetString() );
}
