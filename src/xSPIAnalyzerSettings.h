#ifndef XSPI_ANALYZER_SETTINGS
#define XSPI_ANALYZER_SETTINGS

#include <AnalyzerSettings.h>
#include <AnalyzerTypes.h>

#include "xSPIAnalyzerTypes.h"

class xSPIAnalyzerSettings : public AnalyzerSettings
{
public:
    xSPIAnalyzerSettings();
    virtual ~xSPIAnalyzerSettings();

    virtual bool SetSettingsFromInterfaces();
    void UpdateInterfacesFromSettings();
    virtual void LoadSettings( const char* settings );
    virtual const char* SaveSettings();

    Channel mEnableChannel;
    Channel mClockChannel;
    Channel mD0Channel;
    Channel mD1Channel;
    Channel mD2Channel;
    Channel mD3Channel;
    Channel mD4Channel;
    Channel mD5Channel;
    Channel mD6Channel;
    Channel mD7Channel;
    Channel mDataStrobeChannel;
    AnalyzerEnums::ShiftOrder mShiftOrder;
    U32 mBitsPerTransfer;
    BitState mClockInactiveState;
    AnalyzerEnums::Edge mDataValidEdge;
    BitState mEnableActiveState;
    xSPIAnalyzerEnums::ProtocolMode mProtocolMode;

protected:
    std::auto_ptr<AnalyzerSettingInterfaceChannel>    mEnableChannelInterface;
    std::auto_ptr<AnalyzerSettingInterfaceChannel>    mClockChannelInterface;
    std::auto_ptr<AnalyzerSettingInterfaceChannel>    mD0ChannelInterface;
    std::auto_ptr<AnalyzerSettingInterfaceChannel>    mD1ChannelInterface;
    std::auto_ptr<AnalyzerSettingInterfaceChannel>    mD2ChannelInterface;
    std::auto_ptr<AnalyzerSettingInterfaceChannel>    mD3ChannelInterface;
    std::auto_ptr<AnalyzerSettingInterfaceChannel>    mD4ChannelInterface;
    std::auto_ptr<AnalyzerSettingInterfaceChannel>    mD5ChannelInterface;
    std::auto_ptr<AnalyzerSettingInterfaceChannel>    mD6ChannelInterface;
    std::auto_ptr<AnalyzerSettingInterfaceChannel>    mD7ChannelInterface;
    std::auto_ptr<AnalyzerSettingInterfaceChannel>    mDataStrobeChannelInterface;
    std::auto_ptr<AnalyzerSettingInterfaceNumberList> mShiftOrderInterface;
    std::auto_ptr<AnalyzerSettingInterfaceNumberList> mBitsPerTransferInterface;
    std::auto_ptr<AnalyzerSettingInterfaceNumberList> mClockInactiveStateInterface;
    std::auto_ptr<AnalyzerSettingInterfaceNumberList> mDataValidEdgeInterface;
    std::auto_ptr<AnalyzerSettingInterfaceNumberList> mEnableActiveStateInterface;
	std::auto_ptr<AnalyzerSettingInterfaceNumberList> mProtocolModeInterface;
};

#endif // XSPI_ANALYZER_SETTINGS
