//  for(int i=0;i<m_data->channels.size();i++)
//  {
//    qDebug()<<i<<m_data->channels[i].name<<QByteArray(m_data->channels[i].name).toHex()<<QString(m_data->channels[i].name);
//  }
// e4b893 专 => 4E13
// c3a0 à => E0 (unicode C3 80)
// c2b1 ± => (unicode )
//  DW7DataFilePrivate::debugChannelList(m_data->channels);
//  QByteArray data;

//  int nchannels=DWGetChannelListCount();
//  if(nchannels<0)
//  {
//    m_errorMsg="Error in DWGetChannelListCount";
//    return;
//  }
//  data.resize(sizeof(DWChannel)*(unsigned int)nchannels);
//  DWChannel *channels;
//  channels=reinterpret_cast<DWChannel *>(data.data());
//  if(DWGetChannelList(channels)!=DWSTAT_OK)
//  {
//    m_errorMsg="Failed to get channel list";
//    return;
//  }
//  for(int n=0;n<nchannels;n++)
//    m_data->channels.append(channels[n]);
//  //printf("%d %d\n",(int)((char *)(&channels[1])-(char *)(&channels[0])),sizeof(DWChannel));
//  for(int n=0;n<nchannels;n++)
//  {
//    //m_channels.append(channels[n].name);
//    //channels=(DWChannel *)data.data();
//    DWChannel cch=channels[n];
//    //printf("%4d %s,%d,%s\n",cch.index,cch.name,cch.array_size,cch.unit);
//    //cch.name[sizeof(cch.name)-1]='\0';
//    //cch.description[sizeof(cch.description)-1]='\0';
//    //cch.unit[sizeof(cch.unit)-1]='\0';
//    //m_dwChannels.insert(QString::fromLocal8Bit(cch.name),cch);
//    DataChannel ch;
//    //printf("%s %d\n",cch.name,cch.array_size);
//    ch.id=QString::number(cch.index);
//    ch.name=QString::fromLocal8Bit(cch.name);
//    ch.arraySize=cch.array_size;
//    m_channels.append(ch);
//    m_channelById.insert(ch.id,ch);
//    m_idByName.insert(ch.name, ch.id);
//    //m_channelByIdEx.insert(ch.id,cch);
//  }

//  // Get list of complex channels


//  //printf("%d %d\n",(int)((char *)(&channels[1])-(char *)(&channels[0])),sizeof(DWChannel));
//  for(int n=0;n<nchannels;n++)
//  {
//    //m_channels.append(channels[n].name);
//    //channels=(DWChannel *)data.data();
//    DWChannel cch=channels[n];
//    //printf("%4d %s,%d,%s\n",cch.index,cch.name,cch.array_size,cch.unit);
//    //cch.name[sizeof(cch.name)-1]='\0';
//    //cch.description[sizeof(cch.description)-1]='\0';
//    //cch.unit[sizeof(cch.unit)-1]='\0';
//    //m_dwChannels.insert(QString::fromLocal8Bit(cch.name),cch);
//    //printf("%s %d\n",cch.name,cch.array_size);
//    QString bidx=QString::fromLatin1("CPX")+QString::number(cch.index);
//    QString bname=QString::fromLocal8Bit(cch.name);
//    DataChannel ch;
//    ch.id=bidx;
//    ch.name=bname;
//    ch.arraySize=cch.array_size;
//    m_channels.append(ch);
//    m_channelById.insert(ch.id,ch);
//    m_idByName.insert(ch.name, ch.id);
//    //ch.arraySize=cch.array_size;
//    //m_channels.append(ch);
//    //m_channelById.insert(ch.id,ch);
//    //m_idByName.insert(ch.name, ch.id);
//    for(int j=0;j<cch.array_size;j++)
//    {
//      ch.id=bidx+"/"+QString::number(j);
//      ch.name=QString::fromLatin1("%2[%1]").arg(j).arg(bname);
//      ch.arraySize=1;
//      m_channels.append(ch);
//      m_channelById.insert(ch.id,ch);
//      m_idByName.insert(ch.name, ch.id);
//      //qDebug()<<ch.name<<ch.arraySize;
//    }
//    //m_channelByIdEx.insert(ch.id,cch);
//  }

//  int nevents=DWGetEventListCount();
//  if(nevents<0)
//  {
//    m_errorMsg="Nevents<0";
//    return;
//  }
//  DWEvent *events;
//  data.resize(sizeof(DWEvent)*nevents);
//  events=(DWEvent *)data.data();
//  if(DWGetEventList(events)!=DWSTAT_OK)
//  {
//    m_errorMsg="Failed to get event list";
//    return;
//  }
//  DataRecord curRecord;
//  bool emptyRecord=true;
//  for(int n=0;n<nevents;n++)
//  {
//    curRecord.events.append(events[n]);
//    if(events[n].event_type==etStart)
//    {
//      curRecord.start=events[n].time_stamp;
//      curRecord.reason=events[n].event_text;
//      emptyRecord=false;
//    }
//    else if(events[n].event_type==etStop)
//    {
//      curRecord.end=events[n].time_stamp;
//      curRecord.duration=curRecord.end-curRecord.start;
//      if(!emptyRecord)
//        m_records.append(curRecord);
//      curRecord=DataRecord();
//    }
//    //m_channels.append(channels[n].name);
//   // printf("%d %s %f %d\n",n,events[n].event_text,events[n].time_stamp,events[n].event_type);
//    m_events.append(events[n]);
//  }
////  for(int n=0;n<m_records.count();n++)
////  {
////    printf("%d %f %f %s\n",n,m_records[n].start,m_records[n].end,qPrintable(m_records[n].reason));
////  }

//  //  qDebug()<<n+1<<channels[n].index<<channels[n].name<<'/'<<channels[n].unit<<'/'<<channels[n].description<<'/'<<channels[n].array_size;
//  //    QString unit=QString::fromLatin1(channels[n].unit);
//  ////    QString unit=QString::fromLocal8Bit(channels[n].unit);
//  //    WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), unit.utf16(), unit.size(), NULL, NULL);


//  //  }


//qint64 DW7DataFile::channelNumSamples(int channel)
//{
//  if(!m_numSamplesCache.contains(channel))
//  {
//    qint64 value=DWGetScaledSamplesCount(channel);
//    m_numSamplesCache.insert(channel,value);
//  }
//  return m_numSamplesCache[channel];
//}

//qint64 DW7DataFile::complexChannelNumSamples(int channel)
//{
//  if(!m_numSamplesComplexCache.contains(channel))
//  {
//    qint64 value=DWGetComplexScaledSamplesCount(channel);
//    m_numSamplesComplexCache.insert(channel,value);
//  }
//  return m_numSamplesComplexCache[channel];
//}

//bool DW7DataFile::channelGet(int channel, qint64 start, qint64 length, double *x, double *y)
//{
//  return DWGetScaledSamples(channel,start,length,y,x)==DWSTAT_OK;
//}

//QList<double> DW7DataFile::channelArrayKeys(int id, QString *reason)
//{
//  QList<double> ret;
//  //qDebug()<<DWGetArrayInfoCount(id);
//  if(DWGetArrayInfoCount(id)<1)
//  {
//    if(reason)
//      *reason=QObject::tr("Array has not at least one dimension");
//    return ret;
//  }
//  DWArrayInfo info;
//  info.index=0;
//  if(DWGetArrayInfoList(id, &info)!=DWSTAT_OK)
//  {
//    if(reason)
//      *reason=QObject::tr("DWGetArrayInfoList failed");
//    return ret;
//  }
//  //qDebug()<<info.size<<info.index;
//  for(int i=0;i<info.size;i++)
//  {
////    char temp[8192]="";
////    int retErr;
//    //qDebug()<<"Getting index value"<<id<<0<<i;
////    if((retErr=DWGetArrayIndexValue(id,0,i,temp,sizeof(temp)))!=DWSTAT_OK)
////    {
////      qDebug()<<"ArrayIndexValue failed"<<retErr;
////      ret.clear();
////      if(reason)
////        *reason=QObject::tr("DWGetArrayIndexValue failed");
////      return ret;
////    }
////    temp[sizeof(temp-1)]='\0';
//    double val;
//    //qDebug()<<"Getting"<<id<<i;
//    if(DWGetArrayIndexValueF(id,0,i,&val)!=DWSTAT_OK)
//    {
//      ret.clear();
//      if(reason)
//        *reason=QObject::tr("DWGetArrayIndexValue failed");
//      return ret;
//    }
//    //qDebug()<<i<<val;
//    ret.append(val);
////    ret.append(QString(temp).toDouble(&ok));
////    if(!ok)
////    {
////      ret.clear();
////      if(reason)
////        *reason=QObject::tr("String to double failed");
////      return ret;
////    }
//  }
//  //qDebug()<<"Returning";
//  return ret;
//}
//bool DW7DataFile::complexChannelGet(int channel, qint64 start, qint64 length, double *x, DWComplex *y)
//{
//    return DWGetComplexScaledSamples(channel,start,length,y,x)==DWSTAT_OK;
//}

//QList<double> DW7DataFile::complexChannelArrayKeys(int id, QString *reason)
//{
//    QList<double> ret;
//    QByteArray buffer;
//    buffer.resize(1000);
//    int i=buffer.size();
//    qDebug()<<id;
////    if(DWGetChannelProps(id, DW_CH_INDEX, buffer.data(), &i)!=DWSTAT_OK)
////      qDebug()<<"GetChannelProps failed";
////    else
////      qDebug()<<i<<buffer.left(i);
////    qDebug()<<"ComplexChannel InfoCount"<<DWGetArrayInfoCount(id);

//    if(DWGetArrayInfoCount(id)!=1)
//    {
//      if(reason)
//        *reason=QObject::tr("Array is not mono-dimensional");
//      return ret;
//    }
//    DWArrayInfo info;
//    if(DWGetArrayInfoList(id, &info)!=DWSTAT_OK)
//    {
//      if(reason)
//        *reason=QObject::tr("DWGetArrayInfoList failed");
//      return ret;
//    }
//    //qDebug()<<"ComplexChannelInfoList"<<info.size<<info.name<<info.unit<<info.index;
//    for(int i=0;i<info.size;i++)
//    {
//        char temp[8192]="";
//        if(DWGetArrayIndexValue(id,0,i,temp,500)!=DWSTAT_OK)
//        {
//          qDebug()<<"Failed A";
//          return ret;
//        }
//        double val=qQNaN();
//        if(DWGetArrayIndexValueF(id,0,i,&val)!=DWSTAT_OK)
//        {
//          ret.clear();
//          if(reason)
//            *reason=QObject::tr("DWGetArrayIndexValueF failed");
//          qDebug()<<"Failed B";
//          return ret;
//        }
//        temp[sizeof(temp)-1]='\0';
//        qDebug()<<i<<val<<temp;
//    }
//    return ret;
//    for(int i=1;i<info.size;i++)
//    {
//      char temp[8192]="";
//      if(DWGetArrayIndexValue(id,0,i,temp,sizeof(temp))!=DWSTAT_OK)
//      {
//        //qDebug()<<"Failed A";
//        return ret;
//      }
//      double val=qQNaN();
//      if(DWGetArrayIndexValueF(id,0,i,&val)!=DWSTAT_OK)
//      {
//        ret.clear();
//        if(reason)
//          *reason=QObject::tr("DWGetArrayIndexValueF failed");
//        //qDebug()<<"Failed";
//        return ret;
//      }
//      //qDebug()<<i<<val;
//      temp[sizeof(temp)-1]='\0';
//      //qDebug()<<"ComplexChannelArrKeys"<<temp<<"_";
//      bool ok;
//      ret.append(QString(temp).toDouble(&ok));
//      if(!ok)
//      {
//        ret.clear();
//        if(reason)
//          *reason=QObject::tr("String to double failed");
//        return ret;
//      }
//    }
//    //qDebug()<<"ComplexChannelArrKeys"<<ret;
//    return ret;
//}



//qint64 DW7DataFile::numSamples(const QString &id)
//{
//  qint64 ret=-1;
//  DW7DataSource *src=getSource(id);
//  if(src)
//  {
//    ret=src->length();
//    delete src;
//  }
//  return ret;
//}

//DW7DataSource *DW7DataFile::getSource(const QString &id)
//{
//  int arraySize=1;
//  QString curId=id;
//  int subIndex=-1;
//  bool ok=true;
//  if(curId.contains('/'))
//  {
//    //qDebug()<<id<<"has subindex";
//    int sepIdx=curId.indexOf('/');
//    subIndex=curId.mid(sepIdx+1).toInt(&ok);
//    curId=curId.left(sepIdx);
//  }
//  if(ok)
//  {
//    if(m_channelById.contains(curId))
//      arraySize=m_channelById[curId].arraySize;
//    else
//      ok=false;
//    //qDebug()<<"CHA"<<curId<<arraySize<<ok;
//  }
//  DW7DataSource *ret=nullptr;
//  if(ok)
//  {
//    if(curId.startsWith("CPX"))
//    {
//      int i=curId.mid(3).toInt(&ok);
//      if(ok)
//        ret=new DW7DataSourceComplexChannel(this, i, arraySize, DW7DataSourceComplexChannel::Magnitude);
//    }
//    else
//    {
//      int i=id.toInt(&ok);
//      if(ok)
//        ret=getSourceForChannel(i, arraySize);
//    }
//  }
//  if(!ret)
//  {
//    ret=new DW7DataSourceNull;
//  }
//  else if(subIndex>=0){
//    ret=new DW7DataSourceSubIndex(ret, subIndex);
//  }
//  return ret;
//}

//DW7DataSource *DW7DataFile::getSourceForChannel(int channel, int width)
//{
//  return new DW7DataSourceChannel(this, channel, width);
//}



//bool DW7DataFile::channelGetSingle(int channel, qint64 sample, double &x, double &y)
//{
//  if(sample<0 || sample>=channelNumSamples(channel))
//  {
//    x=y=NAN;
//    return false;
//  }
//  else
//    return DWGetScaledSamples(channel,sample,1,&y,&x)==DWSTAT_OK;
//}

//qint64 DW7DataFile::channelGetSampleAfterTime(int channel,double time)
//{
//  qint64 low=0, high=channelNumSamples(channel);
//  double x, y;
//  while(low!=high)
//  {
//    qint64 mid=(low+high)/2;
//    channelGetSingle(channel,mid,x,y);
//    if(x<time)
//      low=mid+1;
//    else
//      high=mid;
//  }
//  return low;
////  channelGet(channel,low,x,y);
////  printf("Find %f leads to %lld (%f)\n",time,low,x);
////  channelGet(channel,low-1,x,y);
////  printf("Before: %f\n",x);
//  //  fflush(stdout);
//}

//Error DW7DataFile::channelNumSamples(int channel, qint64 &result)
//{
//  QMutexLocker lock(&dw7Mutex);
//  if(!ensureActiveReader())
//    return Error::Lock;
//  return (result=DWGetScaledSamplesCount(channel))>=0?Error::Success:Error::Generic;
//}


//int DW7DataFile::numChannelsDebug()
//{
//  QMutexLocker lock(&dw7Mutex);
//  return ensureActiveReader()?DWGetChannelListCount():-1;
//}

//DataChannel DW7DataFile::channelByName(const QString &name)
//{
// DataChannel ret;
// if(m_idByName.contains(name))
//   ret=m_channelById[m_idByName[name]];
// else
// {
//   ret.id=ret.name="";
// }
// return ret;
//}

//DataChannel DW7DataFile::channelById(const QString &id)
//{
//  DataChannel ret;
//  if(m_channelById.contains(id))
//    ret=m_channelById[id];
//  else
//  {
//    ret.id=ret.name="";
//  }
//  return ret;
//}
//qint64 DW7DataFile::channelGetSampleBeforeTime(int channel,double time)
//{
//    double x,y;
//    qint64 sample;
//    sample=channelGetSampleAfterTime(channel,time);
//    channelGetSingle(channel,sample,x,y);
//  return x==time?sample:sample-1;
//}

//QStringList DW7DataFile::listDir(QString directory)
//{
//  //splitPath(directory)
//  QStringList path=splitPath(directory);
//  if(path.isEmpty())
//  {
//    return QStringList()<<"Channels";
//  }
//  else if(path[0]=="Channels")
//  {
//    if(path.size()==1)
//      return QStringList(m_channels.keys());
//    else if(path.size()==2)
//    {
//      //return QStringList()<<"X"<<"Y";
//    }
//  }
//  return QStringList();
//}
//QMap<QString,QVariant> DW7DataFile::metaData(const QString &path)
//{
//  QStringList plist=splitPath(path);
//  return metaData(plist);
//}
//QMap<QString,QVariant> DW7DataFile::metaData(const QStringList &directory)
//{
//  QMap<QString,QVariant> ret;
//  if(directory.isEmpty())
//  {

//  }
//  else if(directory[0]=="Channels")
//  {
//    if(directory.size()==1)
//      ret.insert("nchannels",m_channels.size());
//    else
//    {
//      if(directory.size()==2)
//      {
//        if(m_channels.contains(directory[1]))
//        {
//          const DWChannel &ch=m_channels[directory[1]];
//          ret.insert("unitX","s");
//          ret.insert("unitY",ch.unit);
//          //ret.insert("name",ch.name);
//          if(ch.description[0])
//            ret.insert("description",ch.description);
//          ret.insert("size",DWGetScaledSamplesCount(ch.index));
//          //ret.insert("unit",ch.unit);
//        }
//        //ret.insert("size",1500);
//        //ret.insert("unitX","s");
//        //ret.insert("unitY","km/h");
//      }
//    }
//  }
//  return ret;
//}


//DW7CachedRead::DW7CachedRead(DW7DataFile &file, int channel):m_file(file)
//{
//  m_channel=channel;
//  m_size=file.channelNumSamples(channel);
//  m_cacheStart=m_cacheEnd=-1;
//  m_cacheSize=1024*64;
//  m_cacheUsed=0;
//  m_cacheXData.resize(m_cacheSize/**sizeof(double)*/);
//  m_cacheYData.resize(m_cacheSize/**sizeof(double)*/);
//  m_cacheX=m_cacheXData.data();
//  m_cacheY=m_cacheYData.data();
//  m_numCacheMiss=0;
//}

//DW7CachedRead::DW7CachedRead(DW7DataSource *source): m_source(source)
//{
//  m_size=source->length();
//  m_cacheStart=m_cacheEnd=-1;
//  m_cacheSize=1024*64;
//  m_cacheUsed=0;
//  m_cacheXData.resize(m_cacheSize/**sizeof(double)*/);
//  m_cacheYData.resize(m_cacheSize/**sizeof(double)*/);
//  m_cacheX=m_cacheXData.data();
//  m_cacheY=m_cacheYData.data();
//  m_numCacheMiss=0;
//}

//DW7CachedRead::~DW7CachedRead()
//{
//#if defined(DEBUG_DATAFILE)
//  qDebug()<<"Cache misses: "<<m_numCacheMiss;
//#endif
//  delete m_source;
//}

//double DW7CachedRead::x(qint64 sample)
//{
//  if(!check(sample))
//    return qQNaN();
//  return m_cacheX[sample-m_cacheStart];
//}

//double DW7CachedRead::y(qint64 sample)
//{
//  if(!check(sample))
//    return qQNaN();
//  return m_cacheY[sample-m_cacheStart];
//}
//double DW7CachedRead::xUncached(qint64 sample)
//{
//  if(!m_cacheUsed)
//    return x(sample);
//  else if(inCache(sample))
//    return m_cacheX[sample-m_cacheStart];
//  else
//  {
//    double x,y;
//    if(!readSamples(sample,1,&x, &y))
//      return qQNaN();
////    if(!m_file.channelGet(m_channel,sample,x,y))
////      return NAN;
//    return x;
//  }
//}

//double DW7CachedRead::yUncached(qint64 sample)
//{
//  if(!m_cacheUsed)
//    return y(sample);
//  else if(inCache(sample))
//    return m_cacheY[sample-m_cacheStart];
//  else
//  {
//    double x,y;
//    if(!readSamples(sample,1,&x, &y))
//      return qQNaN();
////    if(!m_file.channelGet(m_channel,sample,y,x))
////      return NAN;
//    return y;
//  }
//}

//qint64 DW7CachedRead::getSampleBeforeTime(double time)
//{
//  if(m_cacheUsed && time>=m_cacheX[0] && time<=m_cacheX[m_cacheUsed-1])
//    return getSampleBeforeTime(time,m_cacheStart,m_cacheEnd);
//  else
//    return getSampleBeforeTime(time,0,m_size);
//}

//qint64 DW7CachedRead::getSampleAfterTime(double time)
//{
//  if(m_cacheUsed && time>=m_cacheX[0] && time<=m_cacheX[m_cacheUsed-1])
//    return getSampleAfterTime(time,m_cacheStart,m_cacheEnd);
//  else
//    return getSampleAfterTime(time,0,m_size);
//}

//double DW7CachedRead::getAt(double time)
//{
//  qint64 s=getSampleBeforeTime(time);
//  if(s<=0)
//    return qQNaN();
//  //{
//  //  return NAN;
//  double x0=x(s),y0=y(s),x1=x(s+1),y1=y(s+1);
//  if(qIsNaN(y0)||qIsNaN(y1))
//    return qQNaN();
//  else if(x0==x1)
//  {
//    for(s++;s<nSamples();s++)
//    {
//      x1=x(s); y1=y(s);
//      if(x1!=x0)
//        break;
//    }
//    if(s>=nSamples())
//      return qQNaN();
//  }
//  return (time-x0)/(x1-x0)*(y1-y0)+y0;
//}

//qint64 DW7CachedRead::getSampleBeforeTime(double time,qint64 low,qint64 high)
//{
//  qint64 sample;
//  sample=getSampleAfterTime(time,low,high);
//  return x(sample)==time?sample:sample-1;
//}

//qint64 DW7CachedRead::getSampleAfterTime(double time,qint64 low,qint64 high)
//{
//  //qint64 low=0, high=m_size;
//  double vx;
//  while(low!=high)
//  {
//    qint64 mid=(low+high)/2;
//    vx=xUncached(mid);
//    if(vx<time)
//      low=mid+1;
//    else
//      high=mid;
//  }
//  return low;
//}

//bool DW7CachedRead::load(qint64 sample)
//{
//  if(sample<0 || sample>=m_size)
//    return false;
//  //printf("Cache miss %lld %d\n",sample,m_channel);
//  qint64 minS=qMax(sample-m_cacheSize/2,(qint64)0);
//  qint64 maxS=qMin(minS+m_cacheSize,m_size);
//  m_numCacheMiss++;
//  if(!readSamples(minS,maxS-minS,m_cacheX, m_cacheY))
//    return false;
////  if(DWGetScaledSamples(m_channel,minS,maxS-minS,m_cacheY,m_cacheX)!=DWSTAT_OK)
////    return false;
//  m_cacheStart=minS;
//  m_cacheEnd=maxS;
//  m_cacheUsed=m_cacheEnd-m_cacheStart;
//  //printf("Cache now is %lld %lld %d\n",m_cacheStart,m_cacheEnd,m_cacheUsed);
//  //fflush(stdout);
//  return true;
//}

////DW7DataFile &DW7CachedRead::getFile() const
////{
////    return m_file;
////}

//qint64 DW7CachedRead::misses()
//{
//  return m_numCacheMiss;
//}
//#include <QElapsedTimer>
//#include <winbase.h>
//qint64 readTime=0.;
//qint64 timerFreq=0;
//bool DW7CachedRead::readSamples(qint64 start, qint64 length, double *x, double *y)
//{
//#if defined(DEBUG_DATAFILE)
//    if(timerFreq==0)
//      QueryPerformanceFrequency((LARGE_INTEGER *)&timerFreq);
//    qint64 t0,t1;
//    QueryPerformanceCounter((LARGE_INTEGER *)&t0);
//#endif
//  bool ret=m_source->read(x,y,start, length);
//  //bool ret=(DWGetScaledSamples(m_channel,start,length,y,x)==DWSTAT_OK);
//#if defined(DEBUG_DATAFILE)
//  QueryPerformanceCounter((LARGE_INTEGER *)&t1);
//  qint64 readTimeOld=readTime;
//  readTime+=t1-t0;
//    if(floor(readTimeOld*1./timerFreq)<floor(readTime*1./timerFreq))
//      qDebug()<<"Read time"<<readTime*1./timerFreq<<m_numCacheMiss;
//#endif
//  m_numCacheMiss++;
//  return ret;
//}

//DW7DualCachedRead::DW7DualCachedRead(DW7DataFile &file, int channel): DW7CachedRead(file, channel), m_cacheBucket(1000)
//{
//  setCacheBucket(m_cacheSize);
//  setCacheSize(128*(2<<20)/(2*sizeof(double)*m_cacheSize)); // 128MB cache
//#if defined(DEBUG_DATAFILE)
//  qDebug()<<cacheSize();
//#endif
//}

//DW7DualCachedRead::DW7DualCachedRead(DW7DataSource *source): DW7CachedRead(source), m_cacheBucket(1000)
//{
//  setCacheBucket(m_cacheSize);
//  setCacheSize(128*(2<<20)/(2*sizeof(double)*m_cacheSize)); // 128MB cache
//}

//int DW7DualCachedRead::cacheBucket() const
//{
//    return m_cacheBucket;
//}

//void DW7DualCachedRead::setCacheBucket(int cacheBucket)
//{
//  m_cacheBucket = cacheBucket;
//  m_cache.clear(); // Invalidate cache
//}

//int DW7DualCachedRead::cacheSize() const
//{
//  return m_cache.maxCost();
//}

//void DW7DualCachedRead::setCacheSize(int cacheSize)
//{
//  m_cache.setMaxCost(cacheSize);
//}

//bool DW7DualCachedRead::readSamples(qint64 start, qint64 length, double *x, double *y)
//{
//  bool ret=true;
//  if(length>0)
//  {
//    qint64 bucketStart=start/m_cacheBucket;
//    qint64 bucketEnd=(start+length-1)/m_cacheBucket;
//    if(bucketStart==bucketEnd && m_cache.contains(bucketStart))
//    {
//      int offsetStart=start-bucketStart*m_cacheBucket;
//      // Fast lane... only one bucket and already in cache
//      //qDebug()<<"Hit";
//      m_cache[bucketStart]->write(x,y,offsetStart,length);
//    }
//    else
//    {
//      qint64 writePos=0;
//      for(qint64 bucket=bucketStart;bucket<=bucketEnd;bucket++)
//      {
//        qint64 offsetStart=start-bucket*m_cacheBucket;
//        int curL=qMin(m_cacheBucket-offsetStart, length);
//        if(m_cache.contains(bucket)) // Already in cache
//        {
//            //qDebug()<<"Hit";
//            m_cache[bucket]->write(&x[writePos], &y[writePos], offsetStart, curL);
//        }
//        else
//        {
//          // Cache miss
//          CachedData *data=new CachedData(m_cacheBucket);
//          qint64 bucketStartSample=bucket*m_cacheBucket;
//          qint64 bucketReadLen=qMin((qint64)m_cacheBucket,nSamples()-bucketStartSample);
//          if(!DW7CachedRead::readSamples(bucketStartSample, bucketReadLen, data->m_x.data(), data->m_y.data()))
//          {
//            ret=false;
//            break;
//          }
//          data->write(&x[writePos], &y[writePos], offsetStart, curL);
//          m_cache.insert(bucket, data);
//          //qDebug()<<"Miss"<<bucket<<m_cache.size();
//        }
//        writePos+=curL;
//        start+=curL;
//        length-=curL;
//      }
//    }
//  }
//  return ret;
//}

//DW7DualCachedRead::CachedData::CachedData(int size)
//{
//  m_x.resize(size);
//  m_y.resize(size);
//}

//void DW7DualCachedRead::CachedData::write(double *x, double *y, int start, int size)
//{
//  memcpy(x,&m_x[start],size*sizeof(*x));
//  memcpy(y,&m_y[start],size*sizeof(*y));
//}

//DW7DataSource::~DW7DataSource()
//{

//}

//bool DW7DataSource::get(qint64 index, double *x, double *y)
//{
//    return read(x, y, index, 1);
//}

//bool DW7DataSource::getSafe(qint64 index, double *x, double *y)
//{
//  if(index<0 || index>=length())
//    return false;
//  else
//    return get(index, x, y);
//}

//QList<double> DW7DataSource::arrayKeys(QString *reason)
//{
//  Q_UNUSED(reason);
//  return QList<double>();
//}

//DW7DataSourceChannel::DW7DataSourceChannel(DW7DataFile *file, int channel, int arraySize): m_file(file), m_channel(channel), m_arraySize(arraySize)
//{

//}

//DW7DataSourceComplexChannel::DW7DataSourceComplexChannel(DW7DataFile *file, int channel, int arraySize, ComplexPart complexPart): m_file(file), m_channel(channel), m_arraySize(arraySize), m_complexPart(complexPart)
//{

//}

//bool DW7DataSourceComplexChannel::read(double *x, double *y, qint64 start, qint64 size)
//{
//    qint64 ts=size*m_arraySize;
//    if(m_cache.size()<ts)
//        m_cache.resize(static_cast<int>(ts));
//    bool ret=m_file->complexChannelGet(m_channel, start, size, x, m_cache.data());
//    if(ret)
//    {
//        for(qint64 i=0;i<ts;i++)
//        {
//            double val;
//            DWComplex cur=m_cache[i];
//            //qDebug()<<"READ"<<cur.re<<cur.im;
//            switch(m_complexPart)
//            {
//            case Real:
//                val=cur.re; break;
//            case Imaginary:
//                val=cur.im; break;
//            default:
//                val=qSqrt(cur.im*cur.im+cur.re*cur.re);
//            }
//            y[i]=val;
//        }
//    }
//    return ret;
//}

//DW7DataSourceSubIndex::DW7DataSourceSubIndex(DW7DataSource *baseSource, int index): m_baseSource(baseSource), m_index(index)
//{
//  m_baseArraySize=baseSource->arraySize();
//}

//qint64 DW7DataSourceSubIndex::length()
//{
//  return m_baseSource->length();
//}

//bool DW7DataSourceSubIndex::read(double *x, double *y, qint64 start, qint64 size)
//{
//  int tSize=size*m_baseArraySize;
//  if(m_cache.size()<tSize)
//    m_cache.resize(tSize);
//  bool ret=m_baseSource->read(x, m_cache.data(), start, size);
//  if(ret)
//  {
//    for(qint64 i=0;i<size;i++)
//      y[i]=m_cache[i*m_baseArraySize+m_index];
//  }
//  return ret;
//}

//void DW7DataFilePrivate::debugChannelList(const QList<DWChannel> &list)
//{
//  for(int i=0;i<list.size();i++)
//    qDebug()<<i<<list[i].name<<list[i].index<<list[i].description<<list[i].data_type<<list[i].array_size<<list[i].index;
//}
