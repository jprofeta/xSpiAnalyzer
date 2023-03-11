#ifndef XSPI_SIMULATION_DATA_GENERATOR
#define XSPI_SIMULATION_DATA_GENERATOR

#include <SimulationChannelDescriptor.h>
#include <string>
class xSPIAnalyzerSettings;

class xSPISimulationDataGenerator
{
public:
	xSPISimulationDataGenerator();
	~xSPISimulationDataGenerator();

	void Initialize( U32 simulation_sample_rate, xSPIAnalyzerSettings* settings );
	U32 GenerateSimulationData( U64 newest_sample_requested, U32 sample_rate, SimulationChannelDescriptor** simulation_channel );

protected:
	xSPIAnalyzerSettings* mSettings;
	U32 mSimulationSampleRateHz;

protected:
	void CreateSerialByte();
	std::string mSerialText;
	U32 mStringIndex;

	SimulationChannelDescriptor mSerialSimulationData;

};
#endif //XSPI_SIMULATION_DATA_GENERATOR