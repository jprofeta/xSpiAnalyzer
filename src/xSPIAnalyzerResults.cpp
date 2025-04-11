#include "xSPIAnalyzerResults.h"
#include <AnalyzerHelpers.h>
#include "xSPIAnalyzer.h"
#include "xSPIAnalyzerSettings.h"
#include "xSPIAnalyzerFrames.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <string.h>

using namespace xSPIAnalyzerEnums;

xSPIAnalyzerResults::xSPIAnalyzerResults( xSPIAnalyzer* analyzer, xSPIAnalyzerSettings* settings )
:	AnalyzerResults(),
	mSettings( settings ),
	mAnalyzer( analyzer )
{
}

xSPIAnalyzerResults::~xSPIAnalyzerResults()
{
    for( const auto& frame : mDataFrames )
        delete frame;
    mDataFrames.clear();
}

void xSPIAnalyzerResults::GenerateBubbleText( U64 frame_index, Channel& channel, DisplayBase display_base )
{
	ClearResultStrings();
	Frame frame = GetFrame( frame_index );
    std::ostringstream bubbleText;

    if (!frame.HasFlag(FrameFlag::DataIsIFramePtr))
    {
        return;
    }

    IFrame* dataFrame = ( IFrame* )frame.mData1;

    if (mSettings->mBusWidth == BusWidth::OneLane)
    {
        if( channel == mSettings->mD0Channel )
        {
            dataFrame->GenerateBubbleText( bubbleText, display_base, 0 );
        }
        else if (channel == mSettings->mD1Channel)
        {
            dataFrame->GenerateBubbleText( bubbleText, display_base, 1 );
        }
    }
    else
    {
        if (channel == mSettings->mEnableChannel)
        {
            dataFrame->GenerateBubbleText( bubbleText, display_base );
        }
    }

    AddResultString( bubbleText.str().c_str() );
}

void xSPIAnalyzerResults::GenerateExportFile( const char* file, DisplayBase display_base, U32 export_type_user_id )
{
	std::ofstream file_stream( file, std::ios::out );

	U64 trigger_sample = mAnalyzer->GetTriggerSample();
	U32 sample_rate = mAnalyzer->GetSampleRate();

	file_stream << "Time [s],Value" << std::endl;

	U64 num_frames = GetNumFrames();
	for( U32 i=0; i < num_frames; i++ )
	{
		Frame frame = GetFrame( i );
		
		char time_str[128];
		AnalyzerHelpers::GetTimeString( frame.mStartingSampleInclusive, trigger_sample, sample_rate, time_str, 128 );

		char number_str[128];
		AnalyzerHelpers::GetNumberString( frame.mData1, display_base, 8, number_str, 128 );

		file_stream << time_str << "," << number_str << std::endl;

		if( UpdateExportProgressAndCheckForCancel( i, num_frames ) == true )
		{
			file_stream.close();
			return;
		}
	}

	file_stream.close();
}

void xSPIAnalyzerResults::GenerateFrameTabularText( U64 frame_index, DisplayBase display_base )
{
#ifdef SUPPORTS_PROTOCOL_SEARCH
	Frame frame = GetFrame( frame_index );
	ClearTabularText();

	char number_str[128];
	AnalyzerHelpers::GetNumberString( frame.mData1, display_base, 8, number_str, 128 );
	AddTabularText( number_str );
#endif
}

void xSPIAnalyzerResults::GeneratePacketTabularText( U64 packet_id, DisplayBase display_base )
{
	//not supported

}

void xSPIAnalyzerResults::GenerateTransactionTabularText( U64 transaction_id, DisplayBase display_base )
{
	//not supported
}
