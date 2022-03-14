#include "dw7datafile.h"
#include "DWLoadLib.h"
#include <QtMath>
#include <QFileInfo>
#include <QDebug>
#include "dw7util.h"
#include "../Datafile/opener.h"
#
//#define DEBUG_DATAFILE

#include <QMutex>
#include <QList>
static QBasicMutex dw7Mutex;
// Dewesoft unused readers
static QList<int> dw7UnusedReaders;
// Dewesoft reader currently active
static int dw7CurrentReader;

using namespace DataFile;

bool DW7DataFile::m_initDeweLib=false;


#if defined(DEBUG_DATAFILE)
#include <QDebug>
#endif
//static FilePtr openDWFile(const QString &filename, DataFile::Error *error)
//{
//  return ;
//  //
//}
static DataFile::RegisterOpener dw7Opener("Dewesoft", QObject::tr("Dewesoft"), {"dxd"}, [](const QString &filename, Error *error){ return (FilePtr)DW7DataFile::openFile(filename, error); });

namespace DataFile
{
  class DWChannelList
  {
  public:
    DWChannelList()
    {

    }
    DWChannelList(QList<DWChannel> list)
    {
      m_list=list;
      for(int i=0;i<list.size();i++)
        m_map.insert(list[i].index, list[i]);
    }
    DWChannel getChannel(int i) const { return m_map.value(i); }
    inline bool isEmpty() const { return m_list.isEmpty(); }
    inline int size() const { return m_list.size(); }
    inline const DWChannel & operator [](int i) const {return m_list[i]; }
  protected:
    QList<DWChannel> m_list;
    QMap<int, DWChannel> m_map;
  };

  typedef QSharedPointer<DW7DataFile> DW7DataFilePtr;
  struct DW7DataFilePrivate
  {
    static QList<DWChannel> readChannelList(DW7DataFile *file, enum DWStatus (*readFunction)(struct DWChannel* ), int nChannels);
    static QList<DWEvent> readEventList(DW7DataFile *file);
    static void debugChannelList(const QList<DWChannel> &list);
    DWChannelList channels;
    DWChannelList complexChannels;
    DWChannelList textChannels;
    DWChannelList headerChannels;
    QList<DWEvent> events;
  };

  class DW7DataFileChannelList: public Entry
  {
  public:
    typedef std::function<EntryPtr(EntryPtr parent, int)> OpenFunction;
    DW7DataFileChannelList(EntryPtr parent, DW7DataFilePtr file, const DWChannelList &channels, OpenFunction openFunction);
    Children children() override;
    EntryPtr openINode(const INode &index) override;
  protected:
    DW7DataFilePtr m_file;
    const DWChannelList &m_channels;
    OpenFunction m_openFunction;
  };

  class DW7DataFileBaseChannel: public Entry
  {
    // Entry interface
  public:
    DW7DataFileBaseChannel(EntryPtr parent, DW7DataFilePtr file, int id);
    DataPtr data() override;
    QMap<QString, QVariant> metadata() override;
    Children children() override { return Children(); }
    EntryPtr openINode(const INode &inode) override { Q_UNUSED(inode); return nullptr; }
  protected:
    virtual const DWChannelList &channelList()=0;
    inline DW7DataFilePrivate *fileData() { return m_file->m_data; }
    DW7DataFilePtr m_file;
    int m_id;
  };

  class DW7DataFileScaledChannel: public DW7DataFileBaseChannel
  {
  public:
    DW7DataFileScaledChannel(EntryPtr parent, DW7DataFilePtr file, int id): DW7DataFileBaseChannel(parent, file, id) { }
    DataPtr data() override;
  protected:
    const DWChannelList &channelList() override { return fileData()->channels;}
  };

  class DW7DataFileComplexScaledChannel: public DW7DataFileBaseChannel
  {
  public:
    DW7DataFileComplexScaledChannel(EntryPtr parent, DW7DataFilePtr file, int id): DW7DataFileBaseChannel(parent, file, id) { }
  protected:
    const DWChannelList &channelList() override { return fileData()->complexChannels;}
  };

  DW7DataFileBaseChannel::DW7DataFileBaseChannel(EntryPtr parent, DW7DataFilePtr file, int id): Entry(parent), m_file(file), m_id(id)
  {
  }

  DataPtr DW7DataFileBaseChannel::data()
  {
    return DataPtr();
  }

  QMap<QString, QVariant> DW7DataFileBaseChannel::metadata()
  {
    QMap<QString, QVariant> ret;
    auto ch=m_file->m_data->channels.getChannel(m_id);
    ret.insert("Type",DW7Util::typeName(ch.data_type));
    if(ch.array_size!=1)
      ret.insert("Array size",ch.array_size);
    if(ch.description[0])
      ret.insert("Description",ch.description);
    if(ch.unit[0])
      ret.insert("Unit",ch.unit);
    ret.insert("Size", m_file->channelNumSamples(m_id));
    return ret;
  }

  DataPtr DW7DataFileScaledChannel::data()
  {
    return (new DW7DataSource(m_file, m_id))->sharedPtr();
  }
}

DW7DataFileChannelList::DW7DataFileChannelList(EntryPtr parent, DW7DataFilePtr file, const DWChannelList &channels, OpenFunction openFunction): DataFile::Entry(parent), m_file(file), m_channels(channels), m_openFunction(openFunction)
{

}

QMap<Entry::INode, Entry::Key> DW7DataFileChannelList::children()
{
  QMap<INode, Key> ret;
  for(int i=0; i<m_channels.size(); i++)
  {
    ret.insert(QString::fromUtf8(m_channels[i].name, strnlen(m_channels[i].name, sizeof(m_channels[i].name))), QString::number(m_channels[i].index));
  }
  return ret;
}

EntryPtr DW7DataFileChannelList::openINode(const INode &index)
{
  EntryPtr ret;
  bool ok;
  auto key=index.toInt(&ok);
  if(ok && m_openFunction)
    ret=m_openFunction(sharedFromThis(), key);
  return ret;
}

int DW7DataFile::libraryVersion()
{
  QMutexLocker lock(&dw7Mutex);
  return initializeInternal()?DWGetVersion():-1;
}

QString DW7DataFile::libraryVersionString()
{
  QMutexLocker lock(&dw7Mutex);
  QString msg;
  if(initializeInternal(&msg))
  {
    int version=DWGetVersion();
    return QString("%1.%2.%3.%4").arg(QString::number((version/1000000)%100),QString::number((version/10000)%100),QString::number((version/100)%100),QString::number(version%100));
  }
  else {
    return msg;
  }
}

bool DW7DataFile::initializeInternal(QString *message)
{
  if(!m_initDeweLib)
  {
    #ifdef WIN64
        //printf("WIN x64 Application\n");
      if (!LoadDWDLL(TEXT("DWDataReaderLib64.dll")))// Load DLL
    #else
      if (!LoadDWDLL("DWDataReaderLib.dll"))// Load DLL
    #endif
      {
        if(message)
          *message=QObject::tr("Failed INIT: Could not load dll object\n");
      }
      else if(DWInit()!=DWSTAT_OK)
      {
        if(message)
          *message=QObject::tr("Failed INIT: DWInit failed\n");
      }
      else
        m_initDeweLib=true;
      // At initialization we always have #0 reader available, free and active
      dw7UnusedReaders.clear();
      dw7UnusedReaders.append(0);
      dw7CurrentReader=0;
  }

  return m_initDeweLib;
}

bool DW7DataFile::checkStatus(DWStatus status, const char *msg)
{
  if(status!=DWSTAT_OK)
  {
    m_errorMsg=QObject::tr("%1: %2").arg(msg, DW7Util::statusToString(status));
  }
  return (status==DWSTAT_OK);
}

bool DW7DataFile::initialize(QString *message)
{
  QMutexLocker lock(&dw7Mutex);
  return initializeInternal(message);
}

DW7DataFile::DW7DataFile(const QString &filename): m_reader(-1), m_data(new DW7DataFilePrivate)
{
  QMutexLocker lock(&dw7Mutex);
  if(!initializeInternal(&m_errorMsg))
    return;
  // First check if there is a free reader, if not create a new one
  int currentReader=-1;
  if(dw7UnusedReaders.isEmpty())
  {
    if(checkStatus(DWAddReader(), "While opening file") && checkStatus(DWGetNumReaders(&currentReader), "While opening file"))
    {
      currentReader-=1;
      dw7UnusedReaders.append(currentReader);
    }
    else
      return;
  }
  else
  {
    currentReader=dw7UnusedReaders.last();
    if(dw7CurrentReader!=dw7UnusedReaders.last() && !checkStatus(DWSetActiveReader(currentReader), "SetActiveReader"))
      return;
  }
  DWFileInfo info;

  if(!checkStatus(DWOpenDataFile(DW7Util::stringToMultiByte(filename).data(), &info), "DWOpenDataFile")) //filename.toUtf8().data()
    return;

  m_data->channels=DW7DataFilePrivate::readChannelList(this, DWGetChannelList, DWGetChannelListCount());
  m_data->complexChannels=DW7DataFilePrivate::readChannelList(this, DWGetComplexChannelList, DWGetComplexChannelListCount());
  m_data->textChannels=DW7DataFilePrivate::readChannelList(this, DWGetTextChannelList, DWGetTextChannelListCount());
  m_data->headerChannels=DW7DataFilePrivate::readChannelList(this, DWGetHeaderEntryList, DWGetHeaderEntryCount());
  m_data->events=DW7DataFilePrivate::readEventList(this);
  m_filename=QFileInfo(filename).fileName();
  Q_ASSERT(dw7UnusedReaders.last()==currentReader);
  m_reader=currentReader;
  dw7UnusedReaders.takeLast();
}

QSharedPointer<DW7DataFile> DW7DataFile::openFile(const QString &filename, DataFile::Error *error)
{
  DW7DataFile *ret=new DW7DataFile(filename);
  if(!ret->isOpened())
  {
    if(error)
      *error=Error::Generic;
    delete ret;
    ret=nullptr;
  }
  return QSharedPointer<DW7DataFile>(ret);
}

DW7DataFile::~DW7DataFile()
{
  QMutexLocker lock(&dw7Mutex);
  if(ensureActiveReader())
  {
    DWCloseDataFile();
    dw7UnusedReaders.append(m_reader);
  }
  delete m_data;
}


bool DW7DataFile::isOpened()
{
  return (m_reader>=0);
}

bool DW7DataFile::ensureActiveReader()
{
  if(m_reader<0)
    return false;
  else if(dw7CurrentReader!=m_reader)
  {
    if(!checkStatus(DWSetActiveReader(m_reader),"DWSetActiveReader"))
      return false;
     dw7CurrentReader=m_reader;
     qDebug()<<"Set active reader ok"<<m_reader;
  }
  return true;
}

int DW7DataFile::numChannels()
{
  QMutexLocker lock(&dw7Mutex);
  return ensureActiveReader()?DWGetChannelListCount():-1;
}

qint64 DW7DataFile::channelNumSamples(int channel)
{
  QMutexLocker lock(&dw7Mutex);
  return ensureActiveReader()?DWGetScaledSamplesCount(channel):-1;
}

DataFile::Error DW7DataFile::channelGet(int channel, qint64 start, qint64 length, double *x, double *y)
{
  QMutexLocker lock(&dw7Mutex);
  if(!ensureActiveReader())
    return Error::Lock;
  return DW7Util::statusToError(DWGetScaledSamples(channel, start, length, y, x));
}

Entry::Children DW7DataFile::children()
{
  Children ret;
  if(!m_data->channels.isEmpty())
    ret.insert("Data", "1");
  if(!m_data->complexChannels.isEmpty())
    ret.insert("Complex", "2");
  if(!m_data->headerChannels.isEmpty())
    ret.insert("Header", "3");
  if(!m_data->textChannels.isEmpty())
    ret.insert("Text", "4");
//  ret.insert("\n\r\x06/ù\u00B1", "Complex");
  return ret;
}

DataFile::EntryPtr DW7DataFile::openINode(const INode &inode)
{
  DataFile::EntryPtr ret;
  auto key=inode.toInt();
  switch(key)
  {
  case 1:
    ret=DataFile::EntryPtr(
     new DW7DataFileChannelList(
       sharedEntry(),
       sharedFile(),
       m_data->channels,
       [this](EntryPtr parent, int id) { return (new DW7DataFileScaledChannel(parent, sharedFile(), id))->sharedEntry(); }
     )
   );
    break;
  case 2:
    ret=DataFile::EntryPtr(
      new DW7DataFileChannelList(
        sharedEntry(),
        sharedFile(),
        m_data->complexChannels,
        [this](EntryPtr parent, int id)  { return (new DW7DataFileComplexScaledChannel(parent, sharedFile(), id))->sharedEntry(); }
      )
    );
    break;
  case 3:
    ret=DataFile::EntryPtr(
      new DW7DataFileChannelList(
        sharedEntry(),
        sharedFile(),
        m_data->headerChannels,
        [this](EntryPtr parent, int id)  { return (new DW7DataFileComplexScaledChannel(parent, sharedFile(), id))->sharedEntry(); }
      )
    );
    break;
  case 4:
    ret=DataFile::EntryPtr(
      new DW7DataFileChannelList(
        sharedEntry(),
        sharedFile(),
        m_data->textChannels,
        [this](EntryPtr parent, int id)  { return (new DW7DataFileComplexScaledChannel(parent, sharedFile(), id))->sharedEntry(); }
      )
    );
    break;
  }
  return ret;
}


QList<DWChannel> DW7DataFilePrivate::readChannelList(DW7DataFile *file, DWStatus (*readFunction)(DWChannel *), int nChannels)
{
  QList<DWChannel> ret;
  QByteArray data;
  if(nChannels<0)
    file->m_errorMsg="Error in DWGetChannelListCount";
  else
  {
    data.resize(sizeof(DWChannel)*(unsigned int)nChannels);
    DWChannel *channels;
    channels=reinterpret_cast<DWChannel *>(data.data());
    if(file->checkStatus(readFunction(channels), "readChannelList"))
    {
      for(int n=0;n<nChannels;n++)
        ret.append(channels[n]);
    }
  }
  return ret;
}

QList<DWEvent> DW7DataFilePrivate::readEventList(DW7DataFile *file)
{
  QList<DWEvent> ret;
  QByteArray data;
  int nEvents=DWGetEventListCount();
  if(nEvents<0)
    file->m_errorMsg="Error in DWGetEventListCount";
  else
  {
    data.resize(sizeof(DWEvent)*(unsigned int)nEvents);
    DWEvent *events;
    events=reinterpret_cast<DWEvent *>(data.data());
    if(file->checkStatus(DWGetEventList(events), "readEventList"))
    {
      for(int n=0;n<nEvents;n++)
        ret.append(events[n]);
    }
  }
  return ret;
}

DW7DataSource::DW7DataSource(const QSharedPointer<DW7DataFile> &file, int channel): m_file(file), m_channel(channel), m_size(-1)
{

}

DataFile::Error DW7DataSource::size(qint64 &result)
{
  auto ret=DataFile::Error::Success;
  if(m_size<0)
  {
    m_size=m_file->channelNumSamples(m_channel);
    if(m_size<0)
      ret=DataFile::Error::Generic;
  }
  result=m_size;
  return ret;
}

DataFile::Error DW7DataSource::dataUncached(qint64 startSample, qint64 numSamples, double *x, double *y)
{
  return m_file->channelGet(m_channel, startSample, numSamples, x, y);
}


