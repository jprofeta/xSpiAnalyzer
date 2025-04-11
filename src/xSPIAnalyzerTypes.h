#ifndef XSPI_ANALYZER_TYPES
#define XSPI_ANALYZER_TYPES

#include <LogicPublicTypes.h>
#include <AnalyzerResults.h>
#include <AnalyzerTypes.h>

namespace xSPIAnalyzerEnums
{
    enum BusWidth
    {
        OneLane        = 0,
        TwoLanes       = 1,
        FourLanes      = 2,
        EightLanes     = 3,
        DoubleRateFlag = 4,
    };

    enum FrameFlag
    {
        FrameError       = (1 << 0),
        DataIsIFramePtr  = (1 << 1),
        DisplayAsWarning = (1 << 6),
        DisplayAsError   = (1 << 7),
    };
};

struct TraceMarker
{
    U64 mSampleNumber;
    AnalyzerResults::MarkerType mMarkerType;

    inline TraceMarker( U64 sampleNumber, AnalyzerResults::MarkerType markerType )
    {
        mSampleNumber = sampleNumber;
        mMarkerType = markerType;
    }
};

template<int width> class ShiftRegister
{
    U64 mRegister;
    int index;

public:
    inline ShiftRegister(U64 initialValue = 0)
    {
        mRegister = initialValue;
        if (width < 64)
        {
            mRegister &= ( ( U64( 1 ) << width ) - 1 );
        }
        index = 0;
    }

    inline void Clear()
    {
        mRegister = 0;
    }

    inline U64 GetValue()
    {
        return mRegister;
    }

    inline bool ShiftIn(BitState bit)
    {
        mRegister <<= 1;
        mRegister |= bit;
        index++;
        if (index == width)
        {
            index = 0;
            return true;
        }

        return false;
    }
};

#endif //XSPI_ANALYZER_TYPES
