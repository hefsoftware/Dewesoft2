#include "DWLoadLib.h"

HINSTANCE hInstLibrary;//Dll handle
int m_InitDLL;

//functions
_DWInit DWInit;
_DWDeInit DWDeInit;
_DWAddReader DWAddReader;
_DWGetNumReaders DWGetNumReaders;
_DWSetActiveReader DWSetActiveReader;
_DWGetVersion DWGetVersion;
_DWOpenDataFile DWOpenDataFile;
_DWCloseDataFile DWCloseDataFile;
_DWGetChannelListCount DWGetChannelListCount;
_DWGetChannelList DWGetChannelList;
_DWGetChannelFactors DWGetChannelFactors;
_DWGetChannelProps DWGetChannelProps;
_DWGetBinarySamplesCount DWGetBinarySamplesCount;
_DWGetBinarySamples DWGetBinarySamples;
_DWGetBinarySamplesEx DWGetBinarySamplesEx;
_DWGetScaledSamplesCount DWGetScaledSamplesCount;
_DWGetScaledSamples DWGetScaledSamples;
_DWGetRawSamplesCount DWGetRawSamplesCount;
_DWGetRawSamples DWGetRawSamples;
_DWGetComplexChannelListCount DWGetComplexChannelListCount;
_DWGetComplexChannelList DWGetComplexChannelList;
_DWGetComplexScaledSamplesCount DWGetComplexScaledSamplesCount;
_DWGetComplexScaledSamples DWGetComplexScaledSamples;
_DWGetComplexRawSamplesCount DWGetComplexRawSamplesCount;
_DWGetComplexRawSamples DWGetComplexRawSamples;
_DWGetEventListCount DWGetEventListCount;
_DWGetEventList DWGetEventList;
_DWGetStream DWGetStream;
_DWExportHeader DWExportHeader;
_DWGetTextChannelListCount DWGetTextChannelListCount;
_DWGetTextChannelList DWGetTextChannelList;
_DWGetTextValuesCount DWGetTextValuesCount;
_DWGetTextValues DWGetTextValues;
_DWGetReducedValuesCount DWGetReducedValuesCount;
_DWGetReducedValues DWGetReducedValues;
_DWGetHeaderEntryCount DWGetHeaderEntryCount;
_DWGetHeaderEntryList DWGetHeaderEntryList;
_DWGetHeaderEntryText DWGetHeaderEntryText;
_DWGetStoringType DWGetStoringType;
_DWGetArrayInfoCount DWGetArrayInfoCount;
_DWGetArrayInfoList DWGetArrayInfoList;
_DWGetArrayIndexValue DWGetArrayIndexValue;
_DWGetArrayIndexValueF DWGetArrayIndexValueF;
//
_DWGetChannelListItem DWGetChannelListItem;
_DWGetHeaderEntryListItem DWGetHeaderEntryListItem;
_DWGetEventListItem DWGetEventListItem;
_DWGetReducedAveValues DWGetReducedAveValues;
_DWGetReducedMinValues DWGetReducedMinValues;
_DWGetReducedMaxValues DWGetReducedMaxValues;
_DWGetReducedRMSValues DWGetReducedRMSValues;
//
_DWGetHeaderEntryTextF DWGetHeaderEntryTextF;
_DWGetHeaderEntryNameF DWGetHeaderEntryNameF;
_DWGetHeaderEntryIDF DWGetHeaderEntryIDF;
_DWGetEventTimeF DWGetEventTimeF;
_DWGetEventTextF DWGetEventTextF;
_DWGetEventTypeF DWGetEventTypeF;
_DWGetReducedDataChannelCountF DWGetReducedDataChannelCountF;
_DWGetReducedDataChannelNameF DWGetReducedDataChannelNameF;
_DWGetReducedDataChannelIndexF DWGetReducedDataChannelIndexF;
_DWGetRecudedDataChannelInfoF DWGetRecudedDataChannelInfoF;
_DWGetRecudedDataF DWGetRecudedDataF;
_DWGetRecudedYDataF DWGetRecudedYDataF;
_DWGetRecudedDataAllF DWGetRecudedDataAllF;
_DWGetTriggerDataTriggerCountF DWGetTriggerDataTriggerCountF;
_DWGetTriggerDataTriggerTimeF DWGetTriggerDataTriggerTimeF;
_DWGetTriggerDataChannelNameF DWGetTriggerDataChannelNameF;
_DWGetTriggerDataChannelIndexF DWGetTriggerDataChannelIndexF;
_DWGetTriggerDataChannelInfoF DWGetTriggerDataChannelInfoF;
_DWGetTriggerDataF DWGetTriggerDataF;

int LoadDWDLL(LPCSTR lib_name)
{
    hInstLibrary = LoadLibraryA(lib_name);
	if (!hInstLibrary)
		return 0;
	DWInit = (_DWInit)GetProcAddress(hInstLibrary, "DWInit");
	if (!DWInit)
		return 0;
	DWDeInit = (_DWDeInit)GetProcAddress(hInstLibrary, "DWDeInit");
	if (!DWDeInit)
		return 0;
	DWAddReader = (_DWAddReader)GetProcAddress(hInstLibrary, "DWAddReader");
	if (!DWAddReader)
		return 0;
	DWGetNumReaders = (_DWGetNumReaders)GetProcAddress(hInstLibrary, "DWGetNumReaders");
	if (!DWGetNumReaders)
		return 0;
	DWSetActiveReader = (_DWSetActiveReader)GetProcAddress(hInstLibrary, "DWSetActiveReader");
	if (!DWSetActiveReader)
		return 0;
	DWGetVersion = (_DWGetVersion)GetProcAddress(hInstLibrary, "DWGetVersion");
	if (!DWGetVersion)
		return 0;
	DWOpenDataFile = (_DWOpenDataFile)GetProcAddress(hInstLibrary, "DWOpenDataFile");
	if (!DWOpenDataFile)
		return 0;
	DWCloseDataFile = (_DWCloseDataFile)GetProcAddress(hInstLibrary, "DWCloseDataFile");
	if (!DWCloseDataFile)
		return 0;
	DWGetChannelListCount = (_DWGetChannelListCount)GetProcAddress(hInstLibrary, "DWGetChannelListCount");
	if (!DWGetChannelListCount)
		return 0;
	DWGetChannelList = (_DWGetChannelList)GetProcAddress(hInstLibrary, "DWGetChannelList");
	if (!DWGetChannelList)
		return 0;
	DWGetChannelFactors = (_DWGetChannelFactors)GetProcAddress(hInstLibrary, "DWGetChannelFactors");
	if (!DWGetChannelFactors)
		return 0;
	DWGetChannelProps = (_DWGetChannelProps)GetProcAddress(hInstLibrary, "DWGetChannelProps");
	if (!DWGetChannelProps)
		return 0;
	DWGetBinarySamplesCount = (_DWGetBinarySamplesCount)GetProcAddress(hInstLibrary, "DWGetBinarySamplesCount");
	if (!DWGetBinarySamplesCount)
		return 0;
	DWGetBinarySamples = (_DWGetBinarySamples)GetProcAddress(hInstLibrary, "DWGetBinarySamples");
	if (!DWGetBinarySamples)
		return 0;
	DWGetBinarySamplesEx = (_DWGetBinarySamplesEx)GetProcAddress(hInstLibrary, "DWGetBinarySamplesEx");
	if (!DWGetBinarySamplesEx)
		return 0;
	DWGetScaledSamplesCount = (_DWGetScaledSamplesCount)GetProcAddress(hInstLibrary, "DWGetScaledSamplesCount");
	if (!DWGetScaledSamplesCount)
		return 0;
	DWGetScaledSamples = (_DWGetScaledSamples)GetProcAddress(hInstLibrary, "DWGetScaledSamples");
	if (!DWGetScaledSamples)
		return 0;
	DWGetRawSamplesCount = (_DWGetRawSamplesCount)GetProcAddress(hInstLibrary, "DWGetRawSamplesCount");
	if (!DWGetRawSamplesCount)
		return 0;
	DWGetRawSamples = (_DWGetRawSamples)GetProcAddress(hInstLibrary, "DWGetRawSamples");
	if (!DWGetRawSamples)
		return 0;
	DWGetComplexChannelListCount = (_DWGetComplexChannelListCount)GetProcAddress(hInstLibrary, "DWGetComplexChannelListCount");
	if (!DWGetComplexChannelListCount)
		return 0;
	DWGetComplexChannelList = (_DWGetComplexChannelList)GetProcAddress(hInstLibrary, "DWGetComplexChannelList");
	if (!DWGetComplexChannelList)
		return 0;
	DWGetComplexScaledSamplesCount = (_DWGetComplexScaledSamplesCount)GetProcAddress(hInstLibrary, "DWGetComplexScaledSamplesCount");
	if (!DWGetComplexScaledSamplesCount)
		return 0;
	DWGetComplexScaledSamples = (_DWGetComplexScaledSamples)GetProcAddress(hInstLibrary, "DWGetComplexScaledSamples");
	if (!DWGetComplexScaledSamples)
		return 0;
	DWGetComplexRawSamplesCount = (_DWGetComplexRawSamplesCount)GetProcAddress(hInstLibrary, "DWGetComplexRawSamplesCount");
	if (!DWGetComplexRawSamplesCount)
		return 0;
	DWGetComplexRawSamples = (_DWGetComplexRawSamples)GetProcAddress(hInstLibrary, "DWGetComplexRawSamples");
	if (!DWGetComplexRawSamples)
		return 0;
	DWGetEventListCount = (_DWGetEventListCount)GetProcAddress(hInstLibrary, "DWGetEventListCount");
	if (!DWGetEventListCount)
		return 0;
	DWGetEventList = (_DWGetEventList)GetProcAddress(hInstLibrary, "DWGetEventList");
	if (!DWGetEventList)
		return 0;
	DWGetStream = (_DWGetStream)GetProcAddress(hInstLibrary, "DWGetStream");
	if (!DWGetStream)
		return 0;
	DWExportHeader = (_DWExportHeader)GetProcAddress(hInstLibrary, "DWExportHeader");
	if (!DWExportHeader)
		return 0;
	DWGetTextChannelListCount = (_DWGetTextChannelListCount)GetProcAddress(hInstLibrary, "DWGetTextChannelListCount");
	if (!DWGetTextChannelListCount)
		return 0;
	DWGetTextChannelList = (_DWGetTextChannelList)GetProcAddress(hInstLibrary, "DWGetTextChannelList");
	if (!DWGetTextChannelList)
		return 0;
	DWGetTextValuesCount = (_DWGetTextValuesCount)GetProcAddress(hInstLibrary, "DWGetTextValuesCount");
	if (!DWGetTextValuesCount)
		return 0;
	DWGetTextValues = (_DWGetTextValues)GetProcAddress(hInstLibrary, "DWGetTextValues");
	if (!DWGetTextValues)
		return 0;
	DWGetReducedValuesCount = (_DWGetReducedValuesCount)GetProcAddress(hInstLibrary, "DWGetReducedValuesCount");
	if (!DWGetReducedValuesCount)
		return 0;
	DWGetReducedValues = (_DWGetReducedValues)GetProcAddress(hInstLibrary, "DWGetReducedValues");
	if (!DWGetReducedValues)
		return 0;
	DWGetHeaderEntryCount = (_DWGetHeaderEntryCount)GetProcAddress(hInstLibrary, "DWGetHeaderEntryCount");
	if (!DWGetHeaderEntryCount)
		return 0;
	DWGetHeaderEntryList = (_DWGetHeaderEntryList)GetProcAddress(hInstLibrary, "DWGetHeaderEntryList");
	if (!DWGetHeaderEntryList)
		return 0;
	DWGetHeaderEntryText = (_DWGetHeaderEntryText)GetProcAddress(hInstLibrary, "DWGetHeaderEntryText");
	if (!DWGetHeaderEntryText)
		return 0;
	DWGetStoringType = (_DWGetStoringType)GetProcAddress(hInstLibrary, "DWGetStoringType");
	if (!DWGetStoringType)
		return 0;
	DWGetArrayInfoCount = (_DWGetArrayInfoCount)GetProcAddress(hInstLibrary, "DWGetArrayInfoCount");
	if (!DWGetArrayInfoCount)
		return 0;
	DWGetArrayInfoList = (_DWGetArrayInfoList)GetProcAddress(hInstLibrary, "DWGetArrayInfoList");
	if (!DWGetArrayInfoList)
		return 0;
	DWGetArrayIndexValue = (_DWGetArrayIndexValue)GetProcAddress(hInstLibrary, "DWGetArrayIndexValue");
	if (!DWGetArrayIndexValue)
		return 0;
	DWGetArrayIndexValueF = (_DWGetArrayIndexValueF)GetProcAddress(hInstLibrary, "DWGetArrayIndexValueF");
	if (!DWGetArrayIndexValueF)
		return 0;

	//spec
	DWGetChannelListItem = (_DWGetChannelListItem)GetProcAddress(hInstLibrary, "DWGetChannelListItem");
	if (!DWGetChannelListItem)
		return 0;
	DWGetHeaderEntryListItem = (_DWGetHeaderEntryListItem)GetProcAddress(hInstLibrary, "DWGetHeaderEntryListItem");
	if (!DWGetHeaderEntryListItem)
		return 0;
	DWGetEventListItem = (_DWGetEventListItem)GetProcAddress(hInstLibrary, "DWGetEventListItem");
	if (!DWGetEventListItem)
		return 0;
	DWGetReducedAveValues = (_DWGetReducedAveValues)GetProcAddress(hInstLibrary, "DWGetReducedAveValues");
	if (!DWGetReducedAveValues)
		return 0;
	DWGetReducedMinValues = (_DWGetReducedMinValues)GetProcAddress(hInstLibrary, "DWGetReducedMinValues");
	if (!DWGetReducedMinValues)
		return 0;
	DWGetReducedMaxValues = (_DWGetReducedMaxValues)GetProcAddress(hInstLibrary, "DWGetReducedMaxValues");
	if (!DWGetReducedMaxValues)
		return 0;
	DWGetReducedRMSValues = (_DWGetReducedRMSValues)GetProcAddress(hInstLibrary, "DWGetReducedRMSValues");
	if (!DWGetReducedRMSValues)
		return 0;
	
	//spec
	DWGetHeaderEntryTextF = (_DWGetHeaderEntryTextF)GetProcAddress(hInstLibrary, "DWGetHeaderEntryTextF");
	if (!DWGetHeaderEntryTextF)
		return 0;
	DWGetHeaderEntryNameF = (_DWGetHeaderEntryNameF)GetProcAddress(hInstLibrary, "DWGetHeaderEntryNameF");
	if (!DWGetHeaderEntryNameF)
		return 0;
	DWGetHeaderEntryIDF = (_DWGetHeaderEntryIDF)GetProcAddress(hInstLibrary, "DWGetHeaderEntryIDF");
	if (!DWGetHeaderEntryIDF)
		return 0;
	DWGetEventTimeF = (_DWGetEventTimeF)GetProcAddress(hInstLibrary, "DWGetEventTimeF");
	if (!DWGetEventTimeF)
		return 0;
	DWGetEventTextF = (_DWGetEventTextF)GetProcAddress(hInstLibrary, "DWGetEventTextF");
	if (!DWGetEventTextF)
		return 0;
	DWGetEventTypeF = (_DWGetEventTypeF)GetProcAddress(hInstLibrary, "DWGetEventTypeF");
	if (!DWGetEventTypeF)
		return 0;
	DWGetReducedDataChannelCountF = (_DWGetReducedDataChannelCountF)GetProcAddress(hInstLibrary, "DWGetReducedDataChannelCountF");
	if (!DWGetReducedDataChannelCountF)
		return 0;
	DWGetReducedDataChannelNameF = (_DWGetReducedDataChannelNameF)GetProcAddress(hInstLibrary, "DWGetReducedDataChannelNameF");
	if (!DWGetReducedDataChannelNameF)
		return 0;
	DWGetReducedDataChannelIndexF = (_DWGetReducedDataChannelIndexF)GetProcAddress(hInstLibrary, "DWGetReducedDataChannelIndexF");
	if (!DWGetReducedDataChannelIndexF)
		return 0;
	DWGetRecudedDataChannelInfoF = (_DWGetRecudedDataChannelInfoF)GetProcAddress(hInstLibrary, "DWGetRecudedDataChannelInfoF");
	if (!DWGetRecudedDataChannelInfoF)
		return 0;
	DWGetRecudedDataF = (_DWGetRecudedDataF)GetProcAddress(hInstLibrary, "DWGetRecudedDataF");
	if (!DWGetRecudedDataF)
		return 0;
	DWGetRecudedYDataF = (_DWGetRecudedYDataF)GetProcAddress(hInstLibrary, "DWGetRecudedYDataF");
	if (!DWGetRecudedYDataF)
		return 0;
	DWGetRecudedDataAllF = (_DWGetRecudedDataAllF)GetProcAddress(hInstLibrary, "DWGetRecudedDataAllF");
	if (!DWGetRecudedDataAllF)
		return 0;
	DWGetTriggerDataTriggerCountF = (_DWGetTriggerDataTriggerCountF)GetProcAddress(hInstLibrary, "DWGetTriggerDataTriggerCountF");
	if (!DWGetTriggerDataTriggerCountF)
		return 0;
	DWGetTriggerDataTriggerTimeF = (_DWGetTriggerDataTriggerTimeF)GetProcAddress(hInstLibrary, "DWGetTriggerDataTriggerTimeF");
	if (!DWGetTriggerDataTriggerTimeF)
		return 0;
	DWGetTriggerDataChannelNameF = (_DWGetTriggerDataChannelNameF)GetProcAddress(hInstLibrary, "DWGetTriggerDataChannelNameF");
	if (!DWGetTriggerDataChannelNameF)
		return 0;
	DWGetTriggerDataChannelIndexF = (_DWGetTriggerDataChannelIndexF)GetProcAddress(hInstLibrary, "DWGetTriggerDataChannelIndexF");
	if (!DWGetTriggerDataChannelIndexF)
		return 0;
	DWGetTriggerDataChannelInfoF = (_DWGetTriggerDataChannelInfoF)GetProcAddress(hInstLibrary, "DWGetTriggerDataChannelInfoF");
	if (!DWGetTriggerDataChannelInfoF)
		return 0;
	DWGetTriggerDataF = (_DWGetTriggerDataF)GetProcAddress(hInstLibrary, "DWGetTriggerDataF");
	if (!DWGetTriggerDataF)
		return 0;

	return 1;
}

int CloseDWDLL()
{
	return FreeLibrary(hInstLibrary);
}
