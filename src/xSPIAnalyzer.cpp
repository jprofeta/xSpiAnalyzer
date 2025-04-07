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
	SetAnalyzerResults( &mResults );
	mResults.AddChannelBubblesWillAppearOn( mSettings.mClockChannel );
}

void xSPIAnalyzer::WorkerThread()
{
	Setup();

	AdvanceToNextPacket();

	while (true)
	{
		GetWord();
		CheckIfThreadShouldExit();
	}
}

void xSPIAnalyzer::Setup()
{
	mEnable = GetAnalyzerChannelData(mSettings.mEnableChannel);
	mClock = GetAnalyzerChannelData(mSettings.mClockChannel);

	if (mSettings.mDataStrobeChannel != UNDEFINED_CHANNEL)
		mDataStrobe = GetAnalyzerChannelData(mSettings.mDataStrobeChannel);
	else
		mDataStrobe = NULL;

	mData = {
		GetAnalyzerChannelData(mSettings.mD0Channel),
		GetAnalyzerChannelData(mSettings.mD1Channel),
		mSettings.mD2Channel != UNDEFINED_CHANNEL ? GetAnalyzerChannelData(mSettings.mD2Channel) : NULL,
		mSettings.mD3Channel != UNDEFINED_CHANNEL ? GetAnalyzerChannelData(mSettings.mD3Channel) : NULL,
		mSettings.mD4Channel != UNDEFINED_CHANNEL ? GetAnalyzerChannelData(mSettings.mD4Channel) : NULL,
		mSettings.mD5Channel != UNDEFINED_CHANNEL ? GetAnalyzerChannelData(mSettings.mD5Channel) : NULL,
		mSettings.mD6Channel != UNDEFINED_CHANNEL ? GetAnalyzerChannelData(mSettings.mD6Channel) : NULL,
		mSettings.mD7Channel != UNDEFINED_CHANNEL ? GetAnalyzerChannelData(mSettings.mD7Channel) : NULL,
	};
}

void xSPIAnalyzer::AdvanceSignalsToSample()
{
	mClock->AdvanceToAbsPosition(mCurrentSample);
	if (mDataStrobe != NULL)
		mDataStrobe->AdvanceToAbsPosition(mCurrentSample);
	for (size_t i = 0; i < 8; i++)
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
		// Move to the next active edge
		mEnable->AdvanceToNextEdge();
	}
	else
	{
		// Skip the trailing edge
		mEnable->AdvanceToNextEdge();

		// Move to the next active edge
		mEnable->AdvanceToNextEdge();
	}
	mCurrentSample = mEnable->GetSampleNumber();
	AdvanceSignalsToSample();
}

void xSPIAnalyzer::AdvanceToNextPacket()
{
	mResults.CommitPacketAndStartNewPacket();
	mResults.CommitResults();

	AdvanceToCsEdge();

	while (true)
	{
		if (VerifyClockPolarity())
		{
			FrameV2 frameV2StartTransaction;
			mResults.AddFrameV2(frameV2StartTransaction, "enable", mCurrentSample, mCurrentSample + 1);
			break;
		}
	}
}

bool xSPIAnalyzer::VerifyClockPolarity()
{
	if (mClock->GetBitState() == mSettings.mClockInactiveState)
		return true;

	// Clock error
	mResults.AddMarker(mCurrentSample, AnalyzerResults::ErrorSquare, mSettings.mClockChannel);

	Frame errorFrame;
	errorFrame.mStartingSampleInclusive = mCurrentSample;

	// Move to the next edge
	mEnable->AdvanceToNextEdge();
	mCurrentSample = mEnable->GetSampleNumber();

	errorFrame.mEndingSampleInclusive = mCurrentSample;
	errorFrame.mFlags = SPI_ERROR_FLAG | DISPLAY_AS_ERROR_FLAG;
	mResults.AddFrame(errorFrame);

	// Create V2 Frame
	FrameV2 frameV2;
	mResults.AddFrameV2(frameV2, "error", errorFrame.mStartingSampleInclusive, errorFrame.mEndingSampleInclusive + 1);

	mResults.CommitResults();
	ReportProgress(errorFrame.mEndingSampleInclusive);

	// Move to the next activating edge.
	mEnable->AdvanceToNextEdge();
	mCurrentSample = mEnable->GetSampleNumber();
	AdvanceSignalsToSample();

	return false;
}

bool xSPIAnalyzer::IsNextClockEdgeValid()
{
	// Check to see if the enable line transitions before the next clock edge.
	// If it does then the next edge isn't part of this frame or a framing error occurred.

	if (!mClock->DoMoreTransitionsExistInCurrentData() && mEnable->GetBitState() == mSettings.mEnableActiveState)
	{
		// Out of clock transitions (so far), but enable is in the active state

		if (mEnable->DoMoreTransitionsExistInCurrentData())
		{
			// Enable toggles after the current sample number.
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

	U64 nextEdge = mClock->GetSampleOfNextEdge();
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

void xSPIAnalyzer::GetWord()
{
	// Assumes CS is active but SCK is in idle

	mMarkers.clear();
	ReportProgress(mClock->GetSampleNumber());
	
	U8 dataWord = 0;	// Used in all modes
	U8 misoWord = 0;	// Used for 1S mode

	U64 firstClockEdge = 0;	// First clock of the frame

	bool needReset = false;

	for (size_t i = 0; i < 8; i++)
	{
		if (i == 0)
			CheckIfThreadShouldExit();

		if (!IsNextClockEdgeValid())
		{
			AdvanceToNextPacket();
			return;
		}

		mClock->AdvanceToNextEdge();
		if (i == 0)
			firstClockEdge = mClock->GetSampleNumber();

		switch (mSettings.mProtocolMode)
		{
			mCurrentSample = mClock->GetSampleNumber();
			AdvanceSignalsToSample();

			case xSPIAnalyzerEnums::Mode_1S_1S_1S:
			{
				// One bit per clock.
				mMarkers.push_back(TraceMarker(mCurrentSample, AnalyzerResults::MarkerType::UpArrow));

				dataWord <<= 1;
				dataWord |= mData[0]->GetBitState();

				misoWord <<= 1;
				misoWord |= mData[1]->GetBitState();

				if (!IsNextClockEdgeValid())
				{
					needReset = true;
					break;
				}

				mClock->AdvanceToNextEdge();

				break;
			}
			case xSPIAnalyzerEnums::Mode_8D_8D_8D:
			{
				// Two bits per clock (rising and falling edge)

				//
				// Rising edge of the clock.
				//
				mMarkers.push_back(TraceMarker(mCurrentSample, AnalyzerResults::MarkerType::UpArrow));
				for (size_t j = 7; j >= 0; j--)
				{
					dataWord <<= 1;
					dataWord |= mData[j]->GetBitState();
					i++;	// Increment the bit counter
				}

				// Move to the falling edge
				mClock->AdvanceToNextEdge();

				// Build result frames for the rising edge data.
				Frame risingFrame;
				risingFrame.mStartingSampleInclusive = firstClockEdge;
				risingFrame.mEndingSampleInclusive = mClock->GetSampleNumber();
				risingFrame.mData1 = dataWord;
				risingFrame.mData2 = 0;
				risingFrame.mFlags = 0;
				mResults.AddFrame(risingFrame);

				FrameV2 risingFrame2;
				risingFrame2.AddByte("data", dataWord);
				mResults.AddFrameV2(risingFrame2, "result", firstClockEdge, mClock->GetSampleNumber() + 1);

				//
				// Falling edge of the clock
				//
				mCurrentSample = mClock->GetSampleNumber();
				AdvanceSignalsToSample();

				dataWord = 0;
				mMarkers.push_back(TraceMarker(mCurrentSample, AnalyzerResults::MarkerType::DownArrow));
				for (size_t j = 7; j >= 0; j--)
				{
					dataWord <<= 1;
					dataWord |= (mData[j]->GetBitState() == BIT_HIGH) ? 1 : 0;
					i++;	// Increment the bit counter
				}

				if (!IsNextClockEdgeValid())
				{
					needReset = true;
					break;
				}

				// Move to the next edge
				mClock->AdvanceToNextEdge();

				// Build result frames for the rising edge data.
				Frame fallingFrame;
				fallingFrame.mStartingSampleInclusive = firstClockEdge;
				fallingFrame.mEndingSampleInclusive = mClock->GetSampleNumber();
				fallingFrame.mData1 = dataWord;
				fallingFrame.mData2 = 0;
				fallingFrame.mFlags = 0;
				mResults.AddFrame(fallingFrame);

				FrameV2 fallingFrame2;
				fallingFrame2.AddByte("data", dataWord);
				mResults.AddFrameV2(fallingFrame2, "result", mCurrentSample, mClock->GetSampleNumber() + 1);

				break;
			}
		}
	}

	// Save the results
	U32 count = mMarkers.size();
	for (size_t i = 0; i < count; i++)
		mResults.AddMarker(mMarkers[i].mSampleNumber, mMarkers[i].mMarkerType, mSettings.mClockChannel);

	if (mSettings.mProtocolMode == xSPIAnalyzerEnums::Mode_1S_1S_1S)
	{
		// Build result frames for standard SPI
		Frame frame;
		frame.mStartingSampleInclusive = firstClockEdge;
		frame.mEndingSampleInclusive = mClock->GetSampleNumber();
		frame.mData1 = dataWord;
		frame.mData2 = misoWord;
		frame.mFlags = 0;
		mResults.AddFrame(frame);

		FrameV2 frame2;
		frame2.AddByte("mosi", dataWord);
		frame2.AddByte("miso", misoWord);
		mResults.AddFrameV2(frame2, "result", mCurrentSample, mClock->GetSampleNumber() + 1);
	}

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
