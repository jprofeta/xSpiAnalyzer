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
    BitState mClockInactiveState;
    BitState mEnableActiveState;
    xSPIAnalyzerEnums::BusWidth mBusWidth;
    bool mDoubleRate;

protected:
    AnalyzerSettingInterfaceChannel    mEnableChannelInterface;
    AnalyzerSettingInterfaceChannel    mClockChannelInterface;
    AnalyzerSettingInterfaceChannel    mD0ChannelInterface;
    AnalyzerSettingInterfaceChannel    mD1ChannelInterface;
    AnalyzerSettingInterfaceChannel    mD2ChannelInterface;
    AnalyzerSettingInterfaceChannel    mD3ChannelInterface;
    AnalyzerSettingInterfaceChannel    mD4ChannelInterface;
    AnalyzerSettingInterfaceChannel    mD5ChannelInterface;
    AnalyzerSettingInterfaceChannel    mD6ChannelInterface;
    AnalyzerSettingInterfaceChannel    mD7ChannelInterface;
    AnalyzerSettingInterfaceChannel    mDataStrobeChannelInterface;
    AnalyzerSettingInterfaceNumberList mClockInactiveStateInterface;
    AnalyzerSettingInterfaceNumberList mEnableActiveStateInterface;
	AnalyzerSettingInterfaceNumberList mBusWidthInterface;
    AnalyzerSettingInterfaceBool       mDoubleRateInterface;
};

#endif // XSPI_ANALYZER_SETTINGS
