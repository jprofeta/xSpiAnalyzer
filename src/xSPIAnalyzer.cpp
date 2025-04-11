
#include <AnalyzerChannelData.h>
#include <AnalyzerResults.h>

#include "xSPIAnalyzer.h"
#include "xSPIAnalyzerFrames.h"
#include "xSPIAnalyzerSettings.h"
#include "xSPIAnalyzerTypes.h"

xSPIAnalyzer::xSPIAnalyzer()
:	Analyzer2(),
	mSettings(),
	mSimulationInitilized( false ),
	mResults(xSPIAnalyzerResults(this, &mSettings))
{
    mState = State::Search;
	SetAnalyzerSettings( &mSettings );
	UseFrameV2();
}

xSPIAnalyzer::~xSPIAnalyzer()
{
	KillThread();
}

void xSPIAnalyzer::SetupResults()
{
	SetAnalyzerResults(&mResults);

	if( mSettings.mBusWidth == xSPIAnalyzerEnums::OneLane )
    {
        mResults.AddChannelBubblesWillAppearOn( mSettings.mD0Channel );
        mResults.AddChannelBubblesWillAppearOn( mSettings.mD1Channel );
    }
	else
	{
        mResults.AddChannelBubblesWillAppearOn( mSettings.mEnableChannel );
	}
}

/** @brief Entry point for capture analysis. */
void xSPIAnalyzer::WorkerThread()
{
	// Setup the analyzer for this capture
	Setup();

	// Start at the first viable transaction frame.
	AdvanceToNextFrame();

	while (true)
	{
		ReadBus();
		CheckIfThreadShouldExit();
	}
}

/**
 * @brief Sets internal variables needed during analysis.
 */
void xSPIAnalyzer::Setup()
{
    if( mResults.mDataFrames.size() > 0 )
    {
        for( const auto& f : mResults.mDataFrames )
        {
            delete f;
        }
        mResults.mDataFrames.clear();
    }

	mEnable = GetAnalyzerChannelData(mSettings.mEnableChannel);
	mClock = GetAnalyzerChannelData(mSettings.mClockChannel);

	if (mSettings.mDataStrobeChannel != UNDEFINED_CHANNEL)
		mDataStrobe = GetAnalyzerChannelData(mSettings.mDataStrobeChannel);
	else
		mDataStrobe = NULL;

	mData = {
		GetAnalyzerChannelData(mSettings.mD0Channel),	// Required
		GetAnalyzerChannelData(mSettings.mD1Channel),	// Required
		mSettings.mD2Channel != UNDEFINED_CHANNEL ? GetAnalyzerChannelData(mSettings.mD2Channel) : NULL,
		mSettings.mD3Channel != UNDEFINED_CHANNEL ? GetAnalyzerChannelData(mSettings.mD3Channel) : NULL,
		mSettings.mD4Channel != UNDEFINED_CHANNEL ? GetAnalyzerChannelData(mSettings.mD4Channel) : NULL,
		mSettings.mD5Channel != UNDEFINED_CHANNEL ? GetAnalyzerChannelData(mSettings.mD5Channel) : NULL,
		mSettings.mD6Channel != UNDEFINED_CHANNEL ? GetAnalyzerChannelData(mSettings.mD6Channel) : NULL,
		mSettings.mD7Channel != UNDEFINED_CHANNEL ? GetAnalyzerChannelData(mSettings.mD7Channel) : NULL,
	};
}

/**
 * @brief Advances all channels to the current sample number (except the CS channel).
 */
void xSPIAnalyzer::AdvanceSignalsToSample()
{
    mClock->AdvanceToAbsPosition(mCurrentSample);
	if (mDataStrobe != NULL)
        mDataStrobe->AdvanceToAbsPosition(mCurrentSample);
	auto n = mData.size();
	for (size_t i = 0; i < n; i++)
	{
		if (mData[i] == NULL)
			continue;
        mData[i]->AdvanceToAbsPosition(mCurrentSample);
	}
}

/**
 * @brief Advances all signals to the next leading edge of the enable line.
 */
void xSPIAnalyzer::AdvanceSignalsToCsLeadingEdge()
{
	if (mEnable->GetBitState() != mSettings.mEnableActiveState)
	{
		// Move to the next active edge which should be the transition to the ACTIVE state.
		mEnable->AdvanceToNextEdge();
	}
	else
	{
		// Already in the ACTIVE state. Move two edges so it is back at the ACTIVE transition edge.
		// Skip the trailing edge
		mEnable->AdvanceToNextEdge();

		// Move to the next active edge
		mEnable->AdvanceToNextEdge();
	}

	// Save the current frame start and advance the other signals.
	mFrameStart = mCurrentSample = mEnable->GetSampleNumber();
	AdvanceSignalsToSample();
}

/**
 * @brief Finalizes any pending results and moves the signals to the next valid CS transition.
 */
void xSPIAnalyzer::AdvanceToNextFrame()
{
	mResults.CommitPacketAndStartNewPacket();
	mResults.CommitResults();

	AdvanceSignalsToCsLeadingEdge();

	while (!VerifyFrameStartOrAdvance())
	{
        CheckIfThreadShouldExit();
	}

	// Create a new working frame to hold data as we go.
	mWorkingFrame = ( mSettings.mBusWidth == xSPIAnalyzerEnums::OneLane )
						? ( IFrame* ) new SPIFrame(mFrameStart)
                        : ( IFrame* ) new xSPIFrame( mSettings.mBusWidth, mFrameStart );
    mResults.mDataFrames.push_back( mWorkingFrame );

	FrameV2 frameV2StartTransaction;
    mResults.AddFrameV2(frameV2StartTransaction, "enable", mFrameStart, mFrameStart + 1);
}

/**
 * @brief Validates the start of frame has the correct clock polarity; if not the signals are moved to the next frame.
 */
bool xSPIAnalyzer::VerifyFrameStartOrAdvance()
{
	// Clock is already in the inactive state so do nothing.
	if (mClock->GetBitState() == mSettings.mClockInactiveState)
		return true;

	// Clock error, so bad frame
	mResults.AddMarker(mFrameStart, AnalyzerResults::ErrorSquare, mSettings.mEnableChannel);
    auto nextClockEdge = mClock->GetSampleOfNextEdge();
    auto nextEnableEdge = mEnable->GetSampleOfNextEdge();
	if (nextClockEdge < nextEnableEdge)
	{
        mResults.AddMarker( nextClockEdge, AnalyzerResults::ErrorSquare, mSettings.mClockChannel );
	}

	HandleFrameError(nextEnableEdge);

	// Move to the next frame activating edge.
    AdvanceSignalsToCsLeadingEdge();

	return false;
}

/**
 * @brief Removes the working frame and deallocates it.
 */
void xSPIAnalyzer::CleanupWorkingFrame()
{
    if( mWorkingFrame == nullptr )
        return;

    auto& c = mResults.mDataFrames;
    for( std::vector<IFrame*>::iterator it = c.begin(); it != c.end(); )
    {
        if( *it == mWorkingFrame )
            it = c.erase( it );
        else
            it++;
    }

    delete mWorkingFrame;
    mWorkingFrame = nullptr;
}

/**
 * @brief Performs cleanup and adds error markers for a bad frame.
 * @param frameEnd The sample number of the trailing edge of the CS signal ending this frame.
 */
void xSPIAnalyzer::HandleFrameError( U64 frameEnd )
{
    Frame errorFrame;
    errorFrame.mStartingSampleInclusive = mFrameStart;
    errorFrame.mEndingSampleInclusive = frameEnd;
    errorFrame.mFlags = SPI_ERROR_FLAG | DISPLAY_AS_ERROR_FLAG;
    mResults.AddFrame( errorFrame );

    FrameV2 frameV2;
    mResults.AddFrameV2( frameV2, "error", errorFrame.mStartingSampleInclusive, errorFrame.mEndingSampleInclusive + 1 );
    
	mResults.CommitResults();

	mEnable->AdvanceToAbsPosition( frameEnd );
    ReportProgress( errorFrame.mEndingSampleInclusive );

    CleanupWorkingFrame();
}

/**
 * @brief Checks to determine if the next clock edge is in the current frame.
 * @returns { true, nextClockEdgeSample } on a valid clock edge
 * @returns { false, frameEndSample } on an invalid clock edge
 */
std::pair<bool, U64> xSPIAnalyzer::IsNextClockEdgeInFrame()
{
	// Check to see if the enable line transitions before the next clock edge.
	// If it does then the next edge isn't part of this frame or a framing error occurred.
	if (!mClock->DoMoreTransitionsExistInCurrentData() && mEnable->GetBitState() == mSettings.mEnableActiveState)
	{
		// Out of clock transitions (so far), but enable is in the active state
		// See if the CS signal transitions.
		if (mEnable->DoMoreTransitionsExistInCurrentData())
		{
			// Enable toggles after the current sample number, but check again in case more data has come in.
			U64 frameTrailingEdge = mEnable->GetSampleOfNextEdge();    // Above if restricts the next edge to be the transition to inactive.
			if (!mClock->WouldAdvancingToAbsPositionCauseTransition(frameTrailingEdge))
			{
				// No transitions of the clock exist before the next enable edge. Report the error.
				return std::pair(false, frameTrailingEdge);	// Missing clock edge
			}
		}
	}

	// Get the next clock transition sample number
	U64 nextClockEdge = mClock->GetSampleOfNextEdge();

	// If enable transitions first, then that clock transition isn't part of this frame.
	if (mEnable->WouldAdvancingToAbsPositionCauseTransition(nextClockEdge))
	{
        U64 frameTrailingEdge = mEnable->GetSampleOfNextEdge();
        return std::pair(false, frameTrailingEdge);	// Missing clock edge
	}
	else
		return std::pair(true, nextClockEdge);
}

/**
 * Attempts to get the next octet on the bus.
 */
void xSPIAnalyzer::ReadBus()
{
	// Assumes CS is active but SCK is in idle, ie. a valid start to a frame.

	ReportProgress(mCurrentSample);

	U64 firstClockEdge = 0;	// First clock of the frame (used in 1S mode)

	auto clockEdgesPerByte = 16 >> mSettings.mBusWidth;
    if( ( mSettings.mBusWidth > BusWidth::OneLane ) && mSettings.mDoubleRate )
        clockEdgesPerByte >>= 1;

	CheckIfThreadShouldExit();

	if (!mWorkingFrame)
	{
        AdvanceToNextFrame();
        return;
	}

	for( auto i = 0; i < clockEdgesPerByte; i++ )
	{
        const auto& [ edgeValid, sample ] = IsNextClockEdgeInFrame();
		if (!edgeValid)
		{
            HandleFrameError( sample );
            AdvanceToNextFrame();
            return;
		}

		mCurrentSample = sample;
        AdvanceSignalsToSample();

		// Mark the clock edge
		mWorkingFrame->mClockMarkers.push_back(
			TraceMarker(
				mCurrentSample,
				(mClock->GetBitState() == BIT_HIGH ? AnalyzerResults::UpArrow : AnalyzerResults::DownArrow )
			)
		);

		if (mSettings.mBusWidth == BusWidth::OneLane)
		{
            auto dataFrame = ( SPIFrame* )mWorkingFrame;
            dataFrame->ShiftData( mData[ 0 ]->GetBitState(), 0 );
            dataFrame->ShiftData( mData[ 1 ]->GetBitState(), 1 );
		}
		else
		{
            auto dataFrame = ( xSPIFrame* )mWorkingFrame;
            for( auto j = ( 1 << mSettings.mBusWidth ); j > 0; j-- )
			{
                dataFrame->ShiftData( mData[ j - 1 ]->GetBitState() );
			}
		}

		if( !mSettings.mDoubleRate )
        {
            // Only clocks on the leading edge. Skip falling edges.
            const auto& [ edgeValid, sample ] = IsNextClockEdgeInFrame();
            if( !edgeValid )
            {
                HandleFrameError( sample );
                AdvanceToNextFrame();
                return;
            }

			// Skip this edge.
            mCurrentSample = sample;
            AdvanceSignalsToSample();
            i++;
        }
	}

	// If no more clocks this frame, commit the results.
	if (!IsNextClockEdgeInFrame().first && mWorkingFrame)
	{
        mWorkingFrame->mEnd = mEnable->GetSampleOfNextEdge();
        mWorkingFrame->AddToResults( mResults );
        if( mSettings.mBusWidth != BusWidth::OneLane )
        {
            auto marker = TraceMarker( mWorkingFrame->mEnd, AnalyzerResults::Stop );
            ( ( xSPIFrame* )mWorkingFrame )->AddMarker( mSettings.mEnableChannel, marker );

			marker = TraceMarker( mWorkingFrame->mStart, AnalyzerResults::Start );
            ( ( xSPIFrame* )mWorkingFrame )->AddMarker( mSettings.mEnableChannel, marker );
        }

        AdvanceToNextFrame();
	}

	ReportProgress( mCurrentSample );
	mResults.CommitResults();
}

bool xSPIAnalyzer::NeedsRerun()
{
	return false;
}

U32 xSPIAnalyzer::GenerateSimulationData( U64 minimum_sample_index, U32 device_sample_rate, SimulationChannelDescriptor** simulation_channels )
{
	if( mSimulationInitilized == false )
	{
		mSimulationDataGenerator.Initialize( GetSimulationSampleRate(), (xSPIAnalyzerSettings*) &mSettings );
		mSimulationInitilized = true;
	}

	return mSimulationDataGenerator.GenerateSimulationData( minimum_sample_index, device_sample_rate, simulation_channels );
}

U32 xSPIAnalyzer::GetMinimumSampleRateHz()
{
	return 0; //mSettings->mBitRate * 4;
}

const char* xSPIAnalyzer::GetAnalyzerName() const
{
	return "xSPI";
}

const char* GetAnalyzerName()
{
	return "xSPI";
}

Analyzer* CreateAnalyzer()
{
	return new xSPIAnalyzer();
}

void DestroyAnalyzer( Analyzer* analyzer )
{
	delete analyzer;
}
