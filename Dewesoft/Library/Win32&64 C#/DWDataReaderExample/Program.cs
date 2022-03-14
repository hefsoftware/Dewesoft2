using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Reflection;
using System.Runtime.InteropServices;

namespace DWDataReaderExample
{
    class Program
    {
        static void Main(string[] args)
        {
            DWDataReader.DWInit();
            int ver = DWDataReader.DWGetVersion();

            DWDataReader.DWFileinfo FileInfo = new DWDataReader.DWFileinfo();
            DWDataReader.DWOpenDataFile("C:\\DEWESoft\\Data\\TestRandom.dxd", ref FileInfo);
            Console.WriteLine("Sample rate: {0}", FileInfo.sample_rate);
            Console.WriteLine("Start store time: {0}", FileInfo.start_store_time);
            Console.WriteLine("Duration: {0}", FileInfo.duration);

            Int64 channelCount = DWDataReader.DWGetChannelListCount();
            Console.WriteLine("Number of channels: {0}", channelCount);

            int eventCount = DWDataReader.DWGetEventListCount();
            DWDataReader.DWEvent[] dwInfoList = new DWDataReader.DWEvent[eventCount];
            DWDataReader.DWGetEventList(dwInfoList);

            for (int i = 0; i < eventCount; i++) {
                Console.WriteLine("EVENT: type = {0}, text = {1}, position = {2}", dwInfoList[i].event_type, dwInfoList[i].event_text, dwInfoList[i].time_stamp);
            }

            int channelListCount = DWDataReader.DWGetChannelListCount();
            DWDataReader.DWChannel[] dwChannelList = new DWDataReader.DWChannel[channelListCount];
            DWDataReader.DWGetChannelList(dwChannelList);

            for (int i = 0; i < channelListCount; i++) {
                Console.WriteLine("FULL SAMPLE RATE:");
                Console.WriteLine("Index={0}; Name={1}; Unit={2}; Description={3}", dwChannelList[i].index, dwChannelList[i].name, dwChannelList[i].unit, dwChannelList[i].description);
                Int64 numberOfSamples = DWDataReader.DWGetScaledSamplesCount(dwChannelList[i].index);
                double[] data;
                double[] timeStamp = new double[numberOfSamples];

                if (dwChannelList[i].array_size > 1) {
                    data = new double[numberOfSamples * dwChannelList[i].array_size];
                    DWDataReader.DWGetScaledSamples(dwChannelList[i].index, 0, (int)numberOfSamples * dwChannelList[i].array_size, data, timeStamp);
                }
                else {
                    data = new double[numberOfSamples];
                    DWDataReader.DWGetScaledSamples(dwChannelList[i].index, 0, (int)numberOfSamples, data, timeStamp);
                }
                for (int j = 0; j < numberOfSamples; j++)
                    Console.WriteLine("Value: {0}; Timestamp: {1}", data[j], timeStamp[j]);

                int reducedDataCount = 0;
                double block = 0.0;
                DWDataReader.DWGetReducedValuesCount(dwChannelList[i].index, ref reducedDataCount, ref block);
                Console.WriteLine("REDUCED VALUES:");
                DWDataReader.DWReducedValue[] reducedValuesList = new DWDataReader.DWReducedValue[reducedDataCount];
                DWDataReader.DWGetReducedValues(dwChannelList[i].index, 0, reducedDataCount, reducedValuesList);
                for (int j = 0; j < reducedDataCount; j++)
                    Console.WriteLine("Time={0}; Ave={1}; Rms={2}, Min={3}; Max={4};", reducedValuesList[j].time_stamp, reducedValuesList[j].ave, reducedValuesList[j].rms, reducedValuesList[j].min, reducedValuesList[j].max);
            }

            DWDataReader.DWCloseDataFile();
            DWDataReader.DWDeInit();
    }
}
}

