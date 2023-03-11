#ifndef XSPI_ANALYZER_H
#define XSPI_ANALYZER_H

#include <Analyzer.h>
#include "xSPIAnalyzerResults.h"
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

protected: //vars
	std::auto_ptr< xSPIAnalyzerSettings > mSettings;
	std::auto_ptr< xSPIAnalyzerResults > mResults;
	AnalyzerChannelData* mSerial;

	xSPISimulationDataGenerator mSimulationDataGenerator;
	bool mSimulationInitilized;

	//Serial analysis vars:
	U32 mSampleRateHz;
	U32 mStartOfStopBitOffset;
	U32 mEndOfStopBitOffset;
};

extern "C" ANALYZER_EXPORT const char* __cdecl GetAnalyzerName();
extern "C" ANALYZER_EXPORT Analyzer* __cdecl CreateAnalyzer( );
extern "C" ANALYZER_EXPORT void __cdecl DestroyAnalyzer( Analyzer* analyzer );

#endif //XSPI_ANALYZER_H
