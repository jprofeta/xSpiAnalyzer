#ifndef XSPI_ANALYZER_FRAMES_H
#define XSPI_ANALYZER_FRAMES_H

#include <map>
#include <vector>

#include <AnalyzerHelpers.h>

#include "xSPIAnalyzerResults.h"
#include "xSPIAnalyzerTypes.h"

using namespace xSPIAnalyzerEnums;

class IFrame
{
protected:
    inline IFrame(U64 start, U64 end)
    {
        mStart = start;
        mEnd = end;
    }

public:
    inline virtual ~IFrame()
    {
        mClockMarkers.clear();
    }

    U64 mStart;
    U64 mEnd;
    std::vector<TraceMarker> mClockMarkers;

    virtual void AddToResults( xSPIAnalyzerResults& results ) = 0;
    virtual void GenerateBubbleText( std::ostringstream& str, DisplayBase base, int dataIndex = 0 ) = 0;
    virtual void ShiftData( BitState bit, int dataIndex = 0 ) = 0;

    static std::string GetByteAsString( U8 byte, DisplayBase base );
};

class SPIFrame : public IFrame
{
protected:
    ShiftRegister<8> mMosiReg;
    ShiftRegister<8> mMisoReg;

public:
    std::vector<U8> mMosi;
    std::vector<U8> mMiso;

    SPIFrame( U64 start );
    ~SPIFrame();

    virtual void AddToResults( xSPIAnalyzerResults& results );
    virtual void GenerateBubbleText( std::ostringstream& str, DisplayBase base, int dataIndex = 0 );
    virtual void ShiftData( BitState bit, int dataIndex = 0 );
};

class xSPIFrame : public IFrame
{
protected:
    ShiftRegister<8> mByteReg;

public:
    std::vector<U8> mData;
    BusWidth mBusWidth;

    xSPIFrame( BusWidth protocol, U64 start );
    ~xSPIFrame();

    void AddMarker( Channel& channel, TraceMarker& marker );
    virtual void AddToResults( xSPIAnalyzerResults& results );
    virtual void GenerateBubbleText( std::ostringstream& str, DisplayBase base, int dataIndex = 0 );
    virtual void ShiftData( BitState bit, int dataIndex = 0 );

  protected:
    std::map<Channel*, std::vector<TraceMarker>*> mExtraMarkers;
};

#endif  // XSPI_ANALYZER_FRAMES_H
