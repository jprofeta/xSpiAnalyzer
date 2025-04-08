#include "xSPIAnalyzer.h"
#include "xSPIAnalyzerSettings.h"
#include <AnalyzerChannelData.h>

xSPIAnalyzer::xSPIAnalyzer()
:	Analyzer2(),
	mSettings(),
	mSimulationInitilized( false ),
	mResults(xSPIAnalyzerResults(this, &mSettings))
{
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
	mResults.AddChannelBubblesWillAppearOn(mSettings.mClockChannel);
}

/** Entry point for capture analysis. */
void xSPIAnalyzer::WorkerThread()
{
	// Setup the analyzer for this capture
	Setup();

	// Start at the first viable transaction frame.
	AdvanceToNextFrame();

	while (true)
	{
		GetWord();
		CheckIfThreadShouldExit();
	}
}

/**
 * Sets internal variables needed during analysis.
 */
void xSPIAnalyzer::Setup()
{
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
 * Advances all channels to the current sample number.
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

void xSPIAnalyzer::AdvanceToCsEdge()
{
	if (mEnable->GetBitState() != mSettings.mEnableActiveState)
	{
		// Move to the next active edge which should be the transition to the ACTIVE state.
		mEnable->AdvanceToNextEdge();
	}
	else
	{
		// Already in the ACTIVE state. Move two edges to it is back at the transition point.

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
 * Finalizes any pending results and moves the signals to the next valid CS transition.
 */
void xSPIAnalyzer::AdvanceToNextFrame()
{
	mResults.CommitPacketAndStartNewPacket();
	mResults.CommitResults();

	AdvanceToCsEdge();

	while (!VerifyFrameStartClockPolarity())
	{
        CheckIfThreadShouldExit();
	}

	FrameV2 frameV2StartTransaction;
    mResults.AddFrameV2(frameV2StartTransaction, "enable", mFrameStart, mFrameStart + 1);
}

/**
 * Validates the start of frame has the correct clock polarity; if not the signals are moved to the next frame.
 */
bool xSPIAnalyzer::VerifyFrameStartClockPolarity()
{
	// Clock is already in the inactive state so do nothing.
	if (mClock->GetBitState() == mSettings.mClockInactiveState)
		return true;

	// Clock error
	mResults.AddMarker(mFrameStart, AnalyzerResults::ErrorSquare, mSettings.mClockChannel);

	Frame errorFrame;
	errorFrame.mStartingSampleInclusive = mFrameStart;

	// Move to the next edge so we can find the sample number of the end of this frame.
	mEnable->AdvanceToNextEdge();

	errorFrame.mEndingSampleInclusive = mEnable->GetSampleNumber();
	errorFrame.mFlags = SPI_ERROR_FLAG | DISPLAY_AS_ERROR_FLAG;
	mResults.AddFrame(errorFrame);

	// Create V2 Frame
	FrameV2 frameV2;
	mResults.AddFrameV2(frameV2, "error", errorFrame.mStartingSampleInclusive, errorFrame.mEndingSampleInclusive + 1);

	mResults.CommitResults();
	ReportProgress(errorFrame.mEndingSampleInclusive);

	// Move to the next activating edge.
	AdvanceToCsEdge();

	return false;
}

bool xSPIAnalyzer::IsNextClockEdgeValid()
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
			U64 nextEnableEdge = mEnable->GetSampleOfNextEdge();
			if (!mClock->WouldAdvancingToAbsPositionCauseTransition(nextEnableEdge))
			{
				// No transitions of the clock exist before the next enable edge. Report the error.
				FrameV2 frameV2;
				mResults.AddFrameV2(frameV2, "disable", nextEnableEdge, nextEnableEdge + 1);
				return false;	// Missing clock edge
			}
		}
	}

	// Get the next clock transition sample number
	U64 nextEdge = mClock->GetSampleOfNextEdge();

	// If enable transitions first, then that clock transition isn't part of this frame.
	if (mEnable->WouldAdvancingToAbsPositionCauseTransition(nextEdge))
	{
		U64 nextEnableEdge = mEnable->GetSampleOfNextEdge();
		FrameV2 frameV2;
		mResults.AddFrameV2(frameV2, "disable", nextEnableEdge, nextEnableEdge + 1);
		return false;	// Missing clock edge
	}
	else
		return true;
}

/**
 * Attempts to get the next octet on the bus.
 */
void xSPIAnalyzer::GetWord()
{
	// Assumes CS is active but SCK is in idle

	mMarkers.clear();
	ReportProgress(mCurrentSample);
	
	U8 dataWord = 0;	// Used in all modes
	U8 misoWord = 0;	// Used for 1S mode

	U64 firstClockEdge = 0;	// First clock of the frame (used in 1S mode)

	bool needReset = false;	// Indicates an error requires the frame data to be reset

	switch (mSettings.mProtocolMode)
	{
		case xSPIAnalyzerEnums::Mode_1S_1S_1S:
		{
			CheckIfThreadShouldExit();

			for (auto i = 0; i < 8; i++)
			{
				// Make sure the leading clock edge is valid.
				if (!IsNextClockEdgeValid())
				{
                    AdvanceToNextFrame();
                    return;
				}

				// Advance to the leading clock edge.
				mClock->AdvanceToNextEdge();

                if (i == 0)
                    firstClockEdge = mClock->GetSampleNumber();

				// Get the position of the leading clock edge.
				mCurrentSample = mClock->GetSampleNumber();
                AdvanceSignalsToSample();

				// Put a clock arrow on the signal trace.
				mMarkers.push_back(TraceMarker(mCurrentSample, AnalyzerResults::UpArrow));

				// Build MOSI and MISO
                dataWord <<= 1;
                dataWord |= (U8)mData[0]->GetBitState();

				misoWord <<= 1;
                misoWord |= (U8)mData[1]->GetBitState();

				// Make sure the trailing clock edge is valid.
				if (!IsNextClockEdgeValid())
                {
                    needReset = true;
                    break;
                }
				// Move to the trailing edge of the clock.
                mClock->AdvanceToNextEdge();
                mCurrentSample = mClock->GetSampleNumber();
			}
			break;
		}
        case xSPIAnalyzerEnums::Mode_8D_8D_8D:
		{
            // Make sure the next clock edge is valid.
            if(!IsNextClockEdgeValid())
            {
                AdvanceToNextFrame();
                return;
            }
            mClock->AdvanceToNextEdge();
            firstClockEdge = mCurrentSample = mClock->GetSampleNumber();
            AdvanceSignalsToSample();

			if (mClock->GetBitState() == BIT_HIGH)
                mMarkers.push_back(TraceMarker(mCurrentSample, AnalyzerResults::UpArrow));
            else
				mMarkers.push_back(TraceMarker(mCurrentSample, AnalyzerResults::DownArrow));

			// Build the data byte going from D7 -> D0 for easier math
			for (size_t j = 8; j > 0; j--)
            {
                dataWord <<= 1;
                dataWord |= (U8)mData[j - 1]->GetBitState();
            }

			if( IsNextClockEdgeValid() )
                mCurrentSample = mClock->GetSampleOfNextEdge();
            else
                mCurrentSample = mEnable->GetSampleOfNextEdge();
			break;
		}
	}

	// Add the frame
    Frame frame;
    frame.mStartingSampleInclusive = firstClockEdge;
    frame.mEndingSampleInclusive = mCurrentSample;
    frame.mData1 = dataWord;
    frame.mData2 = misoWord;
    frame.mFlags = 0;
    mResults.AddFrame(frame);

    FrameV2 frame2;
    if (mSettings.mProtocolMode == xSPIAnalyzerEnums::Mode_1S_1S_1S)
    {
        frame2.AddByte("mosi", dataWord);
        frame2.AddByte("miso", misoWord);
    }
	else
	{
        frame2.AddByte("data", dataWord);
	}

    mResults.AddFrameV2(frame2, "result", frame.mStartingSampleInclusive, frame.mEndingSampleInclusive + 1);

	// Save the results
	U32 count = mMarkers.size();
	for (size_t i = 0; i < count; i++)
		mResults.AddMarker(mMarkers[i].mSampleNumber, mMarkers[i].mMarkerType, mSettings.mClockChannel);

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
