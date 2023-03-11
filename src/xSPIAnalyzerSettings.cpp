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
    mShiftOrder( AnalyzerEnums::MsbFirst ),
    mBitsPerTransfer( 8 ),
    mClockInactiveState( BIT_LOW ),
    mDataValidEdge( AnalyzerEnums::LeadingEdge ),
    mEnableActiveState( BIT_LOW ),
    mProtocolMode( xSPIAnalyzerEnums::Mode_1S_1S_1S )
{
    mEnableChannelInterface.reset( new AnalyzerSettingInterfaceChannel() );
    mEnableChannelInterface->SetTitleAndTooltip( "Enable", "Enable (CS, Chip Select)" );
    mEnableChannelInterface->SetChannel( mEnableChannel );
    mEnableChannelInterface->SetSelectionOfNoneIsAllowed( true );

    mClockChannelInterface.reset( new AnalyzerSettingInterfaceChannel() );
    mClockChannelInterface->SetTitleAndTooltip( "Clock", "Clock (CLK/SCK)" );
    mClockChannelInterface->SetChannel( mClockChannel );
    mClockChannelInterface->SetSelectionOfNoneIsAllowed( false );
    
    mD0ChannelInterface.reset( new AnalyzerSettingInterfaceChannel() );
    mD0ChannelInterface->SetTitleAndTooltip( "D0", "Data Signal 0 (D0/D0/SI/MOSI)" );
    mD0ChannelInterface->SetChannel( mD0Channel );
    mD0ChannelInterface->SetSelectionOfNoneIsAllowed( true );

    mD1ChannelInterface.reset( new AnalyzerSettingInterfaceChannel() );
    mD1ChannelInterface->SetTitleAndTooltip( "D1", "Data Signal 1 (D1/D1/SO/MISO)" );
    mD1ChannelInterface->SetChannel( mD1Channel );
    mD1ChannelInterface->SetSelectionOfNoneIsAllowed( true );

    mD2ChannelInterface.reset( new AnalyzerSettingInterfaceChannel() );
    mD2ChannelInterface->SetTitleAndTooltip( "D2", "Data Signal 2 (D2/D2)" );
    mD2ChannelInterface->SetChannel( mD2Channel );
    mD2ChannelInterface->SetSelectionOfNoneIsAllowed( true );

    mD3ChannelInterface.reset( new AnalyzerSettingInterfaceChannel() );
    mD3ChannelInterface->SetTitleAndTooltip( "D3", "Data Signal 3 (D3/D3)" );
    mD3ChannelInterface->SetChannel( mD3Channel );
    mD3ChannelInterface->SetSelectionOfNoneIsAllowed( true );

    mD4ChannelInterface.reset( new AnalyzerSettingInterfaceChannel() );
    mD4ChannelInterface->SetTitleAndTooltip( "D4", "Data Signal 4 (D4/D4)" );
    mD4ChannelInterface->SetChannel( mD4Channel );
    mD4ChannelInterface->SetSelectionOfNoneIsAllowed( true );

    mD5ChannelInterface.reset( new AnalyzerSettingInterfaceChannel() );
    mD5ChannelInterface->SetTitleAndTooltip( "D5", "Data Signal 5 (D5/D5)" );
    mD5ChannelInterface->SetChannel( mD5Channel );
    mD5ChannelInterface->SetSelectionOfNoneIsAllowed( true );

    mD6ChannelInterface.reset( new AnalyzerSettingInterfaceChannel() );
    mD6ChannelInterface->SetTitleAndTooltip( "D6", "Data Signal 6 (D6/D6)" );
    mD6ChannelInterface->SetChannel( mD6Channel );
    mD6ChannelInterface->SetSelectionOfNoneIsAllowed( true );

    mD7ChannelInterface.reset( new AnalyzerSettingInterfaceChannel() );
    mD7ChannelInterface->SetTitleAndTooltip( "D7", "Data Signal 7 (D7/D7)" );
    mD7ChannelInterface->SetChannel( mD7Channel );
    mD7ChannelInterface->SetSelectionOfNoneIsAllowed( true );

    mDataStrobeChannelInterface.reset( new AnalyzerSettingInterfaceChannel() );
    mDataStrobeChannelInterface->SetTitleAndTooltip( "DS", "Data Strobe" );
    mDataStrobeChannelInterface->SetChannel( mDataStrobeChannel );
    mDataStrobeChannelInterface->SetSelectionOfNoneIsAllowed( true );

    mShiftOrderInterface.reset( new AnalyzerSettingInterfaceNumberList() );
    mShiftOrderInterface->SetTitleAndTooltip( "Significant Bit", "" );
    mShiftOrderInterface->AddNumber( AnalyzerEnums::MsbFirst, "Most Significant Bit First (Standard)",
                                     "Select if the most significant bit or least significant bit is transmitted first" );
    mShiftOrderInterface->AddNumber( AnalyzerEnums::LsbFirst, "Least Significant Bit First", "" );
    mShiftOrderInterface->SetNumber( mShiftOrder );

    mBitsPerTransferInterface.reset( new AnalyzerSettingInterfaceNumberList() );
    mBitsPerTransferInterface->SetTitleAndTooltip( "Bits per Transfer", "" );
    for( U32 i = 1; i <= 64; i++ )
    {
        std::stringstream ss;

        if( i == 8 )
            ss << "8 Bits per Transfer (Standard)";
        else
            ss << i << " Bits per Transfer";

        mBitsPerTransferInterface->AddNumber( i, ss.str().c_str(), "" );
    }
    mBitsPerTransferInterface->SetNumber( mBitsPerTransfer );

    mClockInactiveStateInterface.reset( new AnalyzerSettingInterfaceNumberList() );
    mClockInactiveStateInterface->SetTitleAndTooltip( "Clock State", "" );
    mClockInactiveStateInterface->AddNumber( BIT_LOW, "Clock is Low when inactive (CPOL = 0)", "CPOL = 0 (Clock Polarity)" );
    mClockInactiveStateInterface->AddNumber( BIT_HIGH, "Clock is High when inactive (CPOL = 1)", "CPOL = 1 (Clock Polarity)" );
    mClockInactiveStateInterface->SetNumber( mClockInactiveState );

    mDataValidEdgeInterface.reset( new AnalyzerSettingInterfaceNumberList() );
    mDataValidEdgeInterface->SetTitleAndTooltip( "Clock Phase", "" );
    mDataValidEdgeInterface->AddNumber( AnalyzerEnums::LeadingEdge, "Data is Valid on Clock Leading Edge (CPHA = 0)",
                                        "CPHA = 0 (Clock Phase)" );
    mDataValidEdgeInterface->AddNumber( AnalyzerEnums::TrailingEdge, "Data is Valid on Clock Trailing Edge (CPHA = 1)",
                                        "CPHA = 1 (Clock Phase)" );
    mDataValidEdgeInterface->SetNumber( mDataValidEdge );

    mEnableActiveStateInterface.reset( new AnalyzerSettingInterfaceNumberList() );
    mEnableActiveStateInterface->SetTitleAndTooltip( "Enable Line", "" );
    mEnableActiveStateInterface->AddNumber( BIT_LOW, "Enable line is Active Low (Standard)", "" );
    mEnableActiveStateInterface->AddNumber( BIT_HIGH, "Enable line is Active High", "" );
    mEnableActiveStateInterface->SetNumber( mEnableActiveState );

    mProtocolModeInterface.reset( new AnalyzerSettingInterfaceNumberList() );
    mProtocolModeInterface->SetTitleAndTooltip( "Protocol Mode", "" );
    mProtocolModeInterface->AddNumber( xSPIAnalyzerEnums::Mode_1S_1S_1S, "Cmd: 1 lane; Addr: 1 lane; Data: 1 lane",
                                        "1S-1S-1S" );
    mProtocolModeInterface->AddNumber( xSPIAnalyzerEnums::Mode_4S_4D_4D, "Cmd: 4 lanes; Addr: 4 lanes, double rate; Data: 4 lanes, double rate",
                                        "4S-4D-4D" );
    mProtocolModeInterface->AddNumber( xSPIAnalyzerEnums::Mode_8D_8D_8D, "Cmd: 8 lanes, double rate; Addr: 8 lanes, double rate; Data: 8 lanes, double rate",
                                        "8D-8D-8D" );
    mProtocolModeInterface->SetNumber( mProtocolMode );


    AddInterface( mD0ChannelInterface.get() );
    AddInterface( mD1ChannelInterface.get() );
    AddInterface( mD2ChannelInterface.get() );
    AddInterface( mD3ChannelInterface.get() );
    AddInterface( mD4ChannelInterface.get() );
    AddInterface( mD5ChannelInterface.get() );
    AddInterface( mD6ChannelInterface.get() );
    AddInterface( mD7ChannelInterface.get() );
    AddInterface( mClockChannelInterface.get() );
    AddInterface( mEnableChannelInterface.get() );
    AddInterface( mDataStrobeChannelInterface.get() );
    AddInterface( mShiftOrderInterface.get() );
    AddInterface( mBitsPerTransferInterface.get() );
    AddInterface( mClockInactiveStateInterface.get() );
    AddInterface( mDataValidEdgeInterface.get() );
    AddInterface( mEnableActiveStateInterface.get() );
    AddInterface( mProtocolModeInterface.get() );

    AddExportOption( 0, "Export as text/csv file" );
    AddExportExtension( 0, "text", "txt" );
    AddExportExtension( 0, "csv", "csv" );

    ClearChannels();
    AddChannel( mD0Channel, "D0", false );
    AddChannel( mD1Channel, "D1", false );
    AddChannel( mD2Channel, "D2", false );
    AddChannel( mD3Channel, "D3", false );
    AddChannel( mD4Channel, "D4", false );
    AddChannel( mD5Channel, "D5", false );
    AddChannel( mD6Channel, "D6", false );
    AddChannel( mD7Channel, "D7", false );
    AddChannel( mClockChannel, "CLOCK", false );
    AddChannel( mEnableChannel, "ENABLE", false );
    AddChannel( mDataStrobeChannel, "DS", false );
}

xSPIAnalyzerSettings::~xSPIAnalyzerSettings()
{
}

bool xSPIAnalyzerSettings::SetSettingsFromInterfaces()
{
    Channel enable = mEnableChannelInterface->GetChannel();
    Channel clock = mClockChannelInterface->GetChannel();
    Channel d0 = mD0ChannelInterface->GetChannel();
    Channel d1 = mD1ChannelInterface->GetChannel();
    Channel d2 = mD2ChannelInterface->GetChannel();
    Channel d3 = mD3ChannelInterface->GetChannel();
    Channel d4 = mD4ChannelInterface->GetChannel();
    Channel d5 = mD5ChannelInterface->GetChannel();
    Channel d6 = mD6ChannelInterface->GetChannel();
    Channel d7 = mD7ChannelInterface->GetChannel();
    Channel dataStrobe = mDataStrobeChannelInterface->GetChannel();

    xSPIAnalyzerEnums::ProtocolMode protocolMode = (xSPIAnalyzerEnums::ProtocolMode)U32( mProtocolModeInterface->GetNumber() );

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

    if ( (protocolMode == xSPIAnalyzerEnums::Mode_1S_1S_1S)
        && ( d0 == UNDEFINED_CHANNEL ) && ( d1 == UNDEFINED_CHANNEL ) )
    {
        SetErrorText( "Please select at least one input for either D0 or D1." );
        return false;
    }

    if ( (dataStrobe == UNDEFINED_CHANNEL)
        && ( (protocolMode == xSPIAnalyzerEnums::Mode_4S_4D_4D) 
            || (protocolMode == xSPIAnalyzerEnums::Mode_8D_8D_8D) ) )
    {
        SetErrorText( "Please select a data strobe channel when using a double-rate protocol mode." );
        return false;
    }

    if ( (protocolMode == xSPIAnalyzerEnums::Mode_4S_4D_4D)
        && ( d0 == UNDEFINED_CHANNEL || d1 == UNDEFINED_CHANNEL || d2 == UNDEFINED_CHANNEL || d3 == UNDEFINED_CHANNEL ) )
    {
        SetErrorText( "Please select D0-D3 when using a 4-lane protocol mode.");
        return false;
    }

    if ( (protocolMode == xSPIAnalyzerEnums::Mode_8D_8D_8D)
        && ( d0 == UNDEFINED_CHANNEL || d1 == UNDEFINED_CHANNEL || d2 == UNDEFINED_CHANNEL || d3 == UNDEFINED_CHANNEL
            || d4 == UNDEFINED_CHANNEL || d5 == UNDEFINED_CHANNEL || d6 == UNDEFINED_CHANNEL || d7 == UNDEFINED_CHANNEL ) )
    {
        SetErrorText( "Please select D0-D7 when using an 8-lane protocol mode.");
        return false;
    }

    mEnableChannel = mEnableChannelInterface->GetChannel();
    mClockChannel = mClockChannelInterface->GetChannel();
    mD0Channel = mD0ChannelInterface->GetChannel();
    mD1Channel = mD1ChannelInterface->GetChannel();
    mD2Channel = mD2ChannelInterface->GetChannel();
    mD3Channel = mD3ChannelInterface->GetChannel();
    mD4Channel = mD4ChannelInterface->GetChannel();
    mD5Channel = mD5ChannelInterface->GetChannel();
    mD6Channel = mD6ChannelInterface->GetChannel();
    mD7Channel = mD7ChannelInterface->GetChannel();
    mDataStrobeChannel = mDataStrobeChannelInterface->GetChannel();
    mShiftOrder = (AnalyzerEnums::ShiftOrder)U32(mShiftOrderInterface->GetNumber());
    mBitsPerTransfer = U32(mBitsPerTransferInterface->GetNumber());
    mClockInactiveState = (BitState)U32(mClockInactiveStateInterface->GetNumber());
    mDataValidEdge = (AnalyzerEnums::Edge)U32(mDataValidEdgeInterface->GetNumber());
    mEnableActiveState = (BitState)U32(mEnableActiveStateInterface->GetNumber());
    mProtocolMode = (xSPIAnalyzerEnums::ProtocolMode)U32( mProtocolModeInterface->GetNumber() );

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

    return true;
}

void xSPIAnalyzerSettings::UpdateInterfacesFromSettings()
{
    mEnableChannelInterface->SetChannel( mEnableChannel );
    mClockChannelInterface->SetChannel( mClockChannel );
    mD0ChannelInterface->SetChannel( mD0Channel );
    mD1ChannelInterface->SetChannel( mD1Channel );
    mD2ChannelInterface->SetChannel( mD2Channel );
    mD3ChannelInterface->SetChannel( mD3Channel );
    mD4ChannelInterface->SetChannel( mD4Channel );
    mD5ChannelInterface->SetChannel( mD5Channel );
    mD6ChannelInterface->SetChannel( mD6Channel );
    mD7ChannelInterface->SetChannel( mD7Channel );
    mDataStrobeChannelInterface->SetChannel( mDataStrobeChannel );
    mShiftOrderInterface->SetNumber( mShiftOrder );
    mBitsPerTransferInterface->SetNumber( mBitsPerTransfer );
    mClockInactiveStateInterface->SetNumber( mClockInactiveState );
    mDataValidEdgeInterface->SetNumber( mDataValidEdge );
    mEnableActiveStateInterface->SetNumber( mEnableActiveState );
    mProtocolModeInterface->SetNumber( mProtocolMode );
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
    text_archive >> *( U32* )&mShiftOrder;
    text_archive >> mBitsPerTransfer;
    text_archive >> *( U32* )&mClockInactiveState;
    text_archive >> *( U32* )&mDataValidEdge;
    text_archive >> *( U32* )&mEnableActiveState;
    text_archive >> *( U32* )&mProtocolMode;

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
    text_archive << mShiftOrder;
    text_archive << mBitsPerTransfer;
    text_archive << mClockInactiveState;
    text_archive << mDataValidEdge;
    text_archive << mEnableActiveState;
    text_archive << mProtocolMode;

    return SetReturnString( text_archive.GetString() );
}
