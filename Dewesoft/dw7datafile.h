#ifndef DW7DATAFILE_H
#define DW7DATAFILE_H
#include <Qt>
#include <QVector>
#include <QCache>
#include "DWLoadLib.h"
#include "dw7data.h"
#include "../Datafile/file.h"
#include "../Datafile/datasource2d.h"
struct DataRecord
{
  double start;
  double end;
  double duration;
  QString reason;
  QList<DWEvent> events;
};
namespace DataFile
{
  class DW7DataFile;
  class DW7DataSource: public DataSource2D
  {
  public:
    DW7DataSource(const QSharedPointer<DW7DataFile> &file, int channel);
    DataFile::Error size(qint64 &result) override;
  protected:
    DataFile::Error dataUncached(qint64 startSample, qint64 numSamples, double *x, double *y) override;
    QSharedPointer<DW7DataFile> m_file;
    int m_channel;
    qint64 m_size;
  };




  //class DW7DataSourceNull: public DW7DataSource
  //{
  //public:
  //  virtual qint64 length(){return 2;}
  //  virtual bool read(double *x, double *y, qint64 start, qint64 size)
  //  {
  //    Q_UNUSED(start)
  //    for(;size>0; size--, x++, y++)
  //    {
  //      *x=*y=qQNaN();
  //    }
  //    return true;
  //  }
  //};
  struct DW7DataFilePrivate;
  class DW7DataFileBaseChannel;
  class DW7DataFile: public File
  {
    friend struct DW7DataFilePrivate;
    friend class DW7DataFileBaseChannel;
  protected:
    static bool initializeInternal(QString *message=nullptr);
    bool checkStatus(DWStatus status, const char *msg);
  public:
    static bool initialize(QString *message);
    // Return dewesoft library version or 0 if initialization failed
    static int libraryVersion();
    // Returns a string containing library version or a message indicating an error if initialization failed
    static QString libraryVersionString();

    inline QSharedPointer<DW7DataFile> sharedFile() { return sharedFromThis().staticCast<DW7DataFile>(); }
    DW7DataFile(const QString &datafile);
    static QSharedPointer<DW7DataFile> openFile(const QString &filename, Error *error=nullptr);
    ~DW7DataFile();
    //  qint64 numSamples(const QString &id);
    //  DW7DataSource *getSource(const QString &id);
    bool isOpened();
    bool ensureActiveReader();
    int numChannelsDebug();
    int numChannels();
    qint64 channelNumSamples(int channel);
    Error channelGet(int channel, qint64 start, qint64 length, double *x, double *y);
    inline Error channelNumSamples(int channel, qint64 &result) { return (result=channelNumSamples(channel))>=0?Error::Success:Error::Generic; }
    //  QStringList listDir(QString directory);
    //  QMap<QString, QVariant> metaData(const QString &directory);
    //  QMap<QString, QVariant> metaData(const QStringList &directory);

    //  QList<DataRecord> records(){return m_records;}
    //  QList<DataChannel> channels(){return m_channels;}
    //  bool hasChannelId(const QString &id){return m_channelById.contains(id);}
    //  bool hasChannelName(const QString &name){return m_idByName.contains(name);}
    //  DataChannel channelByName(const QString &name);
    //  DataChannel channelById(const QString &id);
    //  //DWChannel channelByIdEx(int id){DWChannel ret;memset(&ret,0,sizeof(ret));if(m_channelByIdEx.contains(id))ret=m_channelByIdEx[id]; return ret;}
    //  QString errorMsg(){return m_errorMsg;}
  private:
    QString m_errorMsg;
    // Id of dewesoft reader for this file or -1 in case of error
    int m_reader;
    DW7DataFilePrivate *m_data;
    //  QMap<int,qint64> m_numSamplesCache;
    //  QMap<int,qint64> m_numSamplesComplexCache;
    //  //QMap<QString,DWChannel> m_dwChannels;
    //  QList<DWEvent> m_events;
    //  QList<DataChannel> m_channels;
    //  QList<DataRecord> m_records;
    //  QMap<QString,DataChannel> m_channelById;
    //  QMap<QString, QString> m_idByName;
    //  //QMap<QString,DWChannel> m_channelByIdEx;
    ////  //QStringList m_channels;
    static bool m_initDeweLib;


    //private:
    //  friend class DW7DataSourceChannel;
    //  friend class DW7DataSourceComplexChannel;
    //  qint64 channelNumSamples(int channel);
    //  bool channelGet(int channel, qint64 start, qint64 length, double *x, double *y);
    //  QList<double> channelArrayKeys(int id, QString *reason=nullptr);

    //  qint64 complexChannelNumSamples(int channel);
    //  bool complexChannelGet(int channel, qint64 start, qint64 length, double *x, DWComplex *y);
    //  QList<double> complexChannelArrayKeys(int id, QString *reason=nullptr);

    //  //bool channelGetSingle(int channel, qint64 sample, double &x, double &y);
    //  //qint64 channelGetSampleBeforeTime(int channel,double time);
    //  //qint64 channelGetSampleAfterTime(int channel,double time);
    //  static bool initialize(QString *message=nullptr);
    //  DW7DataSource *getSourceForChannel(int channel, int width);

    //public:
    //  // Return dewesoft library version or 0 if initialization failed
    //  static int libraryVersion();
    //  // Returns a string containing library version or a message indicating an error if initialization failed
    //  static QString libraryVersionString();

    //  DW7DataFile(const QString &datafile);
    //  ~DW7DataFile();
    //  qint64 numSamples(const QString &id);
    //  DW7DataSource *getSource(const QString &id);
    //  bool isOpened();
    ////  QStringList listDir(QString directory);
    ////  QMap<QString, QVariant> metaData(const QString &directory);
    ////  QMap<QString, QVariant> metaData(const QStringList &directory);

    //  QString m_errorMsg;
    //  QList<DataRecord> records(){return m_records;}
    //  QList<DataChannel> channels(){return m_channels;}
    //  bool hasChannelId(const QString &id){return m_channelById.contains(id);}
    //  bool hasChannelName(const QString &name){return m_idByName.contains(name);}
    //  DataChannel channelByName(const QString &name);
    //  DataChannel channelById(const QString &id);
    //  //DWChannel channelByIdEx(int id){DWChannel ret;memset(&ret,0,sizeof(ret));if(m_channelByIdEx.contains(id))ret=m_channelByIdEx[id]; return ret;}
    //  QString errorMsg(){return m_errorMsg;}
    //private:
    //  bool m_opened;
    //  QMap<int,qint64> m_numSamplesCache;
    //  QMap<int,qint64> m_numSamplesComplexCache;
    //  //QMap<QString,DWChannel> m_dwChannels;
    //  QList<DWEvent> m_events;
    //  QList<DataChannel> m_channels;
    //  QList<DataRecord> m_records;
    //  QMap<QString,DataChannel> m_channelById;
    //  QMap<QString, QString> m_idByName;
    //  //QMap<QString,DWChannel> m_channelByIdEx;
    ////  //QStringList m_channels;
    //  static bool m_initDeweLib;


    // Entry interface
  public:
    Children children() override;
    DataFile::EntryPtr openINode(const INode &inode) override;
  };
}
//class DW7DataSourceChannel: public DW7DataSource
//{
//  friend class DW7DataFile;
//protected:
//  DW7DataSourceChannel(DW7DataFile *file, int channel, int arraySize);
//  DW7DataFile *m_file;
//  int m_channel;
//  int m_arraySize;
//public:
//  qint64 length()
//  {
//    return m_file->channelNumSamples(m_channel);
//  }
//  bool read(double *x, double *y, qint64 start, qint64 size)
//  {
//    return m_file->channelGet(m_channel, start, size, x, y);
//  }
//  QList<double> arrayKeys(QString *reason)
//  {
//    return m_file->channelArrayKeys(m_channel, reason);
//  }
//  int arraySize()
//  {
//    return m_arraySize;
//  }
//};

//class DW7DataSourceComplexChannel: public DW7DataSource
//{
//  friend class DW7DataFile;
//protected:
//  enum ComplexPart
//  {
//    Magnitude,
//    Real,
//    Imaginary,
//    Theta
//  };
//  DW7DataSourceComplexChannel(DW7DataFile *file, int channel, int arraySize, ComplexPart complexPart);
//  DW7DataFile *m_file;
//  int m_channel;
//  int m_arraySize;
//  ComplexPart m_complexPart;
//  QVector<DWComplex> m_cache;
//public:
//  qint64 length()
//  {
//    return m_file->complexChannelNumSamples(m_channel);
//  }
//  bool read(double *x, double *y, qint64 start, qint64 size);
//  QList<double> arrayKeys(QString *reason)
//  {
//    return m_file->complexChannelArrayKeys(m_channel, reason);
//  }
//  int arraySize()
//  {
//    return m_arraySize;
//  }
//};
//class DW7DataSourceSubIndex: public DW7DataSource
//{
//  friend class DW7DataFile;
//  DW7DataSourceSubIndex(DW7DataSource *baseSource, int index);
//  DW7DataSource *m_baseSource;
//  int m_index;
//  int m_baseArraySize;
//  QVector<double> m_cache;
//public:

//  // DW7DataSource interface
//public:
//  qint64 length();
//  bool read(double *x, double *y, qint64 start, qint64 size);
//};
////class DW7DataSourceComplexChannel: public DW7DataSource
////{
////  friend class DW7DataFile;
////protected:
////  DW7DataSourceChannel(DW7DataFile *file, int channel, int width);
////  DW7DataFile *m_file;
////  int m_channel;
////  int m_width;
////public:
////  qint64 length()
////  {
////    return m_file->channelNumSamples(m_channel);
////  }
////  bool read(double *x, double *y, qint64 start, qint64 size)
////  {
////    return m_file->channelGet(m_channel, start, size, x, y);
////  }
////  QList<double> arrayKeys(QString *reason)
////  {
////    return m_file->channelArrayKeys(m_channel, reason);
////  }
////  int width()
////  {
////    return m_width;
////  }
////};
//// 4s
//class DW7CachedRead
//{
//private:
//  DW7CachedRead(const DW7CachedRead &other){Q_UNUSED(other)}
//public:
//  DW7CachedRead(DW7DataSource *source);//, int channel);
//  virtual ~DW7CachedRead();
//  virtual double x(qint64 sample);
//  virtual double y(qint64 sample);
//  virtual double xUncached(qint64 sample);
//  virtual double yUncached(qint64 sample);
//  qint64 nSamples(){return m_size;}
//  qint64 getSampleBeforeTime(double time);
//  qint64 getSampleAfterTime(double time);
//  double getAt(double x);
//  //DW7DataFile &getFile() const;
//  qint64 misses();
//protected:
//  qint64 getSampleBeforeTime(double time,qint64 low,qint64 high);
//  qint64 getSampleAfterTime(double time,qint64 low,qint64 high);
//  inline bool check(qint64 sample)
//  {
//    if(sample<m_cacheStart || sample>=m_cacheEnd)
//    {
//      return load(sample);
//    }
//    return true;
//  }
//  inline bool inCache(qint64 sample)
//  {
//    return sample>=m_cacheStart && sample<m_cacheEnd;
//  }

//  bool load(qint64 sample);
//  virtual bool readSamples(qint64 start, qint64 length, double *x, double *y);
//  int m_cacheSize,m_cacheUsed;
//  QVector<double> m_cacheXData, m_cacheYData;
//  //QByteArray m_cacheXData,m_cacheYData;
//  double *m_cacheX,*m_cacheY;
//  //DW7DataFile &m_file;
//  DW7DataSource *m_source;
//  qint64 m_size;
//  //int m_channel;
//  qint64 m_cacheStart,m_cacheEnd;
//  qint64 m_numCacheMiss;
//};

//class DW7DualCachedRead: public DW7CachedRead
//{
//  struct CachedData
//  {
//    CachedData(int size);
//    void write(double *x, double *y, int start, int size);
//    QVector<double> m_x;
//    QVector<double> m_y;
//  };
//public:
//  DW7DualCachedRead(DW7DataSource *source);//DW7DataFile &file, int channel);
//  int cacheBucket() const;
//  void setCacheBucket(int cacheBucket);
//  int cacheSize() const;
//  void setCacheSize(int cacheSize);
//protected:
//  int m_cacheBucket;
//  QCache<int, CachedData> m_cache;

//  // DW7CachedRead interface
//protected:
//  bool readSamples(qint64 start, qint64 length, double *x, double *y);
//};

#endif // DW7DATAFILE_H
