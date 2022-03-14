
#----------------------------------------------------------------------------------------------------------------
# DWDataReader example for Python
#----------------------------------------------------------------------------------------------------------------
# Author: Dewesoft
# Notes:
#   - requires DWDataReaderLib.dll 4.0.0.0 or later
#   - tested with Python 3.4
#----------------------------------------------------------------------------------------------------------------

from DWDataReaderHeader import *
from ctypes import *
import _ctypes

lib = cdll.LoadLibrary('DWDataReaderLib.dll')

# init data reader
if lib.DWInit() != DWStatus.DWSTAT_OK.value:
    DWRaiseError("DWDataReader: DWInit() failed")

# get data reader version
print("DWDataReader version: " + str(lib.DWGetVersion()))

# add additional data reader
if lib.DWAddReader() != DWStatus.DWSTAT_OK.value:
    DWRaiseError("DWDataReader: DWAddReader() failed")

# get number of open data readers
num = c_int()
if lib.DWGetNumReaders(byref(num)) != DWStatus.DWSTAT_OK.value:
    DWRaiseError("DWDataReader: DWGetNumReaders() failed")
print("Number of data readers: " + str(num.value))

# open data file
# data file must be in the same folder as the python script
str = input('Please enter a data file name (.d7d, .d7z or .dxd):')
file_name = c_char_p(str.encode())
file_info = DWFileInfo(0, 0, 0)
if lib.DWOpenDataFile(file_name, c_void_p(addressof(file_info))) != DWStatus.DWSTAT_OK.value:
    DWRaiseError("DWDataReader: DWOpenDataFile() failed")

print("Sample rate: %.2f" %  file_info.sample_rate)
print("Start store time: %.2f" % file_info.start_store_time)
print("Duration: %.2f" % file_info.duration)

# export XML
str = 'SetupFile.xml'
file_name = c_char_p(str.encode())
if lib.DWExportHeader(file_name) != DWStatus.DWSTAT_OK.value:
    DWRaiseError("DWDataReader: DWExportHeader() failed")

# get num channels
num = lib.DWGetChannelListCount()
if num == -1:
    DWRaiseError("DWDataReader: DWGetChannelListCount() failed")
print("Number of channels: %d" % num)

# get channel list
ch_list = (DWChannel * num)()
if lib.DWGetChannelList(byref(ch_list)) != DWStatus.DWSTAT_OK.value:
    DWRaiseError("DWDataReader: DWGetChannelList() failed")

print("\n")

#----------------------------------------------------------------------------------------------------------------
# channel loop
#----------------------------------------------------------------------------------------------------------------
for i in range(0, num):
    # basic channel properties
    print("************************************************")
    print("Channel #%d" % i)
    print("************************************************")
    print("Index: %d" % ch_list[i].index)
    print("Name: %s" % ch_list[i].name)
    print("Unit: %s" % ch_list[i].unit)
    print("Description: %s" % ch_list[i].description)

    # channel factors
    idx = c_int(i)
    ch_scale = c_double()
    ch_offset = c_double()
    if lib.DWGetChannelFactors(idx, byref(ch_scale), byref(ch_offset)) != DWStatus.DWSTAT_OK.value:
        DWRaiseError("DWDataReader: DWGetChannelFactors() failed")

    print("Scale: %.2f" % ch_scale.value)
    print("Offset: %.2f" % ch_offset.value)

    # channel type
    max_len = c_int(INT_SIZE)
    buff = create_string_buffer(max_len.value)
    p_buff = cast(buff, POINTER(c_void_p))
    if lib.DWGetChannelProps(idx, c_int(DWChannelProps.DW_CH_TYPE.value), p_buff, byref(max_len)) != DWStatus.DWSTAT_OK.value:
        DWRaiseError("DWDataReader: DWGetChannelProps() failed")
    ch_type = cast(p_buff, POINTER(c_int)).contents

    if ch_type.value == DWChannelType.DW_CH_TYPE_SYNC.value:
        print("Channel type: sync")
    elif ch_type.value == DWChannelType.DW_CH_TYPE_ASYNC.value:
        print("Channel type: async")
    elif ch_type.value == DWChannelType.DW_CH_TYPE_SV.value:
        print("Channel type: single value")
    else:
        print("Channel type: unknown")

    # channel data type
    if lib.DWGetChannelProps(idx, c_int(DWChannelProps.DW_DATA_TYPE.value), p_buff, byref(max_len)) != DWStatus.DWSTAT_OK.value:
        DWRaiseError("DWDataReader: DWGetChannelProps() failed")
    data_type = cast(p_buff, POINTER(c_int)).contents
    print("Data type: %s" % DWDataType(data_type.value).name)

    # number of samples
    dw_ch_index = c_int(ch_list[i].index)
    sample_cnt = c_int()
    sample_cnt = lib.DWGetScaledSamplesCount(dw_ch_index)
    if sample_cnt < 0:
        DWRaiseError("DWDataReader: DWGetScaledSamplesCount() failed")
    print("Num. samples: %d" % sample_cnt)

    # get actual data
    data = create_string_buffer(DOUBLE_SIZE * sample_cnt * ch_list[i].array_size)
    time_stamp = create_string_buffer(DOUBLE_SIZE * sample_cnt)
    p_data = cast(data, POINTER(c_double))
    p_time_stamp = cast(time_stamp, POINTER(c_double))
    if lib.DWGetScaledSamples(dw_ch_index, c_int64(0), sample_cnt, p_data, p_time_stamp) != DWStatus.DWSTAT_OK.value:
        DWRaiseError("DWDataReader: DWGetScaledSamples() failed")

    # diplay data
    print("Data:")
    for j in range(0, sample_cnt):
        for k in range(0, ch_list[i].array_size):
            print("  Time: %.6f   Value=%.2f" % (p_time_stamp[j], p_data[j * ch_list[i].array_size + k]))

    print("\n")
#----------------------------------------------------------------------------------------------------------------
# end channel loop
#----------------------------------------------------------------------------------------------------------------

# close data file
if lib.DWCloseDataFile() != DWStatus.DWSTAT_OK.value:
    DWRaiseError("DWDataReader: DWCloseDataFile() failed")

# deinit
if lib.DWDeInit() != DWStatus.DWSTAT_OK.value:
    DWRaiseError("DWDataReader: DWDeInit() failed")

# close DLL
_ctypes.FreeLibrary(lib._handle)
del lib
