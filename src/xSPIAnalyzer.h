#ifndef XSPI_ANALYZER_H
#define XSPI_ANALYZER_H

#include <array>
#include <vector>

#include <Analyzer.h>
#include "xSPIAnalyzerResults.h"
#include "xSPIAnalyzerSettings.h"
#include "xSPISimulationDataGenerator.h"

class xSPIAnalyzerSettings;
class ANALYZER_EXPORT xSPIAnalyzer : public Analyzer2
{
public:
	xSPIAnalyzer();
	virtual ~xSPIAnalyzer();

	virtual void SetupResults();
	virtual void WorkerThread();

	virtual U32 GenerateSimulationData( U64 newest_sample_requested, U32 sample_rate, SimulationChannelDescriptor** simulation_channels );
	virtual U32 GetMinimumSampleRateHz();

	virtual const char* GetAnalyzerName() const;
	virtual bool NeedsRerun();

	void Setup();
	void AdvanceSignalsToSample();
	void AdvanceToCsEdge();
	void AdvanceToNextFrame();
	bool VerifyFrameStartClockPolarity();
	void GetWord();
	bool IsNextClockEdgeValid();

protected: //vars
	xSPIAnalyzerSettings mSettings;
	xSPIAnalyzerResults mResults;

	AnalyzerChannelData* mEnable;
	AnalyzerChannelData* mClock;
	AnalyzerChannelData* mDataStrobe;
	std::array<AnalyzerChannelData*, 8> mData{{NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL}};

	xSPISimulationDataGenerator mSimulationDataGenerator;
	bool mSimulationInitilized;

	// analysis vars:
	U64 mFrameStart;		///< Sample number for the start of the current frame.
	U64 mCurrentSample;		///< Sample number of the current position in the frame.
	std::vector<TraceMarker> mMarkers;
};

extern "C" ANALYZER_EXPORT const char* __cdecl GetAnalyzerName();
extern "C" ANALYZER_EXPORT Analyzer* __cdecl CreateAnalyzer( );
extern "C" ANALYZER_EXPORT void __cdecl DestroyAnalyzer( Analyzer* analyzer );

#endif //XSPI_ANALYZER_H
