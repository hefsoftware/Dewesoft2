//#include "dw7data.h"
//#include <QString>
//#include <stdio.h>
//#include <QDebug>
//bool DewetronData::m_initDeweLib=false;
//DewetronData *DewetronData::open(const QString &filename)
//{
//  if(!m_initDeweLib)
//  {
//#ifdef WIN64
//    //printf("WIN x64 Application\n");
//    if (!LoadDWDLL(TEXT("DWDataReaderLib64.dll")))// Load DLL
//    {
//            printf("Could not load dll object\n");
//            return NULL;
//    };
//#else
//    if (!LoadDWDLL("DWDataReaderLib.dll"))// Load DLL
//    {
//            printf("Could not load dll object\n");
//            return NULL;
//    };
//#endif

//    if(DWInit()!=DWSTAT_OK)
//    {
//      //initiate dll
//      return NULL;
//    }
//    m_initDeweLib=true;
//  }
//    DewetronData *ret=new DewetronData(filename);
//    if(ret->m_ok)
//      return ret;
//    else
//      return NULL;
//}

//DewetronData::DewetronData(const QString &filename): m_ok(false), m_opened(false)
//{
//  DWFileInfo info;
//  if(DWOpenDataFile(filename.toUtf8().data(), &info)!=DWSTAT_OK)
//    return;
//  m_opened=true;
//  int nchannels;
//  QByteArray data;
//  DWChannel *channels=NULL;

//  nchannels=DWGetChannelListCount();
//  if(nchannels<0)
//    return;
//  data.resize(sizeof(DWChannel)*nchannels);
//  channels=(DWChannel *)data.data();
//  if(DWGetChannelList(channels)!=DWSTAT_OK)
//    return;
//  for(int n=0;n<nchannels;n++)
//  {
//    qDebug()<<n+1<<channels[n].index<<channels[n].name<<'/'<<channels[n].unit<<'/'<<channels[n].description<<'/'<<channels[n].array_size;
//    QString unit=QString::fromLatin1(channels[n].unit);
////    QString unit=QString::fromLocal8Bit(channels[n].unit);
//    WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), unit.utf16(), unit.size(), NULL, NULL);


//  }
//  qDebug()<<"End channels";

//  nchannels=DWGetHeaderEntryCount();
//  if(nchannels<0)
//    return;
//  data.resize(sizeof(DWChannel)*nchannels);
//  channels=(DWChannel *)data.data();
//  if(DWGetHeaderEntryList(channels)!=DWSTAT_OK)
//    return;
//  for(int n=0;n<nchannels;n++)
//  {
//    qDebug()<<n+1<<channels[n].name;
//  }
//  qDebug()<<"End entries";

//  m_ok=true;
//}
