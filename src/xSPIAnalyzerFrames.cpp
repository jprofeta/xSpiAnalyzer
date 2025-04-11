
#include <format>
#include <sstream>

#include "xSPIAnalyzerSettings.h"
#include "xSPIAnalyzerFrames.h"

#pragma region IFrame

/**
 * @brief Helper method to convert a single byte to a string representation.
 * @param byte Value to convert.
 * @param str  Output string.
 * @param maxN Maximum string length.
 * @param base Display representation.
 * @return The number of characters written to str.
 */
std::string IFrame::GetByteAsString( U8 byte, DisplayBase base )
{
    switch( base )
    {
        case DisplayBase::ASCII:
        {
            if( byte < 0x21 || byte > 0x7E ) // Non-printable range
                return std::format( "\\x{:02X}", byte );
            else
                return std::format( "{:c}", byte );
        }
        case DisplayBase::AsciiHex:
        {
            if( byte < 0x21 || byte > 0x7E )    // Non-printable range
                return std::format("\\x{0:02X} (0x{0:02X})", byte );
            else
                return std::format("{0:c} (0x{0:02X})", byte, byte );
        }
        case DisplayBase::Binary:
        {
            return std::format( "0b{:04b} {:04b}", (byte >> 4), (byte & 0x0F) );
        }
        case DisplayBase::Decimal:
        {
            return std::format( "{:d}", byte );
        }
        case DisplayBase::Hexadecimal:
        {
            return std::format( "0x{:02X}", byte );
        }
    }

    return std::string("");
}

#pragma endregion

#pragma region SPIFrame

SPIFrame::SPIFrame( U64 start )
    : IFrame( start, 0 )
{
}

SPIFrame::~SPIFrame()
{
    mMosi.clear();
    mMiso.clear();

    IFrame::~IFrame();
}

void SPIFrame::AddToResults( xSPIAnalyzerResults& results )
{
    Frame frame;
    frame.mStartingSampleInclusive = mStart;
    frame.mEndingSampleInclusive = mEnd;
    frame.mData1 = ( U64 )this;
    frame.mFlags = (U8)FrameFlag::DataIsIFramePtr;
    results.AddFrame( frame );

    FrameV2 frame2;
    frame2.AddByteArray( "mosi", mMosi.data(), mMosi.size() );
    frame2.AddByteArray( "miso", mMiso.data(), mMiso.size() );
    results.AddFrameV2( frame2, "result", mStart, mEnd );

    auto clock = results.GetSettings()->mClockChannel;
    auto n = mClockMarkers.size();
    for( auto i = 0; i < n; i++ )
    {
        results.AddMarker( mClockMarkers[ i ].mSampleNumber, mClockMarkers[ i ].mMarkerType, clock );
    }
}

void SPIFrame::GenerateBubbleText( std::ostringstream& str, DisplayBase base, int dataIndex )
{
    std::vector<U8>* data = nullptr;
    switch( dataIndex )
    {
        case 0:
        {
            data = &mMosi;
            break;
        }
        case 1:
        {
            data = &mMiso;
            break;
        }
        default:
            return;
    }

    bool first = true;
    for( const auto& d : *data )
    {
        if( !first )
            str << " ";
        else
            first = false;
        str << IFrame::GetByteAsString( d, base );
    }
}

void SPIFrame::ShiftData(BitState bit, int dataIndex)
{
    if ((dataIndex == 0) && mMosiReg.ShiftIn(bit))
    {
        mMosi.push_back( mMosiReg.GetValue() );
    }
    else if ((dataIndex == 1) && mMisoReg.ShiftIn(bit))
    {
        mMiso.push_back( mMisoReg.GetValue() );
    }
}

#pragma endregion

#pragma region xSPIFrame

xSPIFrame::xSPIFrame( BusWidth width, U64 start )
    : IFrame( start, 0 )
{
    mBusWidth = width;
}

xSPIFrame::~xSPIFrame()
{
    for( const auto& [ _, markerList ] : mExtraMarkers )
        delete markerList;
    mExtraMarkers.clear();

    IFrame::~IFrame();
}

void xSPIFrame::AddToResults( xSPIAnalyzerResults& results )
{
    Frame frame;
    frame.mStartingSampleInclusive = mStart;
    frame.mEndingSampleInclusive = mEnd;
    frame.mData1 = ( U64 )this;
    frame.mFlags = ( U8 )FrameFlag::DataIsIFramePtr;
    results.AddFrame( frame );

    FrameV2 frame2;
    frame2.AddByteArray( "data", mData.data(), mData.size() );
    results.AddFrameV2( frame2, "result", mStart, mEnd );

    auto clock = results.GetSettings()->mClockChannel;
    auto n = mClockMarkers.size();
    for( auto i = 0; i < n; i++ )
    {
        results.AddMarker( mClockMarkers[ i ].mSampleNumber, mClockMarkers[ i ].mMarkerType, clock );
    }

    for( const auto& [ ch, markerList ] : mExtraMarkers )
    {
        for( const auto& marker : *markerList )
        {
            results.AddMarker( marker.mSampleNumber, marker.mMarkerType, *ch );
        }
    }
}

void xSPIFrame::GenerateBubbleText( std::ostringstream& str, DisplayBase base, int dataIndex )
{
    ( void )dataIndex; // Not used for this frame type.

    bool first = true;
    for( const auto& d : mData )
    {
        if( !first )
            str << " ";
        else
            first = false;
        str << IFrame::GetByteAsString( d, base );
    }
}

void xSPIFrame::AddMarker( Channel& channel, TraceMarker& marker )
{
    auto chAdded = mExtraMarkers.try_emplace( &channel, new std::vector<TraceMarker>{ { marker } } );
    if( !chAdded.second )
    {
        mExtraMarkers[ &channel ]->push_back( marker );
    }
}

void xSPIFrame::ShiftData(BitState bit, int dataIndex)
{
    ( void )dataIndex; // Not used for this frame type.
    if (mByteReg.ShiftIn(bit))
    {
        mData.push_back( ( U8 )mByteReg.GetValue() );
    }
}

#pragma endregion
